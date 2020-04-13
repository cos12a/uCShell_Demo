/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
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

#define sbiSTREAM_BUFFER_LENGTH_BYTES		 ( ( size_t ) 100 )
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_10	( ( BaseType_t ) 10 )
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_1 	( ( BaseType_t ) 1 )

/*-----------------------------------------------------------*/
#define RX_BUFFER_SZIE              100u
static uint8_t rxReceiveBuffer[ RX_BUFFER_SZIE ];
/*-----------------------------------------------------------*/

/* Implements the task that receives a stream of bytes from the interrupt. */
static void prvReceivingTask( void *pvParameters );

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

void vStartStreamBufferInit( void )
{

    
    Shell_Init();
    if ( ShShell_Init() != DEF_OK)
    {

        HAL_UART_Transmit(&huart2, "ShShell_Init is Err.", 1u, 100u);
    }
    HAL_UART_Transmit(&huart2, "ShShell_Init is Done.", 1u, 100u);
	/* Create the stream buffer that sends data from the interrupt to the
	task, and create the task. */
	xStreamBuffer = xStreamBufferCreate( /* The buffer length in bytes. */
										 sbiSTREAM_BUFFER_LENGTH_BYTES,
										 /* The stream buffer's trigger level. */
										 sbiSTREAM_BUFFER_TRIGGER_LEVEL_1 );

#if (DEBUG_TEST_COM_ENABLE == DEF_EN)

    HAL_UART_Receive_IT(&huart2, rxReceiveBuffer, 1u);

#endif 

}



#if 0

/*-----------------------------------------------------------*/

void vStartStreamBufferInterruptDemo( void )
{
	/* Create the stream buffer that sends data from the interrupt to the
	task, and create the task. */
	xStreamBuffer = xStreamBufferCreate( /* The buffer length in bytes. */
										 sbiSTREAM_BUFFER_LENGTH_BYTES,
										 /* The stream buffer's trigger level. */
										 sbiSTREAM_BUFFER_TRIGGER_LEVEL_10 );

	xTaskCreate( prvReceivingTask, /* The function that implements the task. */
				 "StrIntRx",	   /* Human readable name for the task. */
				 configMINIMAL_STACK_SIZE,	/* Stack size (in words!). */
				 NULL,			   /* Task parameter is not used. */
				 tskIDLE_PRIORITY + 2, /* The priority at which the task is created. */
				 NULL );		   /* No use for the task handle. */
}
/*-----------------------------------------------------------*/





static void prvReceivingTask( void *pvParameters )
{
char cRxBuffer[ 20 ];
BaseType_t xNextByte = 0;

	/* Remove warning about unused parameters. */
	( void ) pvParameters;

	/* Make sure the string will fit in the Rx buffer, including the NULL
	terminator. */
	configASSERT( sizeof( cRxBuffer ) > strlen( pcStringToReceive ) );

	/* Make sure the stream buffer has been created. */
	configASSERT( xStreamBuffer != NULL );

	/* Start with the Rx buffer in a known state. */
	memset( cRxBuffer, 0x00, sizeof( cRxBuffer ) );

	for( ;; )
	{
		/* Keep receiving characters until the end of the string is received.
		Note:  An infinite block time is used to simplify the example.  Infinite
		block times are not recommended in production code as they do not allow
		for error recovery. */
		xStreamBufferReceive( /* The stream buffer data is being received from. */
							  xStreamBuffer,
							  /* Where to place received data. */
							  ( void * ) &( cRxBuffer[ xNextByte ] ),
							  /* The number of bytes to receive. */
							  sizeof( char ),
							  /* The time to wait for the next data if the buffer
							  is empty. */
							  portMAX_DELAY );

		/* If xNextByte is 0 then this task is looking for the start of the
		string, which is 'H'. */
		if( xNextByte == 0 )
		{
			if( cRxBuffer[ xNextByte ] == 'H' )
			{
				/* The start of the string has been found.  Now receive
				characters until the end of the string is found. */
				xNextByte++;
			}
		}
		else
		{
			/* Receiving characters while looking for the end of the string,
			which is an 'S'. */
			if( cRxBuffer[ xNextByte ] == 'S' )
			{
				/* The string has now been received.  Check its validity. */
				if( strcmp( cRxBuffer, pcStringToReceive ) != 0 )
				{
					xDemoStatus = pdFAIL;
				}

				/* Return to start looking for the beginning of the string
				again. */
				memset( cRxBuffer, 0x00, sizeof( cRxBuffer ) );
				xNextByte = 0;

				/* Increment the cycle count as an indication to the check task
				that this demo is still running. */
				if( xDemoStatus == pdPASS )
				{
					ulCycleCount++;
				}
			}
			else
			{
				/* Receive the next character the next time around, while
				continuing to look for the end of the string. */
				xNextByte++;

				configASSERT( ( size_t ) xNextByte < sizeof( cRxBuffer ) );
			}
		}
	}
}
/*-----------------------------------------------------------*/

void vBasicStreamBufferSendFromISR( void )
{
static size_t xNextByteToSend = 0;
const BaseType_t xCallsBetweenSends = 100, xBytesToSend = 4;
static BaseType_t xCallCount = 0;


	/* Is it time to write to the stream buffer again? */
	xCallCount++;
	if( xCallCount > xCallsBetweenSends )
	{
		xCallCount = 0;

		/* Send the next four bytes to the stream buffer. */
		xStreamBufferSendFromISR( xStreamBuffer,
								  ( const void * ) ( pcStringToSend + xNextByteToSend ),
								  xBytesToSend,
								  NULL );

		/* Send the next four bytes the next time around, wrapping to the start
		of the string if necessary. */
		xNextByteToSend += xBytesToSend;

		if( xNextByteToSend >= strlen( pcStringToSend ) )
		{
			xNextByteToSend = 0;
		}
	}
}

#endif


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

#if 0

/* USER CODE BEGIN Header_RxReceive */
/**
* @brief Function implementing the RxTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RxReceive */
void RxReceive(void *argument)
{

    char cRxBuffer[ 20 ];
    BaseType_t xNextByte = 0;

    /* Remove warning about unused parameters. */
    ( void ) pvParameters;

    /* Make sure the string will fit in the Rx buffer, including the NULL
    terminator. */
    configASSERT( sizeof( cRxBuffer ) > strlen( pcStringToReceive ) );

    /* Make sure the stream buffer has been created. */
    configASSERT( xStreamBuffer != NULL );

    /* Start with the Rx buffer in a known state. */
    memset( cRxBuffer, 0x00, sizeof( cRxBuffer ) );







  /* USER CODE BEGIN RxReceive */
  /* Infinite loop */
  for(;;)
  {
    osDelay(10);
  }
  /* USER CODE END RxReceive */
}

#endif


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
    


void RxReceive(void *argument)
{
    const TestTxtCmd sendCMD[3] = 
    {
        {"1help", 5u},
        {"2Test", 5u},
        {"3last", 5u}

    };
    static uint8_t cmdCnt = 0u;
    

    (void)argument;


    for(;;)
    {
      osDelay(5000);
      
     size_t sendCnt = xStreamBufferSend( xStreamBuffer,
                          ( const void * ) sendCMD[cmdCnt].pTxt ,
                          sendCMD[cmdCnt].txtLen,
                          portMAX_DELAY );
     cmdCnt ++;
     if ( 3u <= cmdCnt) 
     {
        cmdCnt = 0u;
     }
    }



}


