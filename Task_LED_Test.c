/*--Task_Blinky.c
 *
 *  Author:			Gary J. Minden
 *	Organization:	KU/EECS/EECS 388
 *  Date:			February 22, 2016
 *
 *  Description:	Blinks LED D1 on Tiva TMC41294 Evaluation board
 *
 */

#include	"inc/hw_ints.h"
#include	"inc/hw_memmap.h"
#include	"inc/hw_types.h"
#include	"inc/hw_uart.h"

#include	<stddef.h>
#include	<stdbool.h>
#include	<stdint.h>
#include	<stdarg.h>

#include	"driverlib/sysctl.h"
#include	"driverlib/pin_map.h"
#include	"driverlib/gpio.h"

#include	"FreeRTOS.h"
#include	"task.h"

#include    "Drivers/EECS388_LED.h"

int16_t LED_Data = 0x0001;

int32_t getButtonState() {
    return GPIOPinRead( GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1 ) ^ 0x03;
}

void playPattern0() {
    LED_Data = 0x0111;
    EECS388_SetLEDs( LED_Data );
    while( getButtonState() == 0x00 ) {
        EECS388_SetLEDs( LED_Data );
        LED_Data = LED_Data << 1;
        if( LED_Data & 0x0400 ) {
            LED_Data += 1;
        }

        vTaskDelay( ( 100 * configTICK_RATE_HZ ) / 1000 );
    }
}

void playPattern1() {
    LED_Data = 0x0001;
    int i;
    while( getButtonState() == 0x01 ) {
        for( i = 0; i < 10; i++ ) {
            LED_Data = LED_Data << 1;
            EECS388_SetLEDs( LED_Data );
            vTaskDelay( ( 25 * configTICK_RATE_HZ ) / 1000 );
        }
        for( i = 0; i < 10; i++ ) {
            LED_Data = LED_Data >> 1;
            EECS388_SetLEDs( LED_Data );
            vTaskDelay( ( 25 * configTICK_RATE_HZ ) / 1000 );
        }
    }
}

void playPattern2() {
    LED_Data = 0x0155;
    while( getButtonState() == 0x02 ) {
        LED_Data ^= 0x03ff;
        EECS388_SetLEDs( LED_Data );
        vTaskDelay( ( 100 * configTICK_RATE_HZ ) / 1000 );
    }
}

void playPattern3() {
    LED_Data = 0x0001;
    while( getButtonState() == 0x03 ) {
        while( LED_Data < 0x03ff ) {
            LED_Data |= LED_Data << 1;
            EECS388_SetLEDs( LED_Data );
            vTaskDelay( ( 100 * configTICK_RATE_HZ) / 1000 );
        }
        while( LED_Data > 0x0001 ) {
            LED_Data = LED_Data >> 1;
            EECS388_SetLEDs( LED_Data );
            vTaskDelay( ( 100 * configTICK_RATE_HZ) / 1000 );
        }
    }
}


extern void Task_LED_Test( void *pvParameters ) {

    //
    // Enable the GPIO Port J.
    //
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOJ );

    //
    // Configure GPIO J to read button state.
    //
    GPIOPinTypeGPIOInput( GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
    GPIOPadConfigSet( GPIO_PORTJ_BASE,
                      GPIO_PIN_0 | GPIO_PIN_1,
                      GPIO_STRENGTH_2MA,
                      GPIO_PIN_TYPE_STD_WPU );

	EECS388_LED_Initialization();

	while( 1 ) {
	    switch( getButtonState() ) {
	    case 0x00:
            vTaskDelay( ( 10 * configTICK_RATE_HZ) / 1000 );
	        if( getButtonState() == 0x00 ) { playPattern0(); };
	        break;
        case 0x01:
            vTaskDelay( ( 10 * configTICK_RATE_HZ) / 1000 );
            if( getButtonState() == 0x01 ) { playPattern1(); }
            break;
        case 0x02:
            vTaskDelay( ( 10 * configTICK_RATE_HZ) / 1000 );
            if( getButtonState() == 0x02 ) { playPattern2(); }
            break;
        case 0x03:
            vTaskDelay( ( 10 * configTICK_RATE_HZ) / 1000 );
            if( getButtonState() == 0x03 ) { playPattern3(); }
            break;
        default:
            break;
	    }
	}
}
