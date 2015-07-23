# bOS

> ## Content
>
> 1.	Introduction
> 2.	File Structure
> 3.	Revision History


> ## Introduction
>
> To be added...
>
>


> ## GIT File Structure
>
>		|- bOS/
>			|
>			|- targets/
>				|- target_1/
>					|- ver…/
>						|- build/
>						|- extra/
>						|- makefile
>						|- kernel.ld
>						|- src/
>							|- header/
>							|- c/
>							|- asm/
>					|- ver…/
>				|- target_2/
>				|- …
>			|- src/
>				|- headers/
>				|- c/
>			|- makefile    // to be added…
>			|- README.md
>		


> ## Revision History
> 
> - ver 1: Initial file structure created, makefile and linker file for Beaglebone Black added.
> - ver 2: Makefile and linker file for Raspberry Pi added.
> - ver 3: Added for Raspberry Pi:
>	- initial start.S (and support functions)
>	- GPIO and UART for serial communication
>	- the ACT led (OK led) has been has been configured
>	- default io configured and output on UART is working (putchar and putstring configured)
> - ver 4: Fixed the Markdown syntax in README.md
> - ver 5: Added:
>	- [Raspberry Pi] turn the ACT led off when enter _deadloop
>	- [Raspberry Pi] __aeabi_uidiv added to handle unsigned integer division
>	- [Raspberry Pi] changed the io file names
>	- [Beaglebone Black] initial start.S (and support functions added)
>	- [Beaglebone Black] the user leds has been configured
>	- [Beaglebone Black] GPIO and UART for serial communication
>	- [Beaglebone Black] __aeabi_uidiv added to handle unsigned integer division
> - ver 6: Added:
>	- [bOS] Enabled formated output by printf(...)
>	- [bOS] Assert and warnings, (files assert.t and log.h)
> - ver 7: Added:
> 	- [Raspberry Pi] timer enabled interrupts
> 	- [Raspberry Pi] on irq, context switch added
> 	- [Beaglebone Black] timer enabled interrupts
> 	- [Beaglebone Black] on irq, context switch added
> - ver 8:
> 	- [bOS] Process schedule added
> - ver 9:
>   - [bOS] Added methods for processes to enable and disable interrupts
>   - [bOS] Added methods to handle semaphores.
>   - [Raspberry Pi] Added methods for a processes to hand over to an other process. Protected the UART0 used for serial communication with a mutex
>   - [Beaglebone Black] Added methods for a processes to hand over to an other process. Protected the UART0 used for serial communication with a mutex
>	  
>		
