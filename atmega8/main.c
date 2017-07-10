/*
 * test.c
 *
 * Created: 31.05.2017 10:54:18
 * Author: Sergey
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define POINT       10
#define TRUE        1
#define FALSE       0
#define BUTT_START  1
#define BUTT_RESET  2
#define BUTT_RISE   3
/* �������� �� 256 */
#define TIMER1_STOP  TCCR1B &= ~(1 << CS12)
#define TIMER1_START TCCR1B |= (1 << CS12)
/* ---------------- */

//----------------------------------------
void timer_init(void);
void segChar (uint8_t seg);
void ledPrint (uint8_t, uint8_t);
void readButtons (void);

//----------------------------------------
volatile uint8_t timerMinutes = 60;
volatile uint8_t timerSeconds = 0;
volatile uint8_t halfsecond   = 0;
uint8_t r1, r2, r3, r4;                 // ��/��
volatile uint8_t n_count = 0;           // ������� � ������������ ���������
volatile uint8_t update;
volatile uint8_t showPoint;

//----------------------------------------
void main(void)
{
	uint8_t buttState  = 0;
	uint8_t buttPress;
	uint8_t isStart    = TRUE;
	uint8_t firstStart = TRUE;

	BUT_Init();
	timer_init();
	// DDRD ����� �� "�����"
	DDRD = 0xFF;
	// DDRB ����� �� "�����" 0, 1, 2, 3. ����� �� "����" 4, 5, 6, 7.
	DDRB = 0b00001111;
	PORTD = 0b11111111;
	// ��������� �������������� ��������� �� ����� B5 � B6
	PORTB = 0b00110000;
	// �������� ����������
	sei();

	while (1)
	{
        if (update == TRUE)
            ledPrint(timerMinutes, timerSeconds);

        BUT_Poll();
        buttPress = BUT_GetBut();

        if (buttPress == BUTT_START)
        {
            buttState = BUT_GetBut();
            if (buttState == 1 && !isStart)
            {
                TIMER1_STOP;
                isStart = !isStart;
            }

            else if (buttState == 1 && isStart)
            {
                if (firstStart)
                {
                    timerMinutes = 60;
                    timerSeconds = 0;
                    firstStart = FALSE;
                }

                TIMER1_START;
                isStart = !isStart;
                showPoint = FALSE;
            }
        }


        if (buttPress == BUTT_RESET)
        {
            //buttState = BUT_GetBut();
            if (buttState == 1)
            {
                TIMER1_STOP;
                timerMinutes = 0;
                timerSeconds = 0;
                ledPrint(timerMinutes, timerSeconds);
                isStart = TRUE;
                firstStart = TRUE;
                showPoint = TRUE;
            }

        }
	}
}

//----------------------------------------

void timer_init (void)
{
	// ������������� ����� ��� (����� ������� �� ����������)
	TCCR1B |= (1 << WGM12);
	// ������������� ������ 0
	TCCR0 = 0b00000001;
	// ������������� �������� ������� 0 � /8
	TCCR0 |= (1 << CS01);
	// ������������� ��� ���������� ���������� �� ������������ ������� 0
	TIMSK |= (1 << TOIE0);
	// ������������� ��� ���������� ���������� 1��� �������� �� ���������� � OCR1A(H � L)
	TIMSK  |= (1 << OCIE1A);
	// ���������� � ������� ����� ��� ��������� 111101000010010 (31250) (�������) // !!!11110100001001 (15625) (��� �������)
	OCR1AH = 0b00111101;
	OCR1AL = 0b00001001;
	// ��������� �������� /256
	//TCCR1B |= (1 << CS12);
	//TCCR1B |= (1 << CS10);
	//TCCR1B |= (1 << CS11);
}

//----------------------------------------
/* ������� ����� */
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

		if (timerMinutes == 0)
			timerMinutes = 60;

		else
			--timerSeconds;

		halfsecond = 0;
	}

	update = TRUE;
}

//----------------------------------------
/* ������������ ��������� ������� */
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

		// ���� ������ ��� ������� ������ ������
		if (halfsecond == 1 || showPoint)
        {
            segChar(POINT);
        }
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
/* ����� �������� �� 7seg ������� */
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
}

//----------------------------------------
/* ���������� ����� � ������ �� ������� */
void ledPrint (uint8_t minutes, uint8_t seconds)
{
    //cli();
	r1 = minutes / 10;
	r2 = minutes % 10;
	r3 = seconds / 10;
	r4 = seconds % 10;

	update = FALSE;
	//sei();
}


//----------------------------------------
/* ������� ������ ��������� ������ */
void readButtons(void)
{

}

