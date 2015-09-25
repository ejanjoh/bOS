# bOS

> ## Content
>
> 1.	Introduction
> 2.	File Structure
> 3.	Revision History
> 4.	Key Words
> 5.	To Do


> ## Introduction
>
> The idea behind this project is to make an simple prototype for a realtime operating system. It's not in any respects to be seen as complete nor fully functional. It's just a prototype and "just for fun". One basic principle used is that the processes should as long as possible take the 'costs' associated to respective process and in this sense try to minimize the system overhead. There are obvious drawbacks, but in a very small embedded system where the developer have full control over the implementation this drawbacks is mitigated somewhat. In addition, there will not be any distinction between kernel space and user land, it's a mix.
> 
> I wrote the first draft some years ago and based it on a Raspberry Pi. On this updated version I have extended the platforms to Beaglebone Black as well. It should be possible to a limited extra work add other platforms, especially ARM based. 
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
> - ver 10: 
> 	- [bOS] Command Line Interface (CLI) added
> 	- [bOS] I/O stream buffers added and I/O functions changed accordingly
> 	- [bOS] Changed the semaphore_signal behavior
> 	- [bOS] Changed e.o.l. characters from "\r\n" to "\n"
> 	- [Raspberry Pi] UART interrupts enabled to get terminal serial input to the system 
> 	- [Beaglebone Black] UART interrupts enabled to get terminal serial input to the system 


>## Key Words
> Assembler, C, ARM, ARMv6, ARMv7, Bare Metal, Operating System (OS), Rasberry Pi (RPI), Beaglebone Black (BBB), Stack, GPIO, Led, Exception Vectors, UART/Serial Communication, Rx/Tx, RHR/THR, Timer, Interrupt, IRQ, SVC/SWI, Context Switch, Scheduler, __aeabi_uidiv, I/O, Semaphore, Mutex, Assert, Process Control Block (PCB), Command Line Interface (CLI), I/O stream buffers…

>## To Do
> - Add a heap and dynamic memory allocation (a heap already exist in the system but isn't used…)
> - Add inter process communication
> - Update information on the system and make a cleanup in the code.


