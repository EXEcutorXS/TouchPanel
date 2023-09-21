
/* Includes -----------------------------------------------------*/
//#include "gd32f30x.h"
#include "PeripheralLib.h"
//--------------------------------------------------------
    #define USART_STOPBIT_1                 0
    #define USART_STOPBIT_1_5               1
    #define USART_STOPBIT_2                 2
    #define USART_PARITY_NONE               0
    #define USART_PARITY_EVEN               1
    #define USART_PARITY_ODD                2
    #define USART_DATAFIRST_LSB             0
    #define USART_DATAFIRST_MSB             1
    #define USART_INTERRUPT_TRANSFER        1
    #define USART_DMA_TRANSFER              2
//-------------------------------------------------------       

	typedef enum
	{
		PortA, PortB, PortC, PortD, PortE, PortF, PortG, PortH
	} Port_t;
	//------------------------
	typedef struct
	{
		Port_t port;
		uint8_t pin;// 0-15
	} InitGpio;
	//---------------
	typedef struct
	{
		uint8_t usartNumb; // 0; 1; 2
		uint32_t baudrate;// ..., 2400, ...
		uint8_t word_length; // 7; 8; 9
		uint8_t stop_bit; // 1s; 1.5s; 2s - 1/2/3 
		uint8_t usart_parity; // NONE / EVEN / ODD
		uint8_t data_first; // MSB / LSB
		uint8_t TransferType; // DMA / Interrupt
		InitGpio RxPort;
		InitGpio TxPort;
	} UsartParams_t;
        
//---------------------------------------------------------
