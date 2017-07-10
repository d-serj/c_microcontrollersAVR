/*
  A bad example
  $Rev: 1003 $
*/

#include <avr/io.h>

// Connect a button from ground to pin 0 on PORTA
#define BUTTON_MASK (1<<PA0)
#define BUTTON_PIN  PINA
#define BUTTON_PORT PORTA

// Connect a LED from ground via a resistor to pin 0 on PORTB
#define LED_MASK (1<<PB0)
#define LED_PORT PORTB
#define LED_DDR  DDRB

int main(void)
{
    // Enable internal pullup resistor on the input pin
    BUTTON_PORT |= BUTTON_MASK;

    // Set to output
    LED_DDR |= LED_MASK;

    while(1)
    {
        // Check if the button is pressed. Button is active low
        // so we invert PINB with ~ for positive logic
        if (~BUTTON_PIN & BUTTON_MASK)
        {
            // Toggle the LED
            LED_PORT ^= LED_MASK;
        }
    }
}
