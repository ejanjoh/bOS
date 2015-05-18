/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-01-03
 *      Updated:    2015-05-09
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  gpio.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       Updated and copied to bOS
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include <stddef.h>
#include <stdint.h>


/* void gpio_funcsel(const uint32_t offset, const uint32_t bitmap, const uint32_t altfcn)
 *
 * in:          offset:     The register "GPIO Function Select X" offset from the GPIO base address
 *              bitmap:     Bitmap triplet marking the GPIOx in the register "GPIO Function Select X"
 *              altfcn:     GPIO pins alternative function assignment, position as in "GPIO Function Select X"
 *
 * out:         none
 *
 * description: Set the GPIO alternative functions. [ref 3] Each of the 54 GPIO pins has at least two alternative 
 *              functions as defined in section 16.2. The FSEL{n} field determines the functionality of the nth 
 *              GPIO pin. See "Table 6-31 GPIO Pins Alternative Function Assignment":
 *
 *              000 = GPIO Pin is an input
 *              001 = GPIO Pin is an output
 *              100 = GPIO Pin takes alternate function 0
 *              101 = GPIO Pin takes alternate function 1
 *              110 = GPIO Pin takes alternate function 2
 *              111 = GPIO Pin takes alternate function 3
 *              011 = GPIO Pin takes alternate function 4
 *              010 = GPIO Pin takes alternate function 5
 *
 * depend:      None
 *
 * note:        None
 */
void gpio_funcsel(const uint32_t offset, const uint32_t bitmap, const uint32_t altfcn);


/* int32_t gpio_output_high(uint32_t gpio)
 * int32_t gpio_output_low(uint32_t gpio)
 *
 * in:          gpio:       The GPIO pin number
 *
 * out:         GPIO pin number on success and negative on error
 *
 * description: GPIO pins configured as output can be set to high respective low voltage.
 *
 * depend:      None
 *
 * note:        None
 */
int32_t gpio_output_high(uint32_t gpio);
int32_t gpio_output_low(uint32_t gpio);


/* void gpio_setup_uart(void)
 *
 * in:          None
 *
 * out:         None
 *
 * description: Set up GPIO pin 14 and 15 used by UART for communication.
 *
 * depend:      None
 *
 * note:        None
 */
void gpio_setup_uart(void);


#endif /* GPIO_H_ */