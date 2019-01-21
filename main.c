
#include "KeyboardHID.h"
#include "german_keyboardCodes.h"

/* Macros: */
#define LED1 P1_1
#define LED2 P1_0
#define TICKS_PER_SECOND 1000
#define TICKS_WAIT 200
/* Systick */
void SysTick_Handler(void);

uint32_t timeMeasured[12];
uint32_t timeNumOn[24];
uint32_t timeNumOff[24];
uint32_t timeCapsOn[24];
uint32_t timeCapsOff[24];
static uint8_t indexNumOn =0;
static uint8_t indexNumOff =0;
static uint8_t indexCapsOn =0;
static uint8_t indexCapsOff =0;
static uint32_t ticks = 0;

static uint8_t stringAlphabet[49]={
GERMAN_KEYBOARD_SC_A,
GERMAN_KEYBOARD_SC_B,
GERMAN_KEYBOARD_SC_C,
GERMAN_KEYBOARD_SC_D,
GERMAN_KEYBOARD_SC_E,
GERMAN_KEYBOARD_SC_F,
GERMAN_KEYBOARD_SC_G,
GERMAN_KEYBOARD_SC_H,
GERMAN_KEYBOARD_SC_I,
GERMAN_KEYBOARD_SC_J,
GERMAN_KEYBOARD_SC_K,
GERMAN_KEYBOARD_SC_L,
GERMAN_KEYBOARD_SC_M,
GERMAN_KEYBOARD_SC_N,
GERMAN_KEYBOARD_SC_O,
GERMAN_KEYBOARD_SC_P,
GERMAN_KEYBOARD_SC_Q,
GERMAN_KEYBOARD_SC_R,
GERMAN_KEYBOARD_SC_S,
GERMAN_KEYBOARD_SC_T,
GERMAN_KEYBOARD_SC_U,
GERMAN_KEYBOARD_SC_V,
GERMAN_KEYBOARD_SC_W,
GERMAN_KEYBOARD_SC_X,
GERMAN_KEYBOARD_SC_Y,
GERMAN_KEYBOARD_SC_Z,
GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION,
GERMAN_KEYBOARD_SC_2_AND_QUOTES,
GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH,
GERMAN_KEYBOARD_SC_4_AND_DOLLAR,
GERMAN_KEYBOARD_SC_5_AND_PERCENTAGE,
GERMAN_KEYBOARD_SC_6_AND_AMPERSAND,
GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,
GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET,
GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET,
GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,
GERMAN_KEYBOARD_SC_SHARP_S_AND_QUESTION_AND_BACKSLASH,
GERMAN_KEYBOARD_SC_CARET_AND_DEGREE,
GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE ,
GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE,
GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON,
GERMAN_KEYBOARD_SC_DOT_AND_COLON,
GERMAN_KEYBOARD_SC_ENTER,
GERMAN_KEYBOARD_SC_ESCAPE,
GERMAN_KEYBOARD_SC_BACKSPACE,
GERMAN_KEYBOARD_SC_TAB,
GERMAN_KEYBOARD_SC_SPACE,
GERMAN_KEYBOARD_SC_HASHMARK_AND_APOSTROPHE,
GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE
};

/* Clock configuration */
XMC_SCU_CLOCK_CONFIG_t clock_config = {
	.syspll_config.p_div  = 2,
	.syspll_config.n_div  = 80,
	.syspll_config.k_div  = 4,
	.syspll_config.mode   = XMC_SCU_CLOCK_SYSPLL_MODE_NORMAL,
	.syspll_config.clksrc = XMC_SCU_CLOCK_SYSPLLCLKSRC_OSCHP,
	.enable_oschp         = true,
	.calibration_mode     = XMC_SCU_CLOCK_FOFI_CALIBRATION_MODE_FACTORY,
	.fsys_clksrc          = XMC_SCU_CLOCK_SYSCLKSRC_PLL,
	.fsys_clkdiv          = 1,
	.fcpu_clkdiv          = 1,
	.fccu_clkdiv          = 1,
	.fperipheral_clkdiv   = 1
};

/* Forward declaration of HID callbacks as defined by LUFA */
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize );

void CALLBACK_HID_Device_ProcessHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							const uint8_t ReportID,
							const uint8_t ReportType,
							const void* ReportData,
							const uint16_t ReportSize );

void SystemCoreClockSetup(void);

/**
 * Main program entry point. This routine configures the hardware required by
 * the application, then enters a loop to run the application tasks in sequence.
 */
int main(void) {
	/* System timer configuration */
	SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);

	// Init LED pins for debugging and NUM/CAPS visual report
	XMC_GPIO_SetMode(LED1,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_SetMode(LED2,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	USB_Init();

	// Wait until host has enumerated HID device
	for(int i = 0; i < 10e6; ++i)
		;


	while (1) {
		HID_Device_USBTask(&Keyboard_HID_Interface);
	}
}

// Callback function called when a new HID report needs to be created
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize ) {
	USB_KeyboardReport_Data_t* report = (USB_KeyboardReport_Data_t *)ReportData;
	*ReportSize = sizeof(USB_KeyboardReport_Data_t);
	static uint8_t characterSent = 0,
				   indexToSend = 0,
					 IndexAlphabet =0,
					 indexMod=0;

// modifire array
static uint8_t modArray[3]={0,HID_KEYBOARD_MODIFIER_RIGHTSHIFT,HID_KEYBOARD_MODIFIER_RIGHTALT};

	// string to be sent
	static uint8_t stringToSend[5] = {
		GERMAN_KEYBOARD_SC_F,
		GERMAN_KEYBOARD_SC_D,
		GERMAN_KEYBOARD_SC_Y,
		GERMAN_KEYBOARD_SC_F,
		GERMAN_KEYBOARD_SC_ENTER,
	};
	stringToSend[2] =	stringAlphabet[IndexAlphabet];


	if(indexToSend < 5) {
		if(characterSent) {
			report->Modifier = 0;
			report->Reserved = 0;
			report->KeyCode[0] = 0;
			characterSent = 0;
	//		timeMeasured[indexToSend]=ticks;
			++indexToSend;
		} else {
			//if (indexToSend == 1 || indexToSend == 5){report->Modifier =HID_KEYBOARD_MODIFIER_RIGHTSHIFT;}
			//else {report->Modifier = 0;}//HID_KEYBOARD_MODIFIER_RIGHTALT;// altgr = 6 = 0x40
			report->Modifier = modArray[indexMod];
			report->Reserved = 0;
			report->KeyCode[0] = stringToSend[indexToSend];
			characterSent = 1;
		//	ticks = 0
		}
	} else if(IndexAlphabet <30 && indexMod <2){
		// indexNumOn  = 0;
		// indexNumOff = 0;

		indexCapsOn = 0;
		indexCapsOff= 0;
		indexToSend = 0;
		IndexAlphabet++;
		//printf("%d\n", timeMeasured[0]);
	} else if (indexMod >= 2)
	{
		IndexAlphabet =0;
		//indexMod=0;
		indexToSend =12;
	} else //if (IndexAlphabet >=49 && indexMod <1)
	{
		IndexAlphabet =0;
		indexToSend = 0;
		indexMod++;
	}


	return true;
}

// Called on report input. For keyboard HID devices, that's the state of the LEDs
void CALLBACK_HID_Device_ProcessHIDReport(
						USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
						const uint8_t ReportID,
						const uint8_t ReportType,
						const void* ReportData,
						const uint16_t ReportSize ) {
	uint8_t *report = (uint8_t*)ReportData;

	if(*report & HID_KEYBOARD_LED_NUMLOCK)
		{XMC_GPIO_SetOutputHigh(LED1);
		timeNumOn[indexNumOn]=ticks;
		indexNumOn++;}
	else
		{XMC_GPIO_SetOutputLow(LED1);
		timeNumOff[indexNumOff]=ticks;
		indexNumOff++;
		if(indexNumOff == 3)
			{
				int loop;
				for(loop = 0; loop < 3; loop++)
	      {
					//printf("%d ", timeNumOff[loop]);//printf("%d\n%d\n%d\n", timeNumOff[0], timeNumOff[1], timeNumOff[2]);}
				}
 			}
		}
	if(*report & HID_KEYBOARD_LED_CAPSLOCK)
		{XMC_GPIO_SetOutputHigh(LED2);
		timeCapsOn[indexCapsOn]=ticks;
		indexCapsOn++;
		}
	else
		{XMC_GPIO_SetOutputLow(LED2);
		timeCapsOff[indexCapsOff]=ticks;
		indexCapsOff++;
		}
}

void SystemCoreClockSetup(void) {
	/* Setup settings for USB clock */
	XMC_SCU_CLOCK_Init(&clock_config);

	XMC_SCU_CLOCK_EnableUsbPll();
	XMC_SCU_CLOCK_StartUsbPll(2, 64);
	XMC_SCU_CLOCK_SetUsbClockDivider(4);
	XMC_SCU_CLOCK_SetUsbClockSource(XMC_SCU_CLOCK_USBCLKSRC_USBPLL);
	XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_USB);

	SystemCoreClockUpdate();
}

void SysTick_Handler(void)
{  //static uint32_t ticks = 0;

  ticks++;
}
