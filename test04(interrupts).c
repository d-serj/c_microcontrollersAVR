/*
 * test.c
 *
 * Created: 31.05.2017 10:54:18
 * Author: Sergey
 */

#include <io.h>
#include <delay.h>
#include <interrupt.h>

#define F_CPU 8000000L
#define DELEYTIME 50

//----------------------------------------

void segChar (int seg);
//----------------------------------------

volatile int timerVar = 0;

//----------------------------------------
void timer_init (void)
{
    // Устанавливаем режим СТС (сброс таймера по совпадению)
    TCCR1B |= (1 << WGM12);
    // Устанавливаем бит разрешения прерывания 1ого счетчика по совпадению с OCR1A(H и L)
    TIMSK  |= (1 << OCIE1A);
    // Записываем в регистр число для сравнения (31250)
    OCR1AH = 0b01111010;
    OCR1AL = 0b00010010;
    // Установим делитель
    TCCR1B |= (1 << CS12);
    //TCCR1B |= (1 << CS10);
    //TCCR1B |= (1 << CS11);
}
//----------------------------------------
interrupt [TIM1_COMPA] void isr_TIM1_COMPA (void)
{
    if (timerVar > 9)
        timerVar = 0;

    segChar(timerVar);
    ++timerVar;
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
    DDRB = 0x00;
    PORTD = 0b11111111;
    // Включение подтягивающего резистора
    PORTB = 0b00000001;
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

/* Вывод символов на дисплей */
void segChar (int seg)
{
    switch (seg) {
        case 1:
            PORTD = ~0b00000110;
            break;
        case 2:
            PORTD = ~0b01011011;
            break;
        case 3:
            PORTD = ~0b01001111;
            break;
        case 4:
            PORTD = ~0b01100110;
            break;
        case 5:
            PORTD = ~0b01101101;
            break;
        case 6:
            PORTD = ~0b01111101;
            break;
        case 7:
            PORTD = ~0b00000111;
            break;
        case 8:
            PORTD = ~0b01111111;
            break;
        case 9:
            PORTD = ~0b01101111;
            break;
        case 0:
            PORTD = ~0b00111111;
            break;
        };
}
//----------------------------------------
