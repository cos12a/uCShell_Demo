/*
*********************************************************************************************************
*                                
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              TERMINAL
*
* Filename : includes.h
* Version  : V0.01.00
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               TERMINAL present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef __INCLUDES_H__
#define __INCLDES_H__




#define DEF_EN                      1u

#define DEBUG_TEST_COM_EN             DEF_EN





#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "StreamBufferInterrupt.h"
#include "usart.h"

#if (DEBUG_TEST_COM_ENABLE == DEF_EN)

#include "shell.h"
#include "sh_shell.h"

#endif 






#endif
