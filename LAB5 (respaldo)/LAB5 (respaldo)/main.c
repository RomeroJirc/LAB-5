/*
 * LAB5.c
 *
 * Created: 12/04/2024 05:39:43 a. m.
 * Author : josei
 */ 


//LIBRERIAS
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

//VARIABLES
uint8_t ADCvalue = 0;

//PROTOTIPOS
void initADC(void);
void initTimer0(void);

int main(void)
{
	cli();
    DDRB |= (1<<DDB1);	//PIN B1 COMO SALIDA
	TCCR1A = 0;
	TCCR1B = 0;
	
	TCCR1A |= (1<<COM1A1);				//OC1A NO INVERTIDO
	TCCR1A |= (1<<WGM10);				//MODO PWM FAST, 8 bits
	TCCR1B |= (1<<WGM12);
	
	TCCR1B |= (1<<CS12)|(1<<CS10);		//PRESCALER DE 1024
	
	initTimer0();						//LLAMAR A LA INICIALIZACIÓN DEL TIMER1
	initADC();							//LLAMAR A LA INICIALIZACIÓN DEL ADC
	
	sei();
	
    while (1) 
    {
	OCR1A =  ADCvalue/6;	
    }
}

void initADC(void){
	ADMUX = 0;
	ADMUX |= (1<<REFS0);				//REFERENCIA AVCC = 5V
	ADMUX |= (1<<ADLAR);				//JUSTIFICACIÓN A LA IZQUIERDA
	ADMUX |= ((1<<MUX2) | (1<<MUX1));	//AL COLOCAR 0110 INDICAMOS EL ADC6 COMO EL RECEPTOR DE LA SEÑAL ANALÓGICA
	
	ADCSRA = 0;
	ADCSRA |= (1<<ADIE);				//HABILITaR INTERRUPCIONES DE ADC
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	//PRESCALER DE 128 ----> 125kHz
	ADCSRA |= (1<<ADEN);				//HABILITANDO EL ADC
	
}

void initTimer0(void){
	TCCR0B |= (1<<CS02)|(1<<CS00);;				//CONFIGURAR PRESCALER DE 1024
	TCNT0 = 255;								//CARGAR VALOR DE DESBORDAMIENTO
	TIMSK0 |= (1 << TOIE0);
}

ISR (TIMER0_OVF_vect){
	TCNT0 = 255;						//CARGAR VALOR DE DESBORDAMIENTO
	TIFR0 |= (1 << TOV0);				//APAGAR BANDERA DEL TIMER1
	ADCSRA |= (1<<ADSC);				//ADC START CONVERSION
}

ISR (ADC_vect){
	
	ADCvalue = ADCH;					//ACTUALIZAR VALOR DEL ADC
	ADCSRA |= (1<<ADIF);				//APAGAR BANDERA DE INTERRUPCIÓN
}