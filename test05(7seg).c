/*
 * test.c
 *
 * Created: 31.05.2017 10:54:18
 * Author: Sergey
 */

#include <io.h>
#include <interrupt.h>

#define F_CPU 8000000L
#define DELEYTIME 50

//----------------------------------------
void segChar (int seg);
void ledPrint (unsigned int number);

//----------------------------------------
volatile unsigned int timerVar = 60;
unsigned int r1, r2;
unsigned short n_count = 0;

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
    // Записываем в регистр число для сравнения (31250)
    OCR1AH = 0b01111010;
    OCR1AL = 0b00010010;
    // Установим делитель /256
    TCCR1B |= (1 << CS12);
    //TCCR1B |= (1 << CS10);
    //TCCR1B |= (1 << CS11);
}

//----------------------------------------
interrupt [TIM1_COMPA] void isr_TIM1_COMPA (void)
{
    ledPrint(timerVar);

    if (timerVar == 0)
        timerVar = 60;

    else
        --timerVar;
}

//----------------------------------------
interrupt [TIM0_OVF] void tim0_interrupt (void)
{
    if (n_count == 0)
    {
        PORTB &= ~(1 << PORTB0);
        PORTB |= (1 << PORTB1);
        segChar(r1);
    }

    if (n_count == 1)
    {
        PORTB &= ~(1 << PORTB1);
        PORTB |= (1 << PORTB0);

        segChar(r2);
    }

    n_count++;

    if (n_count > 1)
        n_count = 0;
}

//----------------------------------------
void main(void)
{
//    int i;
//    unsigned int buttCount = 0;
//    unsigned int buttState = 0;

    timer_init();
    // DDRD порты на "ВЫХОД"
    DDRD = 0xFF;
    // DDRB порты на "ВХОД"
    DDRB = 0b00001111;
    PORTD = 0b01111111;
    // Включение подтягивающего резистора
    PORTB = 0b00100000;
    // Разрешим прерывания
    sei();

    while (1)
    {
//        for (i = 0; i <=9 ; ++i)
//        {
//            while (buttState == 0)
//			{
//                // Антидребезг
//                if (!(PINB&0b00000001))
//                {
//                    if (buttCount < 10)
//                    {
//                        ++buttCount;
//                    }
//                    else
//                    {
//                        i = 0;
//                        buttState = 0;
//                    }
//				}
//
//                else
//                {
//                    if (buttCount > 0)
//                        --buttCount;
//                    else
//                        buttState = 1;
//                }
//            }
//
//            segChar(i);
//            delay_ms(50);
//            buttState = 0;
//        }
    }
}

//----------------------------------------
/* Вывод символов на 7seg дисплей */
void segChar (int seg)
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
        };
}

//----------------------------------------
void ledPrint (unsigned int number)
{
    r1 = number % 10;
    r2 = number / 10;
}
