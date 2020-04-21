/*

 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * A simple example that shows a stream buffer being used to pass data from an
 * interrupt to a task.
 *
 * There are two strings, pcStringToSend and pcStringToReceive, where
 * pcStringToReceive is a substring of pcStringToSend.  The interrupt sends
 * a few bytes of pcStringToSend to a stream buffer ever few times that it
 * executes.  A task reads the bytes from the stream buffer, looking for the
 * substring, and flagging an error if the received data is invalid.
 */

/* Standard includes. */
#include "stdio.h"
#include "string.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

/* Demo app includes. */
#include "StreamBufferInterrupt.h"
   
#include <sh_shell.h>
#include <shell.h>

#include "includes.h"

#include "user_cmd.h"


#define sbiSTREAM_BUFFER_LENGTH_BYTES		 ( ( size_t ) 100 )
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_10	( ( BaseType_t ) 10 )
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_1 	( ( BaseType_t ) 1 )

/*-----------------------------------------------------------*/
#define RX_BUFFER_SZIE              100u
static uint8_t rxReceiveBuffer[ RX_BUFFER_SZIE ];
/*-----------------------------------------------------------*/

/* Implements the task that receives a stream of bytes from the interrupt. */
static void prvReceivingTask( void *pvParameters );



static HAL_StatusTypeDef USER_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);


static void USER_UART_RxISR_8BIT(UART_HandleTypeDef *huart);



/*-----------------------------------------------------------*/

/* The stream buffer that is used to send data from an interrupt to the task. */
static StreamBufferHandle_t xStreamBuffer = NULL;

/* The string that is sent from the interrupt to the task four bytes at a
time.  Must be multiple of 4 bytes long as the ISR sends 4 bytes at a time*/
static const char * pcStringToSend = "_____Hello FreeRTOS_____";

/* The string to task is looking for, which must be a substring of
pcStringToSend. */
static const char * pcStringToReceive = "Hello FreeRTOS";

/* Set to pdFAIL if anything unexpected happens. */
static BaseType_t xDemoStatus = pdPASS;

/* Incremented each time pcStringToReceive is correctly received, provided no
errors have occurred.  Used so the check task can check this task is still
running as expected. */
static uint32_t ulCycleCount = 0;

static const char ShShell_Init_err[] = "ShShell_Init is Err.\r\n";
static const char ShShell_Init_done[] =  "ShShell_Init is Done.\r\n";
static const char userShell_Init_Done[] = "User shell init is done.\r\n";
static const char userShell_Init_Err[] = "User shell init is error:\r\n";







void vStartStreamBufferInit( void )
{

    
    Shell_Init();
    if ( ShShell_Init() != DEF_OK)
    {
        HAL_UART_Transmit(&huart2, (uint8_t *)ShShell_Init_err, sizeof( ShShell_Init_err ), 100u);
    }
    HAL_UART_Transmit(&huart2, (uint8_t *)ShShell_Init_done, sizeof( ShShell_Init_done ), 100u);
    if ( add_user_cmd() != DEF_OK)
    {
           HAL_UART_Transmit(&huart2, (uint8_t *)userShell_Init_Err, sizeof( userShell_Init_Err ), 100u); 
    }
     HAL_UART_Transmit(&huart2, (uint8_t *)userShell_Init_Done, sizeof( userShell_Init_Done ), 100u);
	/* Create the stream buffer that sends data from the interrupt to the
	task, and create the task. */
	xStreamBuffer = xStreamBufferCreate( /* The buffer length in bytes. */
										 sbiSTREAM_BUFFER_LENGTH_BYTES,
										 /* The stream buffer's trigger level. */
										 sbiSTREAM_BUFFER_TRIGGER_LEVEL_1 );


    USER_UART_Receive_IT(&huart2, rxReceiveBuffer, RX_BUFFER_SZIE);



}


/*-----------------------------------------------------------*/

BaseType_t xIsInterruptStreamBufferDemoStillRunning( void )
{
uint32_t ulLastCycleCount = 0;

	/* Check the demo is still running. */
	if( ulLastCycleCount == ulCycleCount )
	{
		xDemoStatus = pdFAIL;
	}
	else
	{
		ulLastCycleCount = ulCycleCount;
	}

	return xDemoStatus;
}


/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//
//    static size_t xNextByteToSend = 0;
//    const BaseType_t xCallsBetweenSends = 100, xBytesToSend = 4;
    const BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    
    /* Is it time to write to the stream buffer again? */
    size_t xDataLengthBytes = huart->RxXferSize;


    /* Send the next four bytes to the stream buffer. */
    size_t sendCnt = xStreamBufferSendFromISR( xStreamBuffer,
                              ( const void * ) rxReceiveBuffer ,
                              xDataLengthBytes,
                              (BaseType_t *) &pxHigherPriorityTaskWoken );

    /* Send the next four bytes the next time around, wrapping to the start
    of the string if necessary. */
    if ( sendCnt <= xDataLengthBytes)
    {
        // 没有传送完全.
    }
    
    HAL_UART_Receive_IT(&huart2, rxReceiveBuffer, 1u);
}



/*
*********************************************************************************************************
*                                       TerminalSerial_RdByte()
*
* Description : Serial byte input.
*
* Argument(s) : none.
*
* Return(s)   : Byte read from port.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

#include  <terminal.h>

CPU_INT08U  TerminalSerial_RdByte (void)
{
//    char cRxBuffer[ 20 ];
    char cRxByte;
//    BaseType_t xNextByte = 0;
    size_t rdByteCnt = 0u;


    /* Keep receiving characters until the end of the string is received.
    Note:  An infinite block time is used to simplify the example.  Infinite
    block times are not recommended in production code as they do not allow
    for error recovery. */
    
    rdByteCnt = xStreamBufferReceive( /* The stream buffer data is being received from. */
                          xStreamBuffer,
                          /* Where to place received data. */
                          ( void * ) &( cRxByte ),
                          /* The number of bytes to receive. */
                          sizeof( char ),
                          /* The time to wait for the next data if the buffer
                          is empty. */
                          portMAX_DELAY );

    if ( rdByteCnt > 0u)
    {
//        HAL_UART_Transmit(&huart2, ( uint8_t *) &cRxByte, 1u, 100u);
        return ( cRxByte );
        
    }
    else 
        return (0u);
}


typedef struct sendTxtCMD
{
    const int8_t *pTxt;
    uint8_t txtLen;
    
}TestTxtCmd;
    
const int8_t help[] = "1help\r";
const int8_t tab1[] = "2Test\r";
const int8_t tab2[] = "3last\r";
const int8_t tab3[] = "?\r";

#define CMD_COUNT      (sizeof(sendCMD)/sizeof(sendCMD[0]))
#define ADD_CMD_QUANTITY                4

void RxReceive(void *argument)
{
    const TestTxtCmd sendCMD[ADD_CMD_QUANTITY] = 
    {
        {help, sizeof(help)},
        {tab1, sizeof(tab1)},
        {tab2, sizeof(tab2)},
        {tab3, sizeof(tab3)}

    };
    static uint8_t cmdCnt = 0u;
    

    (void)argument;

//    for(;;);

    for(;;)
    {
      osDelay(8000);
      
     size_t sendCnt = xStreamBufferSend( xStreamBuffer,
                          ( const void * ) sendCMD[cmdCnt].pTxt ,
                          sendCMD[cmdCnt].txtLen,
                          portMAX_DELAY );
     cmdCnt ++;
     if ( ADD_CMD_QUANTITY <= cmdCnt) 
     {
        cmdCnt = 0u;
     }
    }



}


/**
  * @brief Receive an amount of data in interrupt mode.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the received data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 available through pData.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         address of user data buffer for storing data to be received, should be aligned on a half word frontier (16 bits)
  *         (as received data will be handled using u16 pointer cast). Depending on compilation chain,
  *         use of specific alignment compilation directives or pragmas might be required to ensure proper alignment for pData.
  * @param huart UART handle.
  * @param pData Pointer to data buffer (u8 or u16 data elements).
  * @param Size  Amount of data elements (u8 or u16) to be received.
  * @retval HAL status
  */
static HAL_StatusTypeDef USER_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    __HAL_LOCK(huart);

    huart->pRxBuffPtr  = pData;
    huart->RxXferSize  = Size;
    huart->RxXferCount = Size;
//    huart->RxISR       = NULL;

    /* Computation of UART mask to apply to RDR register */
    UART_MASK_COMPUTATION(huart);

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

    /* Configure Rx interrupt processing*/

    huart->RxISR = USER_UART_RxISR_8BIT;
    __HAL_UNLOCK(huart);

    /* Enable the UART Parity Error interrupt and RX FIFO Threshold interrupt */
    SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    SET_BIT(huart->Instance->CR3, USART_CR3_RXFTIE);

    __HAL_UNLOCK(huart);

    /* Enable the UART Parity Error interrupt and Data Register Not Empty interrupt */
    SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}





/**
  * @brief RX interrrupt handler for 7 or 8  bits data word length and FIFO mode is enabled.
  * @note   Function is called under interruption only, once
  *         interruptions have been enabled by HAL_UART_Receive_IT()
  * @param huart UART handle.
  * @retval None
  */

static uint8_t rxData;
static void USER_UART_RxISR_8BIT(UART_HandleTypeDef *huart)
{
  uint16_t  uhMask = huart->Mask;
  uint16_t  uhdata;
  uint16_t   nb_rx_data;
  uint16_t  rxdatacount;
  
//  uint8_t rxData;
  const BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    for (nb_rx_data = huart->NbRxDataToProcess ; nb_rx_data > 0U ; nb_rx_data--)
    {
      uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
      rxData = (uint8_t)(uhdata & (uint8_t)uhMask);
      
    size_t sendCnt = xStreamBufferSendFromISR( xStreamBuffer,
                              ( const void * ) &rxData ,
                              sizeof( rxData ),
                                  (BaseType_t *) &pxHigherPriorityTaskWoken );
        if ( sendCnt == 0)
        {

            // 错误处理.

        }

    }


      CLEAR_BIT(huart->Instance->CR3, USART_CR3_RXFTIE);

      /* Update the RxISR function pointer */
      huart->RxISR = USER_UART_RxISR_8BIT;

      /* Enable the UART Data Register Not Empty interrupt */
      SET_BIT(huart->Instance->CR1, USART_CR1_RXNEIE_RXFNEIE);
//    }
  }
  else
  {
    /* Clear RXNE interrupt flag */
    __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
  }
}

