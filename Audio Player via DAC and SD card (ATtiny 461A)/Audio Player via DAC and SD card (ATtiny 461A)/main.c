/*
 * Audio Player via DAC and SD card (ATtiny 461A).c
 *SD Card Info:
 *Bytes per sectors : 512B
 *Sectors per cluster: 8
 *Reserved sectors: 32
 *number of FATs: 2
 *number of sectors per FAT:15273
 *
 *root cluster: #2
 *root directory sector: 0
 *FirstDataSector = BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors;
 *
 *first data sector: 32 + 15273x2 + 0 = 30578
 *
 *FirstSectorofCluster = ((N – 2) * BPB_SecPerClus) + FirstDataSector;
 *
 *Root directory offset: 22rows
 *
 *To find the cluster number of the file:
 *  1. Find the 4th of the file descripter.
 *  2. High word = fifth byte and 6th bytes.
 *  3. Low word = 11th and 12th bytes.
 * Created: 4/12/2016 10:03:13 PM
 * Author : Allen
 */ 
 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#define CS (1<<PB6)
#define DATAOUT (1<<PB1)
#define DATAIN (1<<PB0)
#define SCK (1<<PB2)
#define CS_ENABLE() (PORTB &= ~CS)
#define CS_DISABLE() (PORTB |= CS)
#define delay 62

void SPI_Init(void);
uint8_t SPI_Write(uint8_t data);
void SD_Command(unsigned char cmd, unsigned long arg, unsigned char crc, uint16_t read);
void SD_Read(unsigned long sector, unsigned short offset, unsigned char * buffer, unsigned short len);
void SD_Write(unsigned long sector, unsigned char * buffer, unsigned short len) ;
void SD_read_alarm(void);
void SD_read_leilei(void);
volatile uint8_t x[256];


int main(void)
{
	uint8_t* buffer;
    DDRA = 0xFF;
	SPI_Init();
	//_delay_ms(2000);
	while (1)
	{
		CS_DISABLE();
		for(int i=0; i<10; i++) // idle for 1 bytes / 80 clocks
			SPI_Write(0xFF);
		SD_Command(0x40, 0x00000000, 0x95, 8);//CMD0 to reset the SD card.
		SD_Command(0x48, 0x000001AA, 0x87, 8);//CMD8 to support SDHC card.
		SD_Command(0x7A, 0x00000000, 0xFF, 8);//CMD58 check OCR
		SD_Command(0x77, 0x00000000, 0x65, 8);//CMD55
		SD_Command(0x69, 0x40000000, 0x5F, 8);//ACMD41
		SD_Command(0x77, 0x00000000, 0x65, 8);//CMD55
		SD_Command(0x69, 0x40000000, 0xFF, 8);//ACMD41
		SD_Command(0x7A, 0x00000000, 0xFF, 8);//CMD58 check OCR

		//SD_read_alarm();

		SD_read_leilei();
		_delay_ms(5000);
	}	
//	SD_Read(0, 0, *buffer, 512);
	//SD_Write(50000, *buffer, 512);
    while (1) 
    {
		/*
		CS_ENABLE();
		SPI_Write(255);
		CS_DISABLE();
		CS_ENABLE();
		SPI_Write(0);
		CS_DISABLE();
		*/
		//General triangle wave from SD card port
		for (int i=0; i<255; i++){
			PORTA = i;
		}
		for (int i = 255; i>0; i--){
			PORTA = i;
		}
		
	}
	return 0;
}

void SPI_Init(void){
	DDRB |= DATAOUT | CS | SCK;
	DDRB &= ~DATAIN;
	PORTB |= DATAIN; //pull-up mode 
	USICR = 1<<USIWM0; //3 wire mode
}

uint8_t SPI_Write(uint8_t data) {
	
	USIDR = data;
	for (uint8_t i =0; i<8; i++){
		USICR = (1<<USIWM0)|(1<<USITC);
		USICR = (1<<USIWM0)|(1<<USICLK)|(1<<USITC);
	}
	return USIDR;
	
	//Official method from datasheet
	/*
	USIDR = data;
	USISR = 1<<USIOIF; // clear flag

	while ( (USISR & (1<<USIOIF)) == 0 ) {
		USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);
	}
	return USIDR;
	*/
}
/* Root directory sector is at 0x00 00 20 00  ( 8192 )
   FAT sector is at 0x00 00 20 00 + 0x32
   First data sector is at 0x00 00 97 72  ( 30578 + 0x2000 )
   Audio file locates at sector 0x00 00 9E 4A (0x00 00 7E 4A + 0x00 00 20 00)
   The end of the audio file cluster locates at sector 0x00 00 9E 82 ((228-2)*8+30578+8192)
   Audio file ends at sector 0x00 00 9E 88
*/
void SD_read_alarm(void){
	int x = 62;
	CS_ENABLE();
	SPI_Write(0x52);
	SPI_Write(0x00);
	SPI_Write(0x00);
	SPI_Write(0x9E);
	SPI_Write(0x4A);
	SPI_Write(0xFF);

	for(int i=0; i<100 && SPI_Write(0xFF) != 0x00; i++) {} // wait for 
    for(int i=0; i<100 && SPI_Write(0xFF) != 0xFE; i++) {} // wait for data start

/*Skip the header of wav file*/
	for(int i=0; i<58; i++){
		//x[i]=SPI_Write(0xFF);
		SPI_Write(0xFF);
	}
/*Read the data in first sector*/
	for(int i=58; i<256; i++){
		//x[i]=SPI_Write(0xFF);
		PORTA = SPI_Write(0xFF);
		_delay_us(200);
	}
//(void)x[0];
	for(int i=0; i<256; i++){
		//x[i]=SPI_Write(0xFF);
		PORTA = SPI_Write(0xFF);
		_delay_us(200);
	}
//(void)x[0];
	for(int i=0; i<50 && SPI_Write(0xFF) != 0xFE; i++) {} // wait for data start

/* Run 61 cycles to read the middle 61 sectors*/
for (int j=0; j<=60; j++){
	for(int i=0; i<256; i++){
		//x[i]=SPI_Write(0xFF);
		PORTA = SPI_Write(0xFF);
		_delay_us(200);
	}
//(void)x[0];
	for(int i=0; i<256; i++){
		//x[i]=SPI_Write(0xFF);
		PORTA = SPI_Write(0xFF);
		_delay_us(200);
	}
//(void)x[0];
	for(int i=0; i<50 && SPI_Write(0xFF) != 0xFE; i++) {} // wait for data start
}

/*Read for the last sector of the audio file*/
	for(int i=0; i<256; i++){
		//x[i]=SPI_Write(0xFF);
		PORTA = SPI_Write(0xFF);
		_delay_us(200);
	}
//(void)x[0];
	for(int i=0; i<256; i++){
		//x[i]=SPI_Write(0xFF);
		PORTA = SPI_Write(0xFF);
		_delay_us(200);
	}
//(void)x[0];
    for(int i=0; i<50 && SPI_Write(0xFF) != 0xFE; i++) {} // wait for data start

	SPI_Write(0xFF);//for CRC
	SPI_Write(0xFF);//for CRC
	CS_DISABLE();
}
/* Root directory sector is at 0x00 00 20 00  ( 8192 )
   FAT sector is at 0x00 00 20 00 + 0x32
   First data sector is at 0x00 00 97 72  ( 30578 + 0x2000 )
   Audio file locates at sector 0x00 00 99 BA (0x00 00 79 BA + 0x00 00 20 00)
   The end of the audio file cluster locates at sector 0x00 00 ?? ?? ((95-2)*8+30578+8192)
   Audio file ends at sector 0x00 00 9A 5B
*/
void SD_read_leilei(void){
	CS_ENABLE();
	SPI_Write(0x52);
	SPI_Write(0x00);
	SPI_Write(0x00);
	SPI_Write(0x99);
	SPI_Write(0xBA);
	SPI_Write(0xFF);

	for(int i=0; i<100 && SPI_Write(0xFF) != 0x00; i++) {} // wait for
	for(int i=0; i<100 && SPI_Write(0xFF) != 0xFE; i++) {} // wait for data start

	/*Skip the header of wav file*/
	for(int i=0; i<58; i++){
		//x[i]=SPI_Write(0xFF);
		SPI_Write(0xFF);
	}
	/*Read the data in first sector*/
	for(int i=58; i<256; i++){
		//x[i]=SPI_Write(0xFF);
		PORTA = SPI_Write(0xFF);
		_delay_us(59);
	}
	//(void)x[0];
	for(int i=0; i<256; i++){
		//x[i]=SPI_Write(0xFF);
		PORTA = SPI_Write(0xFF);
		_delay_us(59);
	}
	//(void)x[0];
	for(int i=0; i<50 && SPI_Write(0xFF) != 0xFE; i++) {} // wait for data start

	/* Run 159 cycles to read the middle 61 sectors*/
	for (int j=0; j<=158; j++){
		for(int i=0; i<256; i++){
			//x[i]=SPI_Write(0xFF);
			PORTA = SPI_Write(0xFF);
			_delay_us(59);
		}
		//(void)x[0];
		for(int i=0; i<256; i++){
			//x[i]=SPI_Write(0xFF);
			PORTA = SPI_Write(0xFF);
			_delay_us(59);
		}
		//(void)x[0];
		for(int i=0; i<50 && SPI_Write(0xFF) != 0xFE; i++) {} // wait for data start
	}

	/*Read for the last sector of the audio file*/
	for(int i=0; i<256; i++){
		//x[i]=SPI_Write(0xFF);
		PORTA = SPI_Write(0xFF);
		_delay_us(59);
	}
	//(void)x[0];
	for(int i=0; i<50 && SPI_Write(0xFF) != 0xFE; i++) {} // wait for data start

	SPI_Write(0xFF);//for CRC
	SPI_Write(0xFF);//for CRC
	CS_DISABLE();
}

void SD_Read(unsigned long sector, unsigned short offset, unsigned char * buffer, 
             unsigned short len) {
    unsigned short i, pos = 0;
    volatile unsigned char x;
    CS_ENABLE();
    SPI_Write(0x51);
    SPI_Write(sector>>15); // sector*512 >> 24
    SPI_Write(sector>>7);  // sector*512 >> 16
    SPI_Write(sector<<1);  // sector*512 >> 8
    SPI_Write(0);          // sector*512
    SPI_Write(0xFF);
    
    for(i=0; i<50 && SPI_Write(0xFF) != 0x00; i++) {} // wait for 0
    
    for(i=0; i<50 && SPI_Write(0xFF) != 0xFE; i++) {} // wait for data start
    
    for(i=0; i<offset; i++) // "skip" bytes
        SPI_Write(0xFF);
        
    for(i=0; i<len; i++) // read len bytes
        x = SPI_Write(0xFF);
        
    for(i+=offset; i<512; i++) // "skip" again
        SPI_Write(0xFF);
        
    // skip checksum
    SPI_Write(0xFF);
    SPI_Write(0xFF);    

    CS_DISABLE();    
}

void SD_Write(unsigned long sector, unsigned char * buffer, unsigned short len) 
{
    unsigned short i, pos = 0;
    volatile unsigned char x;
    CS_ENABLE();
    SPI_Write(0x58);
    SPI_Write(sector>>15); // sector*512 >> 24
    SPI_Write(sector>>7);  // sector*512 >> 16
    SPI_Write(sector<<1);  // sector*512 >> 8
    SPI_Write(0);          // sector*512
    SPI_Write(0xFF);
    
	for(i=0; i<10 && SPI_Write(0xFF) != 0x00; i++) {} // wait for 0
    
    for(i=0; i<10 && SPI_Write(0xFF) != 0xFE; i++) {} 

    for(i=0; i<len; i++) // read len bytes
    {   
		if(len <=255){
			SPI_Write(len);
		}else{
			SPI_Write(511-len);
		}
    }   
    // skip checksum
    SPI_Write(0xFF);
    SPI_Write(0xFF);    

    CS_DISABLE();    
}

void SD_Command(unsigned char cmd, unsigned long arg,
unsigned char crc, uint16_t read) {
	uint32_t i, buffer[8];
	volatile uint8_t a,b,c,d,e,f,g,h;
	CS_ENABLE();
	SPI_Write(cmd);
	SPI_Write(arg>>24);
	SPI_Write(arg>>16);
	SPI_Write(arg>>8);
	SPI_Write(arg);
	SPI_Write(crc);
	
	for(i=0; i<read; i++){
		buffer[i] = SPI_Write(0xFF);
	}
	a = buffer[0];
	b = buffer[1];
	c = buffer[2];
	d = buffer[3];
	e = buffer[4];
	f = buffer[5];
	g = buffer[6];
	h = buffer[7];
	CS_DISABLE();
}
