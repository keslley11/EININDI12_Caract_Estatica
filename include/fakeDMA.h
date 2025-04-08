#ifndef FAKE_DMA_H
#define FAKE_DMA_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef FAKE_DMA_BUFFER_SIZE
#define FAKE_DMA_BUFFER_SIZE 64
#endif

void fakeDMA_init(uint8_t analog_pin);
void fakeDMA_start();
void fakeDMA_stop();
uint8_t fakeDMA_available();
uint16_t fakeDMA_read();

#endif

static volatile uint16_t buffer[FAKE_DMA_BUFFER_SIZE];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;
static uint8_t analog_channel = 0;

void fakeDMA_init(uint8_t analog_pin) {
    analog_channel = analog_pin;
    // Configura Timer1 para 1kHz
    cli();
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << WGM12); // CTC mode
    TCCR1B |= (1 << CS11);  // Prescaler = 8 (16MHz/8 = 2MHz)
    OCR1A = 1999; // 2MHz / (1999 + 1) = 1kHz
    TIMSK1 |= (1 << OCIE1A); // Interrupção compare match
    sei();
}

void fakeDMA_start() {
    // Liga interrupção do Timer1
    TIMSK1 |= (1 << OCIE1A);
}

void fakeDMA_stop() {
    // Desliga interrupção do Timer1
    TIMSK1 &= ~(1 << OCIE1A);
}

uint8_t fakeDMA_available() {
    return head != tail;
}

uint16_t fakeDMA_read() {
    if (head == tail) return 0;  // buffer vazio
    uint16_t val = buffer[tail];
    tail = (tail + 1) % FAKE_DMA_BUFFER_SIZE;
    return val;
}

ISR(TIMER1_COMPA_vect) {
    // Configura o canal ADC
    ADMUX = (ADMUX & 0xF0) | (analog_channel & 0x0F); // Seleciona canal
    ADCSRA |= (1 << ADEN);  // Habilita ADC
    ADCSRA |= (1 << ADSC);  // Inicia conversão
    while (ADCSRA & (1 << ADSC)); // Espera terminar
    uint16_t value = ADC;
    buffer[head] = value;
    head = (head + 1) % FAKE_DMA_BUFFER_SIZE;
    if (head == tail) {
        tail = (tail + 1) % FAKE_DMA_BUFFER_SIZE; // Evita overflow
    }
}
