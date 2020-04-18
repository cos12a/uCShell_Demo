

/*
*********************************************************************************************************
*
*                                         GENERAL SHELL COMMANDS
*
* Filename : sh_shell.c
* Version  : V1.04.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   SH_SHELL_MODULE
#include <sh_shell.h>


/*
*********************************************************************************************************
*                                              LOCAL DEFINES
*********************************************************************************************************
*/

#define  SH_SHELL_NEW_LINE                      (CPU_CHAR *)"\r\n"
#define  SH_SHELL_BLANK_LINE                    (CPU_CHAR *)"\r\n\r\n"
#define  SH_SHELL_STR_HELP                      (CPU_CHAR *)"-h"

/*
*********************************************************************************************************
*                                        ARGUMENT ERROR MESSAGES
*********************************************************************************************************
*/

#define  SH_SHELL_ARG_ERR_HELP                  (CPU_CHAR *)"Sh_help: usage: Sh_help\r\n                Sh_help [command]"

/*
*********************************************************************************************************
*                                      COMMAND EXPLANATION MESSAGES
*********************************************************************************************************
*/

#define  SH_SHELL_CMD_EXP_HELP                  (CPU_CHAR *)"                Get list of commands, or information about a command."


/*
*********************************************************************************************************
*                                             LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/



static  CPU_INT16S  ShShell_Test(CPU_INT16U        argc,
                                 CPU_CHAR         *argv[],
                                 SHELL_OUT_FNCT    out_fnct,
                                 SHELL_CMD_PARAM  *pcmd_param);

static  SHELL_CMD  ShShell_CmdTbl [] =
{
    {"Sh_help", ShShell_help},
    {0,         0           }
};


/*
*********************************************************************************************************
*                                       LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             ShShell_Init()
*
* Description : Initialize Shell for general shell commands.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if general shell commands were added.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  ShShell_user_cmd (void)
{
    SHELL_ERR    err;
    CPU_BOOLEAN  ok;


    Shell_CmdTblAdd((CPU_CHAR *)"Sh", ShShell_CmdTbl, &err);

    ok = (err == SHELL_ERR_NONE) ? DEF_OK : DEF_FAIL;
    return (ok);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           COMMAND FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             ShShell_help()
*
* Description : List all commands or invoke '--help' for another command.
*
* Argument(s) : argc            The number of arguments.
*
*               argv            Array of arguments.
*
*               out_fnct        The output function.
*
*               pcmd_param      Pointer to the command parameters.
*
* Return(s)   : SHELL_EXEC_ERR, if an error is encountered.
*               SHELL_ERR_NONE, otherwise.
*
* Caller(s)   : Shell, in response to command execution.
*
* Note(s)     : none.
*********************************************************************************************************
*/




static  CPU_INT16S  ShShell_help (CPU_INT16U        argc,
                                  CPU_CHAR         *argv[],
                                  SHELL_OUT_FNCT    out_fnct,
                                  SHELL_CMD_PARAM  *pcmd_param)
{
    CPU_CHAR           cmd_str[SHELL_CFG_MODULE_CMD_NAME_LEN_MAX + 4];
    SHELL_ERR          err;
    SHELL_CMD         *pcmd;
    SHELL_MODULE_CMD  *pmodule_cmd;



}

typedef  CPU_INT16S  (*SHELL_CMD_FNCT)(CPU_INT16U         argc,
                                       CPU_CHAR          *argv[],
                                       SHELL_OUT_FNCT     out_fnct,
                                       SHELL_CMD_PARAM   *p_cmd_param);



static  CPU_INT16S  ShShell_test_cmd (CPU_INT16U        argc,
                                  CPU_CHAR         *argv[],
                                  SHELL_OUT_FNCT    out_fnct,
                                  SHELL_CMD_PARAM  *pcmd_param)
{
    CPU_CHAR           cmd_str[SHELL_CFG_MODULE_CMD_NAME_LEN_MAX + 4];
    SHELL_ERR          err;
    SHELL_CMD         *pcmd;
    SHELL_MODULE_CMD  *pmodule_cmd;



}

