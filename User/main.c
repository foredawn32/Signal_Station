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
	MODE_GREETING = 0, // 欢迎界面
    MODE_MENU,
    MODE_VOLTMETER,
    MODE_GENERATOR,
    MODE_HISTORY
} SystemMode;

// 全局状态变量
SystemMode g_currentMode = MODE_GREETING;

static int g_keyvalue = 0;
static bool g_keyhandled = true;		
const char g_keyboards[] = {'1', '2', '3', 'A',
													'4', '5', '6', 'B',
													'7', '8', '9', 'C',
													'*', '0', '#', 'D'};

bool       g_needsUpdate = true;

/* 信号发生器参数 */
static int g_sampleRate = 20000; // 信号发生器采样率
int32_t g_currentPhase; // 相位累加器，对应 [-1, 1) 的 Unit Angle
int32_t g_phaseStep;  // 每个采样周期的相位增量，计算公式：phaseStep = (frequency / sampleRate) * 2^32
uint32_t g_targetFreq = 100; // 目标频率，单位 Hz
bool g_waveEnabled = false; // 波形输出使能

// 业务数据（需在各模式函数中更新）
float    g_voltValue = 0.0f;
uint32_t g_frequency = 100;

// ==========================================
// 2. 函数声明
// ==========================================

// 逻辑处理
void Task_SystemLogic(void);
void Task_Measurement(void);

// 信号发生器相关
void App_Generator_Init(void);
void Generator_UpdateStep(void);
void Generator_SetFrequency(uint32_t freq);

// UI 显示函数
void UI_DrawGreeting(void);
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
	App_Generator_Init(); // 初始化信号发生器相关设置
	/* 使能中断 */
	NVIC_EnableIRQ(key_INT_IRQN);
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    /*开始计时*/
	DL_TimerG_startCounter(TIMER_0_INST); 
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
		Task_SystemLogic();

        // --- 3. 测量/任务层（后台运行） ---
        Task_Measurement();

        // --- 4. 显示层 ---
        if (g_needsUpdate) {
            OLED_Clear();
            g_needsUpdate = false;
        }
		switch (g_currentMode) {
            case MODE_GREETING:  UI_DrawGreeting();  break;
            case MODE_MENU:      UI_DrawMenu();      break;
            case MODE_VOLTMETER: UI_DrawVoltmeter(); break;
            case MODE_GENERATOR: UI_DrawGenerator(); break;
            case MODE_HISTORY:   UI_DrawHistory();   break;
			default: break;}
		}
}

// ==========================================
// 4. 逻辑处理实现 (待填写)
// ==========================================

//10ms定时器（按键输入）
void TIMER_0_INST_IRQHandler(void){
		static int last_key = 0;
		int raw_key = KeySCInput();
		if(raw_key == last_key && raw_key != 0 && raw_key != g_keyvalue){
				g_keyvalue = raw_key;
				g_keyhandled = false;
		}	else if(raw_key == 0 && !g_keyhandled){
				g_keyvalue = 0;
		}
		last_key = raw_key;
		
		if(g_keyvalue==0) g_keyhandled = true;
		
}

//0.05ms定时器（信号发生器更新）
void TIMER_1_INST_IRQHandler(void){
	/*
	 *TODO - 1. 根据 g_waveEnabled 判断是否需要更新输出
	*/
		if (g_waveEnabled) {
            // 2. 相位累加：每次中断增加一个步长
            // 这里的溢出是“安全的”，例如从 2,147,483,647 加 1 
            // 会变成 -2,147,483,648，正好对应 MATHACL 中 Unit Angle 的循环
            g_currentPhase += g_phaseStep;

            // 3. 触发 MATHACL 计算
            // 直接写入 OP2 寄存器触发计算（假设已经在初始化里配置好了 FUNC 为 SINCOS）
            MATHACL->OP2 = g_currentPhase;

            // 4. 等待硬件计算完成 (通常仅需几个时钟周期)
            // 80MHz 主频下，MATHACL 计算 SINCOS 的速度极快
            while (MATHACL->STATUS & MATHACL_STATUS_BUSY_MASK);

            // 5. 获取正弦结果并映射到 DAC
            // RES2 返回的是 SQ0.31 格式 (-2^31 到 2^31-1)
            int32_t rawSine = (int32_t)MATHACL->RES2;

            // 映射逻辑：
            // rawSine >> 20 : 将 32位数据 缩减到 12位 (-2048 到 2047)
            // + 2048       : 增加直流偏移，使其变为 (0 到 4095)
            uint32_t dacValue = (uint32_t)((rawSine >> 20) + 2048);

            // 6. 立即输出到 DAC
            DL_DAC12_output12(DAC0, dacValue);
            
    	} else {
            // 如果关闭输出，使 DAC 保持在中位（1.65V）
    	    DL_DAC12_output12(DAC0, 2048);
    	}
}

/*菜单*/
//user按键中断打开菜单
void GROUP1_IRQHandler(void){
		switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)){
			case key_INT_IIDX:
					DL_TimerG_stopCounter(TIMER_1_INST); // 进入菜单时停止信号发生器
					g_waveEnabled = false; // 禁止波形输出
					g_needsUpdate = true; 
					g_currentMode = MODE_MENU; break;
			default: break;
		}
}

void Task_SystemLogic(void) {
    /* 
     * TODO: 根据 g_keyValue 实现：
     * 1. 模式切换：在菜单界面时，A, B, C 进入不同模式
     * 2. 参数修改：在信号源模式下增加/减少频率
     * 3. 数据保存：在电压表模式下触发存储
     * 4. 记得操作完成后设置 g_needsUpdate = true
     */
	if(g_currentMode == MODE_MENU && !g_keyhandled && g_keyvalue >0){
		switch(g_keyboards[g_keyvalue - 1]){
			case 'A': g_currentMode = MODE_VOLTMETER; break;
			case 'B': 
				g_currentMode = MODE_GENERATOR;
				g_waveEnabled = true; // 进入信号源模式时启用输出
				DL_TimerG_startCounter(TIMER_1_INST); // 启动信号发生器定时器
				break;
			case 'C': g_currentMode = MODE_HISTORY; break;
			default: break;
		}
		g_keyhandled = true;
		g_needsUpdate = true;
	}
}

void Task_Measurement(void) {
    /*
     * TODO: 后台处理非 UI 相关的任务：
     * 1. 如果在电压表模式，读取 ADC 数值并转换为电压
     * 2. 如果在信号源模式，根据 g_frequency 更新定时器参数
     */
	if(g_currentMode == MODE_GENERATOR){
		// TODO: 更新信号源参数

	}
}

void Generator_UpdateStep(void) {
	// 计算相位增量，公式：phaseStep = (frequency / sampleRate) * 2^32
	g_phaseStep = (int32_t)(((double)g_targetFreq / g_sampleRate) * 4294967296.0);
}

void App_Generator_Init(void){
	/*
	 * TODO: 初始化信号源相关的定时器和 GPIO
	*/
	// 1. DAC 自校准与使能
	DL_DAC12_performSelfCalibrationBlocking(DAC0);
	DL_DAC12_enable(DAC0);
	DL_DAC12_enableOutputPin(DAC0);

	// 2. MATHACL 配置
	// 设置功能为 SINCOS (1h)，迭代次数建议 24 次以获得极高精度
	MATHACL->CTL = (MATHACL_CTL_FUNC_SINCOS << MATHACL_CTL_FUNC_OFS) | 
               (24 << MATHACL_CTL_NUMITER_OFS);

	// 3. 初始参数计算
	g_currentPhase = 0;
	Generator_UpdateStep(); // 根据初始频率计算步长

	// 4. 开启定时器中断
	NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
	//DL_TimerG_startCounter(TIMER_1_INST);
}

void Generator_SetFrequency(uint32_t freq){
	/*
	 * TODO: 设置信号源频率
	 */
}



// ==========================================
// 5. UI 绘制实现 (待填写)
// ==========================================

void UI_DrawGreeting(void) {
    // TODO: 使用 OLED_ShowString 绘制欢迎界面
    OLED_ShowString(8, 2, "Signal Station");
	OLED_ShowString(24, 5, "Press User");
}

void UI_DrawMenu(void) {
    // TODO: 使用 OLED_ShowString 绘制主菜单列表
	OLED_ShowString(0, 0, "Main Menu");
	OLED_ShowString(0, 3, "A. Voltmeter");
	OLED_ShowString(0, 4, "B. Generator");
	OLED_ShowString(0, 5, "C. History");
}

void UI_DrawVoltmeter(void) {
    // TODO: 绘制电压表界面，显示 g_voltValue 的实时数值
}


void UI_DrawGenerator(void) {
    // TODO: 绘制信号源界面，显示当前频率和波形类型
	char buf[16];
	OLED_ShowString(0, 0, "Generator");
    OLED_ShowString(0, 2, "Wave: Sine");
	sprintf(buf, "Freq: %d Hz", g_targetFreq); // 动态显示当前频率
	OLED_ShowString(0, 4, (uint8_t *)buf);
    // OLED_ShowString(0, 4, "Freq: 100 Hz");
    OLED_ShowString(0, 7, "Press D to Back"); // 提示用户怎么回去
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