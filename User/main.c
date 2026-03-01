/*------------------------------------------------------------------------------
 * File Name£º Empty_OLED
 * Description: Empty Programme with OLED and Keyboard support;
 *----------------------------------------------------------------------------*/
#include "ti_msp_dl_config.h"
#include "bsp.h"
#include "oledpicture_V0.2.h"
#include "oled_spi_V0.2.h"
#include <math.h>  // Ìí¼ÓÊýÑ§¿â

//Flash:
uint32_t EEPROMEmulationState;
uint32_t EEPROMEmulationBuffer[EEPROM_EMULATION_DATA_SIZE / sizeof(uint32_t)];

#define SECTOR_SIZE (1024)

#define NUMBER_OF_WRITES ((SECTOR_SIZE / EEPROM_EMULATION_DATA_SIZE) + 1)

uint32_t DataArray[EEPROM_EMULATION_DATA_SIZE / sizeof(uint32_t)] = {
    0xABCDEF00, 0x12345678, 0x00FEDCBA, 0x87654321, 0xABCDEF00, 0x12345678,
    0x00FEDCBA, 0x87654321, 0xABCDEF00, 0x12345678, 0x00FEDCBA, 0x87654321,
    0xABCDEF00, 0x12345678, 0x00FEDCBA, 0x87654321, 0xABCDEF00, 0x12345678,
    0x00FEDCBA, 0x87654321, 0xABCDEF00, 0x12345678, 0x00FEDCBA, 0x87654321,
    0xABCDEF00, 0x12345678, 0x00FEDCBA, 0x87654321, 0xABCDEF00, 0x12345678};

void SaveData(uint32_t*);
void LoadData();

int32_t test = 0;

int main(void)
{
    SYSCFG_DL_init();														//Initialize MCU;
    Keyboard_init();
    OLED_Init();
		delay_cycles(CPU_Frq * 1000);
    OLED_DrawBMP(9, 0, 119, 8, Genshin);    // LOGO
    delay_cycles(CPU_Frq * 1000);
    OLED_Clear();
    
		EEPROMEmulationState = EEPROM_TypeA_init(&EEPROMEmulationBuffer[0]);	//Initialize flash;
		if (EEPROMEmulationState != EEPROM_EMULATION_INIT_OK) {
        __BKPT(0);
    }
		//delay_cycles(CPU_Frq * 1000);
		//SaveData();
		//delay_cycles(CPU_Frq * 1000);
		//LoadData();	
		
    
    int key_value,temp = 0;
    while (1) {
				//LoadData();
        key_value = 0;  
        key_value = KeySCInput();
				if (key_value == 1) ++test,SaveData(&EEPROMEmulationState);
				if (key_value == 2) LoadData();
        OLED_ShowNum(0, 0, key_value, 4, 16);
				OLED_ShowNum(64, 0, test, 4, 16);
				delay_cycles(CPU_Frq * 10);
    }
}


void SaveData(uint32_t * State){
	//uint32_t data[EEPROM_EMULATION_DATA_SIZE / sizeof(uint32_t)];
	if (gEEPROMTypeAEraseFlag == 1) {
		// In this demo, when the sector is full, it will be erased immediately
		EEPROM_TypeA_eraseLastSector();
		gEEPROMTypeAEraseFlag = 0;
	}
	//EEPROM_TypeA_eraseLastSector();
	
	
	EEPROMEmulationBuffer[1] = test;
	EEPROMEmulationBuffer[2] = test;
	EEPROMEmulationBuffer[3] = test;
	EEPROMEmulationBuffer[4] = test;
	EEPROMEmulationBuffer[5] = test;
	EEPROMEmulationBuffer[6] = test;
	EEPROMEmulationBuffer[7] = test;
	
	
	//*State = EEPROM_TypeA_writeData(data);
	*State = EEPROM_TypeA_writeData(EEPROMEmulationBuffer);
	if (*State != EEPROM_EMULATION_WRITE_OK) {
		__BKPT(0);
	}
}

void LoadData(){
	EEPROM_TypeA_readData(EEPROMEmulationBuffer);
	test = EEPROMEmulationBuffer[1];
	// = EEPROMEmulationBuffer[2];
	// = EEPROMEmulationBuffer[3];
	// = EEPROMEmulationBuffer[4];.....
}