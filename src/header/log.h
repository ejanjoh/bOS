/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-01-12
 *      Updated:    2015-05-27
 *
 *      Project:    bOS
 *      File name:  log.h
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
 
 
#undef LOG_WARNING
#undef LOG_WARNING_RETURN
#undef LOG_WARNING_RETURN_NEGATIVE

#undef NDEBUG           // Move this to sysdef.h or local.h and include that header...


#ifdef NDEBUG

#define LOG_WARNING(__bool) ((void) 0)
#define LOG_WARNING_RETURN(__bool) ((void) 0)
#define LOG_WARNING_RETURN_NEGATIVE(__bool) ((void) 0)

#else

// trigger on zero to be consistent with assert...
#define LOG_WARNING(__bool) ((__bool) ? ((void) 0) : log_warning(__FILE__, __LINE__))
#define LOG_WARNING_RETURN(__bool) if (__bool) ((void) 0); \
                                   else {log_warning(__FILE__, __LINE__); return;} ((void) 0)
#define LOG_WARNING_RETURN_NEGATIVE(__bool) if (__bool) ((void) 0); \
                                            else {log_warning(__FILE__, __LINE__); return -1;} ((void) 0)

#endif // !NDEBUG


#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>

void log_warning(const char *file, const uint32_t line);
void log_assert(const char *file, const uint32_t line);

#endif /* LOG_H_ */


 
 
 
 