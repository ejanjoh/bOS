/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-07-16
 *      Updated:    2015-09-08
 *
 *      Project:    bOS
 *      File name:  sys_def.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 9       Created
 *      ver 10      Added PID list
 *
 *
 *      Reference: 
 *
 ******************************************************************************/


#ifndef SYS_DEF_H_
#define SYS_DEF_H_


// supervisor call (in ARMv6 swi and in ARMv7 svc)
#define SYSTEM_CALL_CONTEXT_SWITCH 0x100

// Process Id List (PID List)
#define PID_IDLE        0
#define PID_CLI_IO_IN   1
#define PID_CLI_IO_OUT  2
#define PID_PROC_A      3
#define PID_PROC_B      4
#define PID_PROC_C      5
#define PID_PROC_D      6       // = HIGH_PID defined in process_control.h

#endif /* SYS_DEF_H_ */