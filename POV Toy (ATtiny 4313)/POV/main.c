/*
 * POV.c
 *
 * Created: 4/10/2016 1:44:26 PM
 * Author : Allen
 */ 
#define  F_CPU 1000000UL  //8MHZ
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <util/>
#define dotdelay  _delay_ms(2)
#define chardelay  _delay_ms(10)
#define POVClear PORTB = 0b00000000;

int size = 0;
uint8_t A[8] = {0b00000111, 0b00011100, 0b01110000, 0b10010000, 0b10010000, 0b01110000, 0b00011100, 0b00000111};
uint8_t B[8] = {0b11111111, 0b11111111, 0b10011001, 0b10011001, 0b10011001, 0b10011001, 0b11111111, 0b01100110};
uint8_t C[8] = {0b00111100, 0b01111110, 0b11000011, 0b11000011, 0b11000011, 0b11000011, 0b01100110, 0b01100110};
uint8_t D[8] = {0b11111111, 0b11111111, 0b11000011, 0b11000011, 0b11000011, 0b01100110, 0b01111110, 0b00011000};
uint8_t E[8] = {0b11111111, 0b10011001, 0b10011001, 0b10011001, 0b10011001, 0b10011001, 0b10011001, 0b10000001};
uint8_t F[8] = {0b11111111, 0b10011000, 0b10011000, 0b10011000, 0b10011000, 0b10011000, 0b10011000, 0b10000000};
uint8_t G[8] = {0b00111100, 0b01111110, 0b11000011, 0b11000011, 0b11000011, 0b11001011, 0b01101111, 0b00101111};
uint8_t H[8] = {0b11111111, 0b11111111, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b11111111, 0b11111111};
uint8_t I[8] = {0b11000011, 0b11000011, 0b11000011, 0b11111111, 0b11111111, 0b11000011, 0b11000011, 0b11000011};
uint8_t J[8] = {0b11000110, 0b11000111, 0b11000011, 0b11000011, 0b11111111, 0b11111110, 0b11000000, 0b11000000};
uint8_t K[8] = {0b11111111, 0b11111111, 0b00011000, 0b00111100, 0b01100110, 0b11000011, 0b10000001, 0b10000001};
uint8_t L[8] = {0b11111111, 0b11111111, 0b00000011, 0b00000011, 0b00000011, 0b00000011, 0b00000011, 0b00000011};
uint8_t M[8] = {0b00011111, 0b11111000, 0b11111000, 0b00011111, 0b00011111, 0b11111000, 0b11111000, 0b00011111};
uint8_t N[8] = {0b11111111, 0b11111111, 0b00110000, 0b00011000, 0b00001100, 0b00000110, 0b11111111, 0b11111111};
uint8_t O[8] = {0b00111100, 0b01111110, 0b11000011, 0b11000011, 0b11000011, 0b11000011, 0b01111110, 0b00111100};
uint8_t P[8] = {0b11111111, 0b11111111, 0b10001000, 0b10001000, 0b10001000, 0b10001000, 0b11111000, 0b01110000};
uint8_t Q[8] = {0b00111100, 0b01111110, 0b11000011, 0b11000011, 0b11000011, 0b11000111, 0b01111110, 0b00111111};
uint8_t R[8] = {0b11111111, 0b11111111, 0b11001000, 0b11001000, 0b11001100, 0b11001110, 0b01111011, 0b00110001};
uint8_t S[8] = {0b00100100, 0b01110110, 0b11010011, 0b10011001, 0b10011001, 0b11001011, 0b01101110, 0b00100100};
uint8_t T[8] = {0b11000000, 0b11000000, 0b11000000, 0b11111111, 0b11111111, 0b11000000, 0b11000000, 0b11000000};
uint8_t U[8] = {0b11111100, 0b11111110, 0b00000011, 0b00000011, 0b00000011, 0b00000011, 0b11111110, 0b11111100};
uint8_t V[8] = {0b11100000, 0b00111100, 0b00001110, 0b00000011, 0b00000011, 0b00001110, 0b00111100, 0b11100000};
uint8_t W[8] = {0b11111000, 0b00001111, 0b00001111, 0b11111000, 0b11111000, 0b00001111, 0b00001111, 0b11111000};
uint8_t X[8] = {0b10000001, 0b11000011, 0b01100110, 0b00111100, 0b00111100, 0b01100110, 0b11000011, 0b10000001};
uint8_t Y[8] = {0b10000000, 0b11000000, 0b01100000, 0b00111100, 0b00111100, 0b01100000, 0b11000000, 0b10000000};
uint8_t Z[8] = {0b11000001, 0b11000011, 0b11010111, 0b11011111, 0b11111011, 0b11101011, 0b11000011, 0b10000011};

uint8_t Heart[9] = {0b01110000, 0b10001000, 0b10000100, 0b01000010, 0b00100001, 0b01000010, 0b10000100, 0b10001000, 0b01110000};
uint8_t Brick[8]={0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111};
void DisplayChar(char x);
void POVDisplay(uint8_t array[], int size);
void DisplayString(char* s);
void POVDisplay(uint8_t array[], int size){
	for (int i=0; i< size; i++){
		PORTB = *(array+i);
		dotdelay;
	}
}
void DisplayChar(char x)
{
	if(x == 'A'){
		size = sizeof(A);
		POVDisplay(A, size);
	}else if (x == 'B'){
		size = sizeof(B);
		POVDisplay(B, size);
	}else if (x == 'C'){
		size = sizeof(C);
		POVDisplay(C, size);
	}else if (x == 'D'){
		size = sizeof(D);
		POVDisplay(D, size);
	}else if (x == 'E'){
		size = sizeof(E);
		POVDisplay(E, size);
	}else if (x == 'F'){
		size = sizeof(F);
		POVDisplay(F, size);
	}else if (x == 'G'){
		size = sizeof(G);
		POVDisplay(G, size);
	}else if (x == 'H'){
		size = sizeof(H);
		POVDisplay(H, size);
	}else if (x == 'I'){
		size = sizeof(I);
		POVDisplay(I, size);
	}else if (x == 'J'){
		size = sizeof(J);
		POVDisplay(J, size);
	}else if (x == 'K'){
		size = sizeof(K);
		POVDisplay(K, size);
	}else if (x == 'L'){
		size = sizeof(L);
		POVDisplay(L, size);
	}else if (x == 'M'){
		size = sizeof(M);
		POVDisplay(M, size);
	}else if (x == 'N'){
		size = sizeof(N);
		POVDisplay(N, size);
	}else if (x == 'O'){
		size = sizeof(O);
		POVDisplay(O, size);
	}else if (x == 'P'){
		size = sizeof(P);
		POVDisplay(P, size);
	}else if (x == 'Q'){
		size = sizeof(Q);
		POVDisplay(Q, size);
	}else if (x == 'R'){
		size = sizeof(R);
		POVDisplay(R, size);
	}else if (x == 'S'){
		size = sizeof(S);
		POVDisplay(S, size);
	}else if (x == 'T'){
		size = sizeof(T);
		POVDisplay(T, size);
	}else if (x == 'U'){
		size = sizeof(U);
		POVDisplay(U, size);
	}else if (x == 'V'){
		size = sizeof(V);
		POVDisplay(V, size);
	}else if (x == 'W'){
		size = sizeof(W);
		POVDisplay(W, size);
	}else if (x == 'X'){
		size = sizeof(X);
		POVDisplay(X, size);
	}else if (x == 'Y'){
		size = sizeof(Y);
		POVDisplay(Y, size);
	}else if (x == 'Z'){
		size = sizeof(Z);
		POVDisplay(Z, size);
	}else if (x == '@'){
		size = sizeof(Heart);
		POVDisplay(Heart, size);
	}else{
		size = sizeof(Brick);
		POVDisplay(Brick, size);
	}
	POVClear;
	chardelay;
}
void DisplayString(char* s){
	for (int i =0; i <= strlen(s); i++ )
	{
		DisplayChar(s[i]);
	}
}
int main(void)
{

	DDRB |= 0xFF;
    while (1) 
    {
		DisplayString("ALLEN");
		DisplayChar('@');
		DisplayString("DORIS");
    }
	return 0;
}

