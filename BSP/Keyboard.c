#include "Keyboard.h"
//const bool InterruptON = 1;	//Switch of the Interrupt Function;
void Keyboard_init(){
	DL_GPIO_setPins(Keyboard_PORT,Keyboard_KE_1_PIN);
	DL_GPIO_setPins(Keyboard_PORT,Keyboard_KE_2_PIN);
	DL_GPIO_setPins(Keyboard_PORT,Keyboard_KE_3_PIN);
	DL_GPIO_setPins(Keyboard_PORT,Keyboard_KE_4_PIN);
}

inline bool readButton(size_t B){
	if(DL_GPIO_readPins(Keyboard_PORT,B) == 0){
		if (delay_level != 0) DL_Common_delayCycles(CPU_Frq * delay_level);
		if(DL_GPIO_readPins(Keyboard_PORT,B) == 0){
			return 0;
		}
	}
	return 1;
}
uint16_t KeySCInput(){
	//delay_level Unit: ms;
	int x,y,cvt;
	bool flag;
	flag = 1;cvt = x = y = 0;
	DL_GPIO_clearPins(Keyboard_PORT,Keyboard_KE_1_PIN);
	//Pins改变电压需要时间
	DL_Common_delayCycles(3200);
	//Delay确保电平已经改变
	x = 1;
	if(readButton(Keyboard_KR_1_PIN) == 0) y = 1,flag = 0;
	//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_1_PIN) == 0) y = 1,flag = 0;
	if(readButton(Keyboard_KR_2_PIN) == 0) y = 2,flag = 0;
	//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_2_PIN) == 0) y = 2,flag = 0;
	if(readButton(Keyboard_KR_3_PIN) == 0) y = 3,flag = 0;
	//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_3_PIN) == 0) y = 3,flag = 0;
	if(readButton(Keyboard_KR_4_PIN) == 0) y = 4,flag = 0;
	//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_4_PIN) == 0) y = 4,flag = 0;
	DL_GPIO_setPins(Keyboard_PORT,Keyboard_KE_1_PIN);
	if (flag){
		DL_GPIO_clearPins(Keyboard_PORT,Keyboard_KE_2_PIN);
		DL_Common_delayCycles(3200);
		x = 2;
		if(readButton(Keyboard_KR_1_PIN) == 0) y = 1,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_1_PIN) == 0) y = 1,flag = 0;
		if(readButton(Keyboard_KR_2_PIN) == 0) y = 2,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_2_PIN) == 0) y = 2,flag = 0;
		if(readButton(Keyboard_KR_3_PIN) == 0) y = 3,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_3_PIN) == 0) y = 3,flag = 0;
		if(readButton(Keyboard_KR_4_PIN) == 0) y = 4,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_4_PIN) == 0) y = 4,flag = 0;
		DL_GPIO_setPins(Keyboard_PORT,Keyboard_KE_2_PIN);
	}
	if (flag){
		DL_GPIO_clearPins(Keyboard_PORT,Keyboard_KE_3_PIN);
		DL_Common_delayCycles(3200);
		x = 3;
		if(readButton(Keyboard_KR_1_PIN) == 0) y = 1,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_1_PIN) == 0) y = 1,flag = 0;
		if(readButton(Keyboard_KR_2_PIN) == 0) y = 2,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_2_PIN) == 0) y = 2,flag = 0;
		if(readButton(Keyboard_KR_3_PIN) == 0) y = 3,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_3_PIN) == 0) y = 3,flag = 0;
		if(readButton(Keyboard_KR_4_PIN) == 0) y = 4,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_4_PIN) == 0) y = 4,flag = 0;
		DL_GPIO_setPins(Keyboard_PORT,Keyboard_KE_3_PIN);
	}
	if (flag){
		DL_GPIO_clearPins(Keyboard_PORT,Keyboard_KE_4_PIN);
		DL_Common_delayCycles(3200);
		x = 4;
		if(readButton(Keyboard_KR_1_PIN) == 0) y = 1,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_1_PIN) == 0) y = 1,flag = 0;
		if(readButton(Keyboard_KR_2_PIN) == 0) y = 2,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_2_PIN) == 0) y = 2,flag = 0;
		if(readButton(Keyboard_KR_3_PIN) == 0) y = 3,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_3_PIN) == 0) y = 3,flag = 0;
		if(readButton(Keyboard_KR_4_PIN) == 0) y = 4,flag = 0;
		//if(DL_GPIO_readPins(Keyboard_PORT,Keyboard_KR_4_PIN) == 0) y = 4,flag = 0;
		DL_GPIO_setPins(Keyboard_PORT,Keyboard_KE_4_PIN);
	}
	if(flag == 0) cvt = (x - 1) * 4 + y;
	return cvt;
}
