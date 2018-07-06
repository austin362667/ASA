/*
* GccApplication1.c
*
* Created: 2018/7/6 下午 1:58:30
* Author : AUSTIN
*/



#define F_CPU 11059200UL//設定頻率為11059200
#include "ASA_Lib.h"//引入標準函式庫
#include <avr/interrupt.h>

volatile uint32_t g_ticks = 0;

/*設定DIO00的ID*/
#define DIO00_ID1 (3)
#define DIO00_ID2 (4)

#define pwm_max_val (8)

/*FireWork_4*/
#define DIO_1_FW_4_0 (0)
#define DIO_1_FW_4_1 (0)
#define DIO_1_FW_4_2 (0)
#define DIO_1_FW_4_A_R (0)//第1個DIO版的A的red
#define DIO_1_FW_4_A_G (1)
#define DIO_1_FW_4_A_B (2)
#define DIO_1_FW_4_B_R (0)//第1個DIO版的B的red
#define DIO_1_FW_4_B_G (1)
#define DIO_2_FW_4_B_B (2)//第2個DIO版的B的blue
/*FireWork_1*/
#define DIO_2_FW_1_A_R (0)
#define DIO_2_FW_1_A_G (1)
#define DIO_2_FW_1_A_B (2)
/*others*/
#define DIO_2_CS_1(0)
#define DIO_2_CSL(0)
#define DIO_2_CSF(0)
#define DIO_2_R(0)

volatile uint8_t pwm_A[8] = {0, 0, 0, 0, 0, 0, 0, 0};
volatile uint8_t pwm_B[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/*sin函數的查詢表*/
const uint8_t sin_p[] = {
	0 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,
	2 ,2 ,2 ,2 ,2 ,3 ,3 ,3 ,3 ,3 ,
	3 ,3 ,3 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,
	4 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,6 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,7 ,
	7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,
	7 ,7 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,
	8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,
	8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,
	8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,
	8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,
	8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,7 ,
	7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,
	7 ,7 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	6 ,6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,
	5 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,3 ,3 ,
	3 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,
	2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
	
};


/*Variables*/
char ASA_ID = 2; //宣告模組位址變數
char LSByte = 0; //宣告資料在驅動函式暫存器表上最小住址
char check;      //宣告模組回傳錯誤代碼變數
uint32_t part = 0;//全域

/*tt為延遲多久後播放燈泡動畫(單位為tick)*/
/*tick與毫秒的換算方式為10tick = 50ms*/
/*請將單位物件包成function以便直接呼叫*/

void FireWork_4(int tt){

	int tick = 0;
	tick = part%36;
	tick *= 10;
	//10tick = 50ms  (*5)
	pwm_A_set(0,sin_p[(360+tick - tt)%360]);//DIO_1_FW_4_0
	pwm_A_set(1,sin_p[(360+tick - tt)%360]);//DIO_1_FW_4_1
	pwm_A_set(2,sin_p[(360+tick - tt)%360]);//DIO_1_FW_4_2
	pwm_A_set(3,sin_p[(360+tick - tt)%360]);//DIO_1_FW_4_A_R
	pwm_A_set(4,sin_p[(360+tick - tt)%360]);//DIO_1_FW_4_A_G
	pwm_A_set(5,sin_p[(360+tick - tt)%360]);//DIO_1_FW_4_A_B
	pwm_A_set(6,sin_p[(360+tick - tt)%360]);//DIO_1_FW_4_B_R
	pwm_A_set(7,sin_p[(360+tick - tt)%360]);//DIO_1_FW_4_B_G
	pwm_B_set(0,sin_p[(360+tick - tt)%360]);//DIO_2_FW_4_B_B

}



void FireWork_1(int tt){
	
	int tick = 0;
	tick = part%36;
	tick *= 10;
	//10tick = 50ms  (*5)
	pwm_B_set(1,sin_p[(360+tick - tt)%360]);//DIO_2_FW_1_A_R
	pwm_B_set(2,sin_p[(360+tick - tt)%360]);//DIO_2_FW_1_A_G
	pwm_B_set(3,sin_p[(360+tick - tt)%360]);//DIO_2_FW_1_A_B

}

void CS_1(int tt){
	
	int tick = 0;
	tick = part%36;
	tick *= 10;
	//10tick = 50ms  (*5)
	pwm_B_set(4,sin_p[(360+tick - tt)%360]);//DIO_2_CS_1

}

void CSL(int tt){
	
	int tick = 0;
	tick = part%36;
	tick *= 10;
	//10tick = 50ms  (*5)
	pwm_B_set(5,sin_p[(360+tick - tt)%360]);//DIO_2_CSL

}

void CSF(int tt){
	
	int tick = 0;
	tick = part%36;
	tick *= 10;
	//10tick = 50ms  (*5)
	pwm_B_set(6,sin_p[(360+tick - tt)%360]);//DIO_2_CSF

}

void R(int tt){
	
	int tick = 0;
	tick = part%36;
	tick *= 10;
	//10tick = 50ms  (*5)
	pwm_B_set(7,sin_p[(360+tick - tt)%360]);//DIO_2_R

}


/*從8種亮度(0~7)中選擇一個閥值*/
void pwm_A_set(uint8_t bit, uint8_t selection) {
	
	pwm_A[bit] = selection;
}

void pwm_B_set(uint8_t bit, uint8_t selection) {
	
	pwm_B[bit] = selection;
}

/*製造亮暗程度並傳至DIO*/
inline void pwm_A_task()
{
	uint8_t PWM_counter = g_ticks % pwm_max_val;
	uint8_t output = 0;
	
	for (int i = 0; i < 8; i++)
	{
		if (PWM_counter < pwm_A[i])
		{
			output |= 1 << i;
		}
	}
	
	ASA_DIO00_fpt(DIO00_ID1, LSByte, 0xff, 0, output);
	return;
}

inline void pwm_B_task()
{
	uint8_t PWM_counter = g_ticks % pwm_max_val;
	uint8_t output = 0;
	
	for (int i = 0; i < 8; i++)
	{
		if (PWM_counter < pwm_B[i])
		{
			output |= 1 << i;
		}
	}
	
	ASA_DIO00_fpt(DIO00_ID2, LSByte, 0xff, 0, output);
	return;
}

/*tick*/
ISR(TIMER0_COMP_vect)
{
	g_ticks++;
	
	pwm_A_task();
	pwm_B_task();
}

/*初始化計時器*/
void init_timer()
{
	TCCR0 = 0b00001100;
	OCR0 = 173;
	TIMSK |= 0x01 << OCIE0;
}

/*主程式*/
void main(void)
{
	
	ASA_M128_set();
	
	init_timer();
	
	sei();
	while (1)
	{
		runState(0);// 0 1 2 3 ...
	}
	
	return ;
}
/*解碼器編寫中*/
/* ... */
/*選擇 輸入的state(共16種)*/
void runState(int state){

	int tick = 0;
	switch (state){
		case 0:
		
		case 1:

		case 2:

		case 3:

		case 4:

		case 5:

		case 6:

		case 7:

		case 8:

		case 9:

		case 10:

		case 11:

		case 12:

		case 13:

		case 14:

		case 15:

	}
	/*注意delay的位置與part計量*/
	_delay_ms(50)
	part++;

}