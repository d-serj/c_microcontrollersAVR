/*
 * test.c
 *
 * Created: 31.05.2017 10:54:18
 * Author: Sergey
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define POINT       10
#define TRUE        1
#define FALSE       0
#define BUTT_START  1
#define BUTT_RESET  2
#define BUTT_RISE   3
/* Делитель на 256 */
#define TIMER1_STOP  TCCR1B &= ~(1 << CS12)
#define TIMER1_START TCCR1B |= (0 << CS10) | (0 << CS11) | (1 << CS12)
/* ---------------- */

//----------------------------------------
void timer_init(void);
void segChar (uint8_t seg);
void divIntoCategories (uint8_t, uint8_t);
void readButtons (void);

//----------------------------------------
                             //0   1    2    3    4    5    6    7    8    9
const uint8_t symbols[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
volatile uint8_t timerMinutes = 60;
volatile uint8_t timerSeconds = 0;
volatile uint8_t halfsecond   = 0;
uint8_t timeUnits[4];                   // мм/сс
volatile uint8_t n_count = 0;           // счетчик в динамическую индикацию
volatile uint8_t update = TRUE;
volatile uint8_t showPoint = TRUE;

//----------------------------------------
void main(void)
{
	uint8_t buttState  = 0;
	uint8_t buttPress;
	uint8_t isStart    = TRUE;
	uint8_t firstStart = TRUE;

	BUT_Init();
	timer_init();
	// DDRD порты на "ВЫХОД"
	DDRD = 0xFF;
	// DDRB порты на "ВЫХОД" 0, 1, 2, 3. Порты на "ВХОД" 4, 5, 6, 7.
	DDRB = 0b00001111;
	PORTD = 0b11111111;
	// Включение подтягивающего резистора на пинах B5 и B6
	PORTB = 0b00110000;
	// Разрешим прерывания
	sei();

	while (1)
	{
        if (update == TRUE)
            divIntoCategories(timerMinutes, timerSeconds);

        // Остановка если закончилось время
        if (timerMinutes == 0 && timerSeconds == 0)
        {
            TIMER1_STOP;
            showPoint = TRUE;
        }

        // Обновляем состояние кнопок
        BUT_Poll();
        buttPress = BUT_GetBut();

        // Если нажат "СТАРТ"
        if (buttPress == BUTT_START)
        {
            buttState = BUT_GetBut();
            // СТОП есди флаг старта "0"
            if (buttState == 1 && !isStart)
            {
                TIMER1_STOP;
                isStart = !isStart;
            }

            // СТАРТ если флаг старта "1"
            else if (buttState == 1 && isStart)
            {
                // Если это первый старт, то установим время на 60:00
                if (firstStart)
                {
                    timerMinutes = 60;
                    timerSeconds = 0;
                    firstStart = FALSE;
                }

                // Стартуем таймер, инвертируем флаг старта в 0, выключаем "."
                TIMER1_START;
                isStart = !isStart;
                showPoint = FALSE;
            }
        }


        if (buttPress == BUTT_RESET)
        {
            buttState = BUT_GetBut();
            if (buttState == 1)
            {
                TIMER1_STOP;
                timerMinutes = 0;
                timerSeconds = 0;
                divIntoCategories(timerMinutes, timerSeconds);
                isStart =    TRUE;
                firstStart = TRUE;
                showPoint =  TRUE;
            }

        }

        if (buttPress == BUTT_RISE)
        {
            buttState = BUT_GetBut();
            if (buttState == 1)
            {
                firstStart = FALSE;
                ++timerMinutes;
                divIntoCategories(timerMinutes, timerSeconds);
            }

        }
	}
}


//----------------------------------------
void timer_init (void)
{
	// Устанавливаем режим СТС (сброс таймера по совпадению)
	TCCR1B |= (1 << WGM12);
	// Устанавливаем бит разрешения прерывания по переполнению таймера 0
	TIMSK |= (1 << TOIE0);
	// Устанавливаем таймер 0 делитель /8
	TCCR0 = (0 << CS00) | (1<<CS01) | (0 << CS02);
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
    ++halfsecond;

	if (halfsecond == 2)
	{
		if (timerSeconds == 0)
		{
			--timerMinutes;
			timerSeconds = 60;
		}

		--timerSeconds;
		halfsecond = 0;
		showPoint  = FALSE;
	}

	else if (halfsecond == 1)
        showPoint = TRUE;

	update    = TRUE;
}


//----------------------------------------
/* Динамическая индикация дисплея */
ISR(TIMER0_OVF_vect)
{
	PORTB = 0x00;
    PORTB |= _BV(n_count);
    PORTD = ~timeUnits[n_count];
	n_count;

	if (++n_count > 3)
        n_count = 0;

    // Если разряд на 2й цифре и сейчас пол секунды или флаг показать точку
    if (n_count == 2 && showPoint)
        PORTD &= ~(1 << PIN7);  // POINT "."
}
/*	if (n_count == 0)
	{
		PORTB = 0b00000001;
		segChar(timeUnits[0]);
	}

	else if (n_count == 1)
	{
		PORTB = 0b00000010;
		segChar(timeUnits[1]);

		// Если сейчас пол секунды мигаем точкой
		if (halfsecond == 1 || showPoint)
        {
            segChar(POINT);
        }
	}

	else if (n_count == 2)
	{
		PORTB = 0b00000100;
		segChar(timeUnits[2]);
	}

	else if (n_count == 3)
	{
		PORTB = 0b00001000;
		segChar(timeUnits[3]);
	}

	n_count++;

	if (n_count > 3)
		n_count = 0;*/


/*
 Вывод символов на 7seg дисплей
void segChar (uint8_t seg)
{
	switch (seg)
	{
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
}*/


//----------------------------------------
/* Разделение минут и секунд на разряды */
void divIntoCategories (uint8_t minutes, uint8_t seconds)
{
    uint8_t temp;

    // Атомарная операция во избежание использования данных переменных в прерываниях
    // во время операций над ними
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        // minutes /10
        temp = minutes / 10;
        timeUnits[0] = symbols[temp];
        // minutes /1
        temp = minutes % 10;
        timeUnits[1] = symbols[temp];
        //seconds /10
        temp = seconds / 10;
        timeUnits[2] = symbols[temp];
        // seconds /1
        temp = seconds % 10;
        timeUnits[3] = symbols[temp];

        update = FALSE;
    }
}
