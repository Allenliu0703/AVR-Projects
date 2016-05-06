/*
 * Temperature and Humidity Display on LCD.c
 *
 * Created: 4/20/2016 6:01:47 PM
 * Author : Allen
 */ 

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

/* LCD Commands*/
#define Clear_Screen 0x01
#define Return_Home 0x02
#define Decrement_Cursor 0x04
#define Increment_Cursor 0x06
#define DisplayCursor_On 0x0E
#define Cursor_1stline 0x80
#define Cursor_2ndline 0xC0
#define Set_Datalength 0x38
uint8_t pattern[8] = {0x08, 0x14, 0x08, 0x06, 0x09, 0x08, 0x09, 0x06};
/* PORTB */
#define DHT22_DATA PIN2 
#define LED_PIN PIN7
/* PORTC */
#define LCD_E PIN3
#define LCD_RS PIN5     
/* PORTD */
#define LCD_DATA0 PIN0 
#define LCD_DATA1 PIN1 
#define LCD_DATA2 PIN2 
#define LCD_DATA3 PIN3 
#define LCD_DATA4 PIN4 
#define LCD_DATA5 PIN5 
#define LCD_DATA6 PIN6 
#define LCD_DATA7 PIN7 

uint8_t Databuffer [40];
uint16_t temperature = 0;
uint16_t humidity = 0;
inline void initTimer1(void);
char firstline[4][16] = {"Temperat: ", "emperatu: ", "mperatur: ", "perature: "}; //8
char temperaturestring[6];
uint8_t temperaturestringlength = 0;
char secondline[16] = "Humidity: "; //10
uint8_t isNegative = 0;
/*DHT22 Interface functions*/
void Opendrain_Set(void);
void Opendrain_Clear(void);
uint8_t Start_DHT22(void);
uint8_t Datatransmit_DHT22(uint8_t Databuffer [40]);
uint8_t Dataconverter_DHT22(uint8_t Databuffer[40], uint16_t temperature, uint16_t humidity, uint8_t isNegative);
void DHT22_Measurement(void);
/*LCD Interface functions*/
void LCD_Init(void);
void LCD_Write_Command(uint8_t command);
void LCD_Write_Character(uint8_t character);
void LCD_Custom_Character(uint8_t CGRAM_Address, uint8_t pattern[8]);
void LCD_Write_String(char* firstline, char* secondline);
void LCD_Write_Firstline(char* firstline);
void LCD_Write_Secondline(char* secondline);
void Display_Data(uint8_t num);

int main(void)
{
	int counter = 0;
    DDRB |= (1<<LED_PIN);
	PORTB |= (1<<LED_PIN);
	LCD_Init();
	Opendrain_Set();
	initTimer1();
	PORTB &= ~(1<<LED_PIN);
	LCD_Write_String("Initializing...","");
	_delay_ms(1000);
	DHT22_Measurement();
	while(1){
		if(counter%2){
			DHT22_Measurement();
		}
		if(counter>=4){
			counter = 0;
		}
		Display_Data(counter);
		_delay_ms(1000);
		counter ++;
	}
}

inline void initTimer1(void) {
	/* Normal mode (default), just counting */
	TCCR1B |= (1 << CS10);
	/* Clock speed: 1 MHz, no prescaler
	each tick is 1 microseconds */
}

/*Initialize LCD Pins and Ports*/
void LCD_Init(void){
	DDRD = 0xFF;
	DDRC |= (1<<LCD_E)|(1<<LCD_RS);
	PORTC |= (1<<LCD_E);
	LCD_Custom_Character(0, pattern);
	LCD_Write_Command(Set_Datalength);
	LCD_Write_Command(0x0C); /*Display on cursor off*/
	LCD_Write_Command(Clear_Screen);
}

void LCD_Write_Command(uint8_t command){
	PORTC |= (1<<LCD_E);
	PORTD = command;
	PORTC &= ~(1<<LCD_RS);
	_delay_ms(5);
	PORTC &= ~(1<<LCD_E);
	_delay_ms(5);
	PORTC |= (1<<LCD_E);
}

void LCD_Write_Character(uint8_t character){
	PORTC |= (1<<LCD_E);
	PORTD = character;
	PORTC |= (1<<LCD_RS);
	_delay_ms(1);
	PORTC &= ~(1<<LCD_E);
	_delay_ms(1);
	PORTC |= (1<<LCD_E);
}
void LCD_Custom_Character(uint8_t CGRAM_Address, uint8_t pattern[8]){
	if (CGRAM_Address < 8){
		LCD_Write_Command((CGRAM_Address*8)+0x40);
		for(int i=0; i<8; i++){
			LCD_Write_Character(pattern[i]);
		}
		LCD_Write_Command(0x80);
	}
}
void LCD_Write_String(char* firstline, char* secondline){
	LCD_Write_Command(Clear_Screen);
	LCD_Write_Firstline(firstline);
	LCD_Write_Secondline(secondline);
}

void LCD_Write_Firstline(char* firstline){
	int i = 0;
	LCD_Write_Command(Cursor_1stline);
	while(*(firstline+i) != '\0'){
		LCD_Write_Character((uint8_t)firstline[i]);
		i++;
	}
}

void LCD_Write_Secondline(char* secondline){
	int i = 0;
	LCD_Write_Command(Cursor_2ndline);
	while(*(secondline+i) != '\0'){
		LCD_Write_Character((uint8_t)secondline[i]);
		i++;
	}
}
void Display_Data(uint8_t num){
	int i = 0;
	LCD_Write_Firstline(firstline[num]);
	while(i!=temperaturestringlength){
		LCD_Write_Character(temperaturestring[i]);
		i++;
	}
	LCD_Write_Secondline(secondline);
}
/*Set DHT22 DATA PIN to input no pull-up resistor mode, which sends 1 to Data Pin*/
void Opendrain_Set(void){
	DDRB &= ~(1<<DHT22_DATA);
	PORTB &= ~(1<<DHT22_DATA);
}
/*Set DHT22 DATA PIN to output mode and output 0, which pull the DATA wire to low*/
void Opendrain_Clear(void){
	DDRB |= (1<<DHT22_DATA);
	PORTB &= ~(1<<DHT22_DATA);
}
/*Proceed the start process of DHT22*/
uint8_t Start_DHT22(void){
	_delay_ms(1000);
	Opendrain_Set();
	_delay_ms(1);
	Opendrain_Clear();
	_delay_ms(1);
	Opendrain_Set();
	_delay_us(40);
	TCNT1 = 0;
	while((PINB&(1<<DHT22_DATA))!=0x00){ /*wait for Data Pin to go low */
		if (TCNT1 >80){
			return 0;
		}
	}
	TCNT1 = 0;
	while(!(PINB&(1<<DHT22_DATA))){ /*wait for Data Pin to go high*/
		if (TCNT1 >160){
			return 0;
		}
	}
	while((PINB&(1<<DHT22_DATA))!=0x00); /*wait for Data Pin to go low*/
	return 1;
}
/*Proceed the data transmit process of DHT22*/
uint8_t Datatransmit_DHT22(uint8_t Databuffer [40]){
	for(int i = 0; i<40; i++){
		while(!(PINB&(1<<DHT22_DATA))); /*wait for Data Pin to go high*/
		TCNT1 = 0;
		while((PINB&(1<<DHT22_DATA))!=0x00){ /*wait for Data Pin to go high*/
			if(TCNT1 >= 1000){
				return 0;
			}
		}
		if(TCNT1>50){
			Databuffer[i] = 1;
		}else{
			Databuffer[i] = 0;
		}
	}
	return 1;
}

uint8_t Dataconverter_DHT22(uint8_t Databuffer[40], uint16_t temperature, uint16_t humidity, uint8_t isNegative){
	uint8_t sum = 0;
	uint8_t checksum = 0;
	uint16_t temp = 0;
	uint16_t hum = 0;
	for(int i=0; i<16; i++){
		temp |= (Databuffer[i]<<(15-i));
	}
	for(int i=16; i<32; i++){
		hum |= (Databuffer[i]<<(31-i));
	}
	for(int i=32; i<40; i++){
		checksum |= (Databuffer[i]<<(39-i));
	}
	sum = (temp>>8)+(temp&(0xFF))+(hum>>8)+(hum&(0xFF));
	if(sum == checksum){
		humidity = hum;
		if((temp>>15)&1)
		{
			temp &= ~(1<<15);
			temperature =temp;
			isNegative = 1;
		}else{
			temperature = temp;
		}
		return 1;
	}else{
		temperature = 0;
		humidity = 0;
		isNegative = 0;
		return 0;
	}
}

void DHT22_Measurement(void){
	uint8_t sum = 0;
	uint8_t checksum = 0;
	uint16_t temp = 0;
	uint16_t hum = 0;
	if(Start_DHT22()){
		if(Datatransmit_DHT22(Databuffer)){
			for(int i=0; i<16; i++){
				hum |= (Databuffer[i]<<(15-i));
			}
			for(int i=16; i<32; i++){
				temp |= (Databuffer[i]<<(31-i));
			}
			for(int i=32; i<40; i++){
				checksum |= (Databuffer[i]<<(39-i));
			}
			sum = (temp>>8)+(temp&(0xFF))+(hum>>8)+(hum&(0xFF));
			if(sum == checksum){
				humidity = hum;
				if((temp>>15))
				{
					temp &= ~(1<<15);
					temperature =temp;
					isNegative = 1;
					}else{
					temperature = temp;
				}
				}else{
				temperature = 0;
				humidity = 0;
				isNegative = 0;
			}

			int i = 0;
			if(humidity/100){
				secondline[10+i] = '0'+(humidity/100);
				i++;
			}
			if(humidity/10){
				secondline[10+i] = '0'+((humidity%100)/10);
				i++;
			}
			secondline[10+i] = '.';
			secondline[11+i] = '0'+(humidity%10);
			secondline[12+i] = '%';
			secondline[13+i] = '\0';
			i = 0;
			if (isNegative){
				temperaturestring[i] = '-';
				i++;
			}
			if(temperature/100){
				temperaturestring[i] = '0'+(temperature/100);
				i++;
			}
			if(temperature/10){
				temperaturestring[i] = '0'+((temperature%100)/10);
				i++;
			}
			temperaturestring[i] = '.';
			temperaturestring[i+1] = '0'+(temperature%10);
			temperaturestring[i+2] = '\0';
			temperaturestringlength = i+3;
			}else{
			LCD_Write_String("Data Wrong","");
		}
		}else{
		LCD_Write_String("DHT Init Fail","");
	}
}