/*
 * test.c
 *
 * Created: 31.05.2017 10:54:18
 * Author: Sergey
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define DELEYTIME 50
#define POINT 10
#define TIMER1_STOP TCCR1B &= ~(1 << CS12)
#define TIMER1_START TCCR1B |= (1 << CS12)


//----------------------------------------
void timer_init(void);
void segChar (uint8_t seg);
void ledPrint (uint8_t, uint8_t);

//----------------------------------------
volatile uint8_t timerMinutes = 60;
volatile uint8_t timerSeconds = 0;
volatile uint8_t halfsecond   = 0;
uint8_t r1, r2, r3, r4;                 // мм/сс
uint8_t n_count = 0;

//----------------------------------------
void main(void)
{
    uint8_t i;
    uint8_t buttCount = 0;
    uint8_t buttState = 0;

    timer_init();
    // DDRD порты на "ВЫХОД"
    DDRD = 0xFF;
    PORTD = 0b11111111;
    // DDRB порты на "ВХОД"
    DDRB = 0b00001111;
    // Включение подтягивающего резистора на пине B5
    PORTB = 0b00010000;
    // Разрешим прерывания
    sei();

    while (1)
    {
		// Антидребезг
		while (buttState == 0)
		{
			if (!(PINB&0b00010000))
			{
				buttState = 1;
				TIMER1_STOP;
			}

			else
			{
				buttState = 0;
				TIMER1_START;
			}

		}

    }

}

//----------------------------------------

void timer_init (void)
{
    // Устанавливаем режим СТС (сброс таймера по совпадению)
    TCCR1B |= (1 << WGM12);

    // Устанавливаем таймер 0
    TCCR0 = 0b00000001;
    // Устанавливаем делитель таймера 0 в /8
    TCCR0 |= (1 << CS01);
    // Устанавливаем бит разрешения прерывания по переполнению таймера 0
    TIMSK  |= (1 << TOIE0);
    // Устанавливаем бит разрешения прерывания 1ого счетчика по совпадению с OCR1A(H и L)
    TIMSK  |= (1 << OCIE1A);
    // Записываем в регистр число для сравнения 111101000010010 (31250) (секунда) // !!!11110100001001 (15625) (Пол секунды)
    OCR1AH = 0b00111101;
    OCR1AL = 0b00001001;
    // Установим делитель /256
    //TCCR1B |= (1 << CS12);
    //TCCR1B |= (1 << CS10);
    //TCCR1B |= (1 << CS11);
}

//----------------------------------------
/* Считаем время */
ISR(TIMER1_COMPA_vect)
{
    ledPrint(timerMinutes, timerSeconds);

    ++halfsecond;

    if (halfsecond == 2)
    {
        if (timerSeconds == 0)
        {
            --timerMinutes;
            timerSeconds = 60;
        }

        if (timerMinutes == 0)
            timerMinutes = 60;

        else
            --timerSeconds;

        halfsecond = 0;
    }
}

//----------------------------------------
/* Динамическая индикация дисплея */
ISR(TIMER0_OVF_vect)
{
    if (n_count == 0)
    {
        PORTB = 0b00000001;
        segChar(r1);
    }

    else if (n_count == 1)
    {
        PORTB = 0b00000010;
        segChar(r2);

        // Если сейчас пол секунды мигаем точкой
        if (halfsecond == 1)
            segChar(POINT);
    }

    else if (n_count == 2)
    {
        PORTB = 0b00000100;
        segChar(r3);
    }

    else if (n_count == 3)
    {
        PORTB = 0b00001000;
        segChar(r4);
    }

    n_count++;

    if (n_count > 3)
        n_count = 0;
}
//----------------------------------------
/* Вывод символов на 7seg дисплей */
void segChar (uint8_t seg)
{
    switch (seg) {
        case 1:
            PORTD = ~0b00000110;    // "1"
            break;
        case 2:
            PORTD = ~0b01011011;    // "2"
            break;
        case 3:
            PORTD = ~0b01001111;    // "3"
            break;
        case 4:
            PORTD = ~0b01100110;    // "4"
            break;
        case 5:
            PORTD = ~0b01101101;    // "5"
            break;
        case 6:
            PORTD = ~0b01111101;    // "6"
            break;
        case 7:
            PORTD = ~0b00000111;    // "7"
            break;
        case 8:
            PORTD = ~0b01111111;    // "8"
            break;
        case 9:
            PORTD = ~0b01101111;    // "9"
            break;
        case 0:
            PORTD = ~0b00111111;    // "0"
            break;
        case 10:
            PORTD &= ~(1 << PIN7);  // POINT "."
            break;
        };
}

//----------------------------------------
/* Разделение минут и секунд на разряды */
void ledPrint (uint8_t minutes, uint8_t seconds)
{
    r1 = minutes / 10;
    r2 = minutes % 10;
    r3 = seconds / 10;
    r4 = seconds % 10;
}
