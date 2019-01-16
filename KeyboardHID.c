#include <string.h>

#include "KeyboardHID.h"
#include "system_XMC4500.h"

// For comparision within the driver. I think we don't need it directly
static uint8_t PrevHIDReportBuffer[KEYBOARD_REPORT_SIZE];

USB_ClassInfo_HID_Device_t Keyboard_HID_Interface = {
	.Config = {
		.InterfaceNumber              = INTERFACE_ID_KeyboardHID,
		.ReportINEndpoint             = {
			.Address              = KEYBOARD_IN_EPADDR,
			.Size                 = KEYBOARD_REPORT_SIZE,
			.Banks                = 1,
		},
		.PrevReportINBuffer           = PrevHIDReportBuffer,
		.PrevReportINBufferSize       = sizeof(PrevHIDReportBuffer),
	},
};

uint8_t buffer[KEYBOARD_REPORT_SIZE];

/* USB runtime structure*/
XMC_USBD_t USB_runtime = {
	.usbd = USB0,
	.usbd_max_num_eps = XMC_USBD_MAX_NUM_EPS_6,
	.usbd_transfer_mode = XMC_USBD_USE_FIFO,
	.cb_xmc_device_event = USBD_SignalDeviceEventHandler,
	.cb_endpoint_event = USBD_SignalEndpointEvent_Handler
};

void USB0_0_IRQHandler(void) {
	XMC_USBD_IRQHandler(&USB_runtime);
}

/*The function initializes the USB core layer and register call backs. */
void USB_Init(void) {
	USBD_Initialize(&USB_runtime);

	/* Interrupts configuration*/
	NVIC_SetPriority( USB0_0_IRQn,
					  NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0) );
	NVIC_ClearPendingIRQ(USB0_0_IRQn);
	NVIC_EnableIRQ(USB0_0_IRQn);

	/* USB Connection*/
	USB_Attach();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_(void) {
}
/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void) {
	//Device is ready!
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Reset(void) {
	if(device.IsConfigured) {
		USB_Init();
		device.IsConfigured=0;
	}
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	USBD_SetEndpointBuffer(KEYBOARD_IN_EPADDR, buffer, KEYBOARD_REPORT_SIZE);

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);

	device.IsConfigured = ConfigSuccess;
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void) {
	HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
}

bool CALLBACK_HIDParser_FilterHIDReportItem(
									HID_ReportItem_t* const CurrentItem ) {
	return true;
}
