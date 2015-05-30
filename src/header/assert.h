/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-01-12
 *      Updated:    2015-05-29
 *
 *      Project:    bOS
 *      File name:  assert.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 6       Updated
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/
 
#include "log.h"

extern void _deadloop(void);

#undef assert
#undef ASSERT

#undef NDEBUG           // Move this to sysdef.h or local.h and include that header...
 
#ifdef NDEBUG                               // required by ANSI standard
#define ASSERT(__bool) ((void) 0)
#else
#define ASSERT(__bool) (__bool) ? ((void) 0) : (log_assert(__FILE__, __LINE__), _deadloop())
#endif

