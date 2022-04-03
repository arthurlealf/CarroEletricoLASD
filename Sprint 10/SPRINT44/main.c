//Arthur de Farias Leal - 121111391
//Laboratório de Arquitetura de Sistemas Digitais - Turma 7
//Sprint 10

#include "SSD1306.h"
#include "Font5x8.h"
#include <avr/eeprom.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#define F_CPU 16000000UL
#define  BAUD 9600
#define MYUBRR  F_CPU/16/BAUD-1

char eprom_str[2];
uint16_t temp2=0, bateria=0, pinoC=0, PWMtemperatura=0, temp=0, PWMbateria=0, diam_pneu=65, freq=0, hodometro=0, hodometro2=0, vel_media=0, tempoX=0, r=0, centena=0, dezena=0, unidade=0, borda=0, adc_=0, pwm_=0;//a variavel borda representa o instante em que há a interrupção (subida-descida)
uint32_t subida, tempoD;
char display[16];

ISR(INT0_vect){
	diam_pneu++;
	eeprom_write_byte(0, diam_pneu);
}

ISR(INT1_vect){
	diam_pneu--;
	eeprom_write_byte(0, diam_pneu);
}

ISR(TIMER0_COMPA_vect){
	static float cont;
	tempoX++;
	
	if((tempoX % 1000) == 0) //quando alcança 1seg.
	{
		cont = (borda/2); //subida e descida
		freq = (cont*60); //colocando a frequencia em rpm (1min - 60s)
		borda = 0; //reinicializando
		
		hodometro2++;
	}
}

ISR(PCINT2_vect){
	borda++;
}

ISR(ADC_vect){
	if(pinoC==0){
		adc_ = ADC;
		if (ADC==1023){
			pwm_ = 255;
		}
		else {
			pwm_ = (ADC/4);
		}
		if(tempoD<=300 && vel_media>19){
			pwm_=100;
		}
	}
	
	if(pinoC==1){
		if (ADC==1023){
			PWMtemperatura = 255;
			temp=200;
		}
		else{
			PWMtemperatura= (ADC/4);
			temp=(PWMtemperatura-105)/0.35;
		}
		
		//NOVA FUNCIONALIDADE
		if (temp > 150){
			PORTC |= 0b00001000;
		}
		else{
			PORTC &= 0b11110111;
		}
	}
	
	if(pinoC==2){
		if (ADC==1023){
			PWMbateria = 255;
			bateria=100;
		}
		else{
			PWMbateria= (ADC/4);
			bateria=(PWMbateria*100)/255;
		}
	}
}

ISR(TIMER1_CAPT_vect){
	
	if(TCCR1B &(1<<ICES1)) //lendo o valor de contagem do TC1 na borda de subida 
	subida=ICR1; //salva a 1a contagem para determinar a larg. de pulso
	
	else //lendo o valor de contagem do TC1 na borda de descida do sinal
	tempoD=((ICR1-subida)*16)/58;
	TCCR1B^=(1<<ICES1); //invertendo a borda
	
}

void USART_Init(unsigned int ubrr){
	
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L=(unsigned char)ubrr;
	UCSR0B =  (1<<RXCIE0) | ( 1<<RXEN0) | (1<<TXEN0);
	UCSR0C =(1<<USBS0)|(3<<UCSZ00);

}

void USART_Transmit(unsigned char data){
	
	while(!( UCSR0A & (1<< UDRE0)));
	UDR0 = data;
}

unsigned char USART_Receive (void){
	 
	 while(!(UCSR0A & (1<<RXC0)));
	 return UDR0;
 }
 
ISR(USART_RX_vect){
	int print=0;
	char comando;
	comando=UDR0;
	
	sprintf(eprom_str, "%u", temp2);
	if(comando=='l'){
		eeprom_write_byte(3,0);
	}
	
	if(comando=='d'){
		if (temp2<10){
			print=1;
		}
		else if (temp2<100){
			print=2;
		}
		else{
			print=3;
		}
		
		for (int cont=0;cont<print;cont++){
			USART_Transmit(eprom_str[cont]);
		}
	}
	
	//NOVA FUNCIONALIDADE
	if (comando=='f'){ 
		PORTC |= 0b01000000; //ligar o farol
	}
	if (comando=='s'){
		PORTC &= 0b10111111; //desligar o farol
	}
}
int main(void){
	DDRB  = 0b11111110;
	PORTB = 0b00000001;
	DDRC  = 0b11111000;
	PORTC = 0b10110000;
	DDRD  = 0b00000001;
	PORTD = 0b01111101;
	
	TCCR0A = 0b00000010;
	TCCR0B = 0b00000011;
	OCR0A = 249;
	TIMSK0 = 0b00000010;
	TCCR1B= (1<<ICES1)|(1<<CS12);
	TIMSK1 = 1<<ICIE1;
	
	
	EICRA = 0b00001010; //borda de descida INT0 e INT1
	EIMSK = 0b00000011; //habilitando INT0 e INT1
	
	PCICR = 0b00000100;
	PCMSK2 = 0b10000000;
	
	ADMUX = 0b11000000;
	ADCSRA = 0b11101111;
	ADCSRB = 0x00;
	DIDR0 =  0b00111000;
	
	TCCR0A = 0b10100011;
	TCCR0B = 0b00000011;
	
	USART_Init(MYUBRR);
	
	sei(); //habilitando interrupções globais
	
	//nokia_lcd_init(); //inicializando
	
	GLCD_Setup(); //inicializando a biblioteca
	GLCD_SetFont(Font5x8,5,8, GLCD_Overwrite);
	GLCD_InvertScreen(); //invertendo o background
		
	
	while (1){
		r = diam_pneu/2;
		
		vel_media = 2*3.14*(diam_pneu/2)/100000*freq*60;
		hodometro = hodometro2*vel_media/3600;
		
		OCR0A = pwm_;
		diam_pneu= eeprom_read_byte(0);
		
		GLCD_Clear();
		
		ADMUX = 0b11000000;
		pinoC=0;
		_delay_ms(100);
		
		ADMUX = 0b11000010;
		pinoC=2;
		_delay_ms(100);
		GLCD_GotoXY(120,0);
		GLCD_PrintString("%");
		GLCD_GotoXY(105,0);
		GLCD_PrintInteger(bateria);
		
		ADMUX = 0b11000001;
		pinoC=1;
		_delay_ms(100);
		GLCD_GotoXY(120,15);
		GLCD_PrintString("C");
		GLCD_GotoXY(100,15);
		GLCD_PrintInteger(temp);
		
		temp2 = eeprom_read_byte(3);
		if (temp>temp2){
			eeprom_write_byte(3,temp);
		}
		
		GLCD_GotoXY(0,0);
		GLCD_PrintString("LACD Car");
		
		GLCD_GotoXY(0,15);
		GLCD_PrintInteger(freq);
		GLCD_GotoXY(30,15);
		GLCD_PrintString("rpm");
		
		GLCD_GotoXY(0,28);
		GLCD_PrintString("Diam.:");
		GLCD_GotoXY(45,28);
		GLCD_PrintInteger(diam_pneu);
		
		GLCD_GotoXY(0,41);
		GLCD_PrintString("Sonar:");
		GLCD_GotoXY(45,41);
		GLCD_PrintInteger(tempoD);
		

		GLCD_GotoXY(0,54);
		GLCD_PrintString("Dist.:");
		GLCD_GotoXY(45,54);
		GLCD_PrintInteger(hodometro2);
		
		GLCD_GotoXY(65,54);
		GLCD_PrintString("km");
		
		GLCD_GotoXY(120,105);
		GLCD_PrintString("D");
		
		
		//NOVA FUNCIONALIDADE
		while(temp > 150){
			GLCD_Clear();
			GLCD_GotoXY(35,20);
			GLCD_PrintString("ALERTA DE");
			GLCD_GotoXY(25,35);
			GLCD_PrintString("AQUECIMENTO!!!");
			GLCD_Render();
		}
		
		//GLCD_FillRectangle(70,15,70,40,0x00);
		//GLCD_DrawRectangle(70,15,70,40,0x00); tentei desenhar o retangulo mas não consegui 
		
		GLCD_Render();
	
		PORTB &= 0b00000001;
		PORTB |= 0b11000000;
		centena = ((((vel_media)%10)*2) & 0b00011110);
		PORTB |= centena;
		_delay_ms(1);
		
		PORTB &= 0b00000001;
		PORTB |= 0b10100000;
		dezena = ((((vel_media/10)%10)*2) & 0b00011110);
		PORTB |= dezena;
		_delay_ms(1);
		
		PORTB &= 0b00000001;
		PORTB |= 0b01100000;
		unidade = ((((vel_media/100)%10)*2) & 0b00011110);
		PORTB |= unidade;
		_delay_ms(1);
		
	}
}