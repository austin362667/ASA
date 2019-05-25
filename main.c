/*
* GccApplication1.c
*
* Created: 2018/7/6 下午 1:58:30
* Author : AUSTIN
*/

int command = 0;
#define idle_state (0)
#define end_state (1)
#define running_state (2)

volatile int tick = 0;

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
#define DIO_1_FW_4_1 (1)
#define DIO_1_FW_4_2 (2)
#define DIO_1_FW_4_A_R (3)//第1個DIO版的A的red
#define DIO_1_FW_4_A_G (4)
#define DIO_1_FW_4_A_B (5)
#define DIO_1_FW_4_B_R (6)//第1個DIO版的B的red
#define DIO_1_FW_4_B_G (7)
#define DIO_2_FW_4_B_B (0)//第2個DIO版的B的blue
/*FireWork_1*/
#define DIO_2_FW_1_A_R (4)
#define DIO_2_FW_1_A_G (5)
#define DIO_2_FW_1_A_B (6)
/*others*/
#define DIO_2_CS_1 (1)
#define DIO_2_CSL (3)
#define DIO_2_CSF (2)
#define DIO_2_R (7)

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
char ASA_ID = 6; //宣告模組位址變數
char LSByte = 0; //宣告資料在驅動函式暫存器表上最小住址
char check;      //宣告模組回傳錯誤代碼變數
volatile uint32_t part = 0;//全域

/*tt為延遲多久後播放燈泡動畫(單位為tick)*/
/*tick與毫秒的換算方式為10tick = 50ms*/
/*請將單位物件包成function以便直接呼叫*/

void FireWork_4(int tt){

	uint32_t temp_tick = (part*5)%360;
	
	pwm_A_set(DIO_1_FW_4_0,sin_p[(360+temp_tick - tt)%360]);//DIO_1_FW_4_0
	pwm_A_set(DIO_1_FW_4_1,sin_p[(360+temp_tick - tt)%360]);//DIO_1_FW_4_1
	pwm_A_set(DIO_1_FW_4_2,sin_p[(360+temp_tick - tt)%360]);//DIO_1_FW_4_2
	pwm_A_set(DIO_1_FW_4_A_R,sin_p[(360+temp_tick - tt)%360]);//DIO_1_FW_4_A_R
	pwm_A_set(DIO_1_FW_4_A_G,sin_p[(360+temp_tick - tt)%360]);//DIO_1_FW_4_A_G
	pwm_A_set(DIO_1_FW_4_A_B,sin_p[(360+temp_tick - tt)%360]);//DIO_1_FW_4_A_B
	pwm_A_set(DIO_1_FW_4_B_R,sin_p[(360+temp_tick - tt)%360]);//DIO_1_FW_4_B_R
	pwm_A_set(DIO_1_FW_4_B_G,sin_p[(360+temp_tick - tt)%360]);//DIO_1_FW_4_B_G
	pwm_B_set(DIO_2_FW_4_B_B,sin_p[(360+temp_tick - tt)%360]);//DIO_2_FW_4_B_B
	printf("%d\n", sin_p[(360+temp_tick - tt)%360]);

}



void FireWork_1(int tt){
	
	uint32_t temp_tick = (part*5)%360;

	pwm_B_set(DIO_2_FW_1_A_R,sin_p[(360+temp_tick - tt)%360]);//DIO_2_FW_1_A_R
	pwm_B_set(DIO_2_FW_1_A_G,sin_p[(360+temp_tick - tt)%360]);//DIO_2_FW_1_A_G
	pwm_B_set(DIO_2_FW_1_A_B,sin_p[(360+temp_tick - tt)%360]);//DIO_2_FW_1_A_B
	printf("%d\n", sin_p[(360+temp_tick - tt)%360]);

}

void CS_1(int tt){
	
	uint32_t temp_tick = (part*5)%360;

	pwm_B_set(DIO_2_CS_1,sin_p[(360+temp_tick - tt)%360]);//DIO_2_CS_1
	printf("pwm: %d\t", sin_p[(360UL+temp_tick - tt)%360]);
	printf("index: %d\t", (360UL+temp_tick - tt)%360);
	printf("tick: %d\n", temp_tick);

}

void CSL(int tt){
	
	uint32_t temp_tick = (part*5)%360;

	pwm_B_set(DIO_2_CSL,sin_p[(360+temp_tick - tt)%360]);//DIO_2_CSL
	printf("%d\n", sin_p[(360+temp_tick - tt)%360]);

}

void CSF(int tt){
	
	uint32_t temp_tick = (part*5)%360;

	pwm_B_set(DIO_2_CSF,sin_p[(360+temp_tick - tt)%360]);//DIO_2_CSF
	printf("%d\n", sin_p[(360+temp_tick - tt)%360]);

}

void R(int tt){
	
	uint32_t temp_tick = (part*5)%360;

	pwm_B_set(DIO_2_R,sin_p[(360+temp_tick - tt)%360]);//DIO_2_R
	printf("%d\n", sin_p[(360+temp_tick - tt)%360]);

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
	cli();
	ASA_DIO00_fpt(DIO00_ID1, LSByte, 0xff, 0, output);
	sei();
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
	cli();
	ASA_DIO00_fpt(DIO00_ID2, LSByte, 0xff, 0, output);
	sei();
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

uint8_t  get_command(){

	uint8_t data_in = PINC;
	if((data_in >> 2) & 0b00000001)
	{

		return 0;

	}else{

		return ((data_in >> 3) & 0b00001111);

	}

}


/*主程式*/
void main(void)
{
	
	ASA_M128_set();
	
	DDRC &= 0b00000011;
	PORTC = 0b11111100;
	init_timer();
	sei();
	command = 0;
	while (1)
	{	
		
		//printf("pinc:%d\n", PINC);

		/*for(int i=0; i<8 ;i++) {
			if(PINC & 1<<i) printf("1");
			else printf("0");
		
		printf("\n");*/

		//printf("DDRC:%d\n", DDRC);

			if (command  == 0)
			{
				command = get_command();

						if(command!=0){
							
							printf("cmmmand:%d\n", command);
						}
			}
		
			int checking = runState(command);

			if (checking == running_state)
			{
			} 

			if (checking == end_state)
			{
				command = 0;
			}

			if (checking == idle_state)
			{
			}	
		
	}
	
	return ;
}

/*選擇 輸入的state=command(共16種)*/
int runState(int state){

	part++;
	/*注意delay的位置與part計量*/
	//_delay_ms(20);

	static uint32_t part_tick = 0;

	static uint8_t last_state = 0;
	
	////////////////       Delay 50 ms here     //////////////////////
	static uint32_t timing_tick = 0;
	while( (g_ticks - timing_tick) < 50);
	timing_tick = g_ticks;
	/////////////////////////////////////////////////////////////////

	// If state change at this moment
	if(state != last_state) {
		part_tick = 0;
		printf("State change !");
	}
	last_state = state;

	// How many times since new state loop entered;
	part_tick++;

	switch (state){
		case 0:
			part_tick = 0;
			part = 0;
			return idle_state;
			break;
		case 1:
			CSF(0);//DIO_2_CSF
			if (part_tick == 36){
				pwm_B_set(DIO_2_CSF,0);
			}
			CS_1(36);
			pwm_A_set(0,sin_p[(360+part_tick - 44)%360]);
			pwm_A_set(1,sin_p[(360+part_tick - 44)%360]);
			pwm_A_set(2,sin_p[(360+part_tick - 44)%360]);
			printf("!\n");
			if (part_tick == 72){
				return end_state;
			}else{
				return running_state;
			}
			break;
		case 2:
			CSL(0);
			CSF(18);
						if (part_tick == 54){
							pwm_B_set(DIO_2_CSF,0);
						}
			CS_1(36);
			if (part_tick == 72){
				return end_state;
			}else{
				return running_state;
				}
			break;
		case 3:
			FireWork_1(36);
			FireWork_4(36);
			CS_1(0);
			printf("temp_tick: %d\n", part_tick);
			if (part_tick == 72){
				return end_state;
			}else{
				return running_state;
				}
			break;
		case 4:
			pwm_B_set(DIO_2_CSL,8);//DIO_2_CSL
									if (part_tick == 20){
											pwm_B_set(DIO_2_CS_1,8);//DIO_2_CS_1
									}
			
												if (part_tick == 60){
													pwm_B_set(DIO_2_CSL,0);//DIO_2_CSL
													pwm_B_set(DIO_2_CS_1,0);//DIO_2_CS_1
												}
	
			
			
			
			FireWork_1(72);
			FireWork_4(72);
						if (part_tick == 144){
							return end_state;
							}else{
							return running_state;
						}
						break;
			break;
		case 5:
			pwm_B_set(DIO_2_R,8);//DIO_2_R
			
			CS_1(72);
						if (part_tick == 144){
							return end_state;
							}else{
							return running_state;
						}
						break;
		case 6:
			CSL(36);
			CS_1(36);
						if (part_tick == 144){
							return end_state;
							}else{
							return running_state;
						}
						break;
		case 7:
			R(0);
			CSL(36);
			CSF(36);
			CS_1(72);
			FireWork_1(108);
			FireWork_4(144);
						if (part_tick == 216){
							return end_state;
							}else{
							return running_state;
						}
						break;
			break;
		case 8:
			CSL(0);
			CSF(0);
			CS_1(0);
			FireWork_1(50);
			FireWork_4(100);
			R(0);
						if (part_tick == 140){
							return end_state;
							}else{
							return running_state;
						}
						break;
			break;
		case 9:
			CSL(30);
			CSF(10);
			CS_1(50);
			FireWork_1(50);
			FireWork_4(20);
			R(20);
						if (part_tick == 140){
							return end_state;
							}else{
							return running_state;
						}
						break;
			break;
		case 10:
			R(50);
			CSL(40);
			CSF(70);
			CS_1(60);
			FireWork_1(40);
			FireWork_4(80);
						if (part_tick == 140){
							return end_state;
							}else{
							return running_state;
						}
						break;
			break;
		case 11:	
			R(0);
			CSL(0);
			CSF(0);
			CS_1(0);
			FireWork_1(0);
			FireWork_4(0);
						if (part_tick == 140){
							return end_state;
							}else{
							return running_state;
						}
						break;
			break;
		case 12:
			R(0);
			CSL(40);
			CSF(0);
			CS_1(40);
			FireWork_1(0);
			FireWork_4(40);
						if (part_tick == 140){
							return end_state;
							}else{
							return running_state;
						}
						break;
			break;
		case 13:
			R(40);
			CSL(0);
			CSF(40);
			CS_1(0);
			FireWork_1(40);
			FireWork_4(0);
						if (part_tick == 140){
							return end_state;
							}else{
							return running_state;
						}
						break;
			break;
		case 14:
			pwm_B_set(0,0);
			pwm_B_set(1,0);
			pwm_B_set(2,0);
			pwm_B_set(3,0);
			pwm_B_set(4,0);
			pwm_B_set(5,0);
			pwm_B_set(6,0);
			pwm_B_set(7,0);
			pwm_A_set(0,0);
			pwm_A_set(1,0);
			pwm_A_set(2,0);
			pwm_A_set(3,0);
			pwm_A_set(4,0);
			pwm_A_set(5,0);
			pwm_A_set(6,0);
			pwm_A_set(7,0);
			printf("state: 14\n");
			return end_state;

			break;
		case 15:
			pwm_B_set(0,8);
			pwm_B_set(1,8);
			pwm_B_set(2,8);
			pwm_B_set(3,8);
			pwm_B_set(4,8);
			pwm_B_set(5,8);
			pwm_B_set(6,8);
			pwm_B_set(7,8);
			pwm_A_set(0,8);
			pwm_A_set(1,8);
			pwm_A_set(2,8);
			pwm_A_set(3,8);
			pwm_A_set(4,8);
			pwm_A_set(5,8);
			pwm_A_set(6,8);
			pwm_A_set(7,8);
			printf("state: 15\n");
			return end_state;

			break;

	}





}
