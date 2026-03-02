/*------------------------------------------------------------------------------
 * 项目名称: Signal_Station (Pocket Lab)
 * 功能描述: 集成电压测量、信号发生器与数据记录的多功能工作站
 * 当前进度: Day 1 - 完善系统框架与菜单导航
 *----------------------------------------------------------------------------*/
#include "ti_msp_dl_config.h"
#include "bsp.h"
#include "oledpicture_V0.2.h"
#include "oled_spi_V0.2.h"
#include <math.h>

// ==========================================
// 1. 系统模式与全局变量定义
// ==========================================
typedef enum {
    MODE_MENU = 0,
    MODE_VOLTMETER,
    MODE_GENERATOR,
    MODE_HISTORY
} SystemMode;

// 全局状态变量
SystemMode g_currentMode = MODE_MENU;

static int key_value = 0;
static bool key_handled = false;		
const char keyboards[] = {'1', '2', '3', 'A',
													'4', '5', '6', 'B',
													'7', '8', '9', 'C',
													'*', '0', '#', 'D'};

bool       g_needsUpdate = true;

// 业务数据（需在各模式函数中更新）
float    g_voltValue = 0.0f;
uint32_t g_frequency = 100;

// ==========================================
// 2. 函数声明
// ==========================================

// 逻辑处理
void Task_SystemLogic(void);
void Task_Measurement(void);

// UI 显示函数
void UI_DrawMenu(void);
void UI_DrawVoltmeter(void);
void UI_DrawGenerator(void);
void UI_DrawHistory(void);

// --- Flash 存储相关 ---
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

// ==========================================
// 3. 主函数
// ==========================================
int main(void)
{
    SYSCFG_DL_init();
    Keyboard_init();
    OLED_Init();
    
	// 初始化 Flash 模拟 EEPROM
	EEPROMEmulationState = EEPROM_TypeA_init(&EEPROMEmulationBuffer[0]);	//Initialize flash;
	if (EEPROMEmulationState != EEPROM_EMULATION_INIT_OK) {
        __BKPT(0); // 如果 Flash 初始化失败，进入断点
    }
		//delay_cycles(CPU_Frq * 1000);
		//SaveData();
		//delay_cycles(CPU_Frq * 1000);
		//LoadData();	
		
    
    while (1) {
		// --- 1. 输入层 ---

        // --- 2. 逻辑层 ---


        // --- 3. 测量/任务层（后台运行） ---
        Task_Measurement();

        // --- 4. 显示层 ---
        if (g_needsUpdate) {
            OLED_Clear();
            switch (g_currentMode) {
                case MODE_MENU:      UI_DrawMenu();      break;
                case MODE_VOLTMETER: UI_DrawVoltmeter(); break;
                case MODE_GENERATOR: UI_DrawGenerator(); break;
                case MODE_HISTORY:   UI_DrawHistory();   break;
            }
            g_needsUpdate = false;
        }
    }
}

// ==========================================
// 4. 逻辑处理实现 (待填写)
// ==========================================

//10ms定时器（按键输入）
void TIMER_0_INST_IRQHandler(void){
		static int last_key = 0;
		int raw_key = KeySCInput();
		if(raw_key == last_key && raw_key != 0 && raw_key != key_value){
				key_value = raw_key;
				key_handled = true;
		}	else if(raw_key == 0 && !key_handled){
				key_value = 0;
		}
		last_key = raw_key;
		
		if(key_value==0) key_handled = false;
		
}

void Task_SystemLogic(void) {
    /* 
     * TODO: 根据 g_keyValue 实现：
     * 1. 模式切换：例如 A 键返回菜单，1, 2, 3 进入不同模式
     * 2. 参数修改：在信号源模式下增加/减少频率
     * 3. 数据保存：在电压表模式下触发存储
     * 4. 记得操作完成后设置 g_needsUpdate = true
     */
}

void Task_Measurement(void) {
    /*
     * TODO: 后台处理非 UI 相关的任务：
     * 1. 如果在电压表模式，读取 ADC 数值并转换为电压
     * 2. 如果在信号源模式，根据 g_frequency 更新定时器参数
     */
}

// ==========================================
// 5. UI 绘制实现 (待填写)
// ==========================================

void UI_DrawMenu(void) {
    // TODO: 使用 OLED_ShowString 绘制主菜单列表
}

void UI_DrawVoltmeter(void) {
    // TODO: 绘制电压表界面，显示 g_voltValue 的实时数值
}

void UI_DrawGenerator(void) {
    // TODO: 绘制信号源界面，显示当前频率和波形类型
}

void UI_DrawHistory(void) {
    // TODO: 从缓存数组或 Flash 中读取记录并显示在屏幕上
}

// ==========================================
// 6. 存储功能实现
// ==========================================

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