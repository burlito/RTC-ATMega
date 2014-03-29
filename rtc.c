/**
  The MIT License (MIT)

  Copyright (c) 2014 Andrej Manduch, Adrián Matejov, XLC Team

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <util/atomic.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "rtc.h"

#ifdef RTC_32_BIT
uint16_t high_bites;
#endif /* RTC_32_BIT */

void rtc_init(void)
{
#if RTC_PRESCALER == 1
	TCCR1B |= _BV(CS10);
#elif RTC_PRESCALER == 8
	TCCR1B |= _BV(CS11);
#elif RTC_PRESCALER == 64
	TCCR1B |= _BV(CS10) | _BV(CS11);
#elif RTC_PRESCALER == 256
	TCCR1B |= _BV(CS12);
#elif RTC_PRESCALER == 1024
	TCCR1B |= _BV(CS12) | _BV(CS10);
#else /* default is 1024 div prescaler */
#	define RTC_PRESCALER 1024
	TCCR1B |= _BV(CS12) | _BV(CS10);
#endif

#ifdef RTC_32_BIT
	TIMSK1 |= _BV(TOIE1);
	high_bites = 0;
#endif /* RTC_32_BIT */

	/*Timer 1 can be accessed from TCNT1H and TCNT1L */
}

#ifdef RTC_32_BIT
ISR(TIMER1_OVF_vect) {
	high_bites++;
}
#endif /* RTC_32_BIT */

uint16_t get_rtc(void)
{
	uint16_t tmp;
	uint8_t i;

	/* attem to read without blocking */
	for (i = 0; i < 4; i++) {
		((uint8_t *)&tmp)[1] = TCNT1H;
		((uint8_t *)&tmp)[0] = TCNT1L;
		if ((uint8_t)((&tmp)[1]) == TCNT1H)
			return tmp;
	}

	/* when non-block reading was not possible: */
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		tmp = TCNT1;
	}

	return tmp;
}

#ifdef RTC_32_BIT
/* if it's  at least remotelly posible, use get_rtc instead */
uint32_t get_full_rtc(void)
{
	uint32_t ret;
	uint16_t tmp;
	uint8_t i;

	for (i = 0; i < 4; i++) {
		ret = high_bites;
		((uint16_t *)&ret)[1] = high_bites;
		tmp = get_rtc();
		((uint8_t *)&ret)[0] = (uint8_t)tmp;
		((uint8_t *)&ret)[1] = ((uint8_t *)&tmp)[1];
		if (((uint16_t *)&ret)[1] == high_bites)
			return ret;
	}

	return 1;
	/* hopefully this will not be needed */
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		((uint16_t *)&ret)[1] = high_bites;
		tmp = get_rtc();
		((uint8_t *)&ret)[0] = (uint8_t)tmp;
		((uint8_t *)&ret)[1] = ((uint8_t *)&tmp)[1];
	}

	return ret;
}
#endif /* RTC_32_BIT */

#ifdef RTC_MS_RESOLUTION
/** \todo need to test for marginal vaues like ~(uint32_t)0  */
#ifdef RTC_32_BIT
uint32_t ticks2ms(uint32_t ticks)
{
	uint64_t tmp = ticks;
#else /* RTC_32_BIT */
uint16_t ticks2ms(uint16_t ticks)
{
	uint32_t tmp = ticks;
#endif /* RTC_32_BIT */

#if (F_CPU == 8000000UL) || (F_CPU == 4000000UL) || (F_CPU == 2000000UL) ||    \
		(F_CPU == 1000000UL)
#	define QUOT (RTC_PRESCALER/(F_CPU/1000000UL))
	tmp *= QUOT;
	tmp /= 1000;
#ifdef RTC_32_BIT
	return (uint32_t) tmp;
#else /* RTC_32_BIT */
	return (uint16_t) tmp;
#endif /* RTC_32_BIT */

#	undef QUOT
#else
	tmp *= 1000;
	tmp *= RTC_PRESCALER;
#ifdef RTC_32_BIT
	return (uint32_t)(tmp/F_CPU);
#else /* RTC_32_BIT */
	return (uint16_0t)(tmp/F_CPU);
#endif /* RTC_32_BIT */
#endif /* F_CPU type */
}

#ifdef RTC_32_BIT
uint32_t get_rtc_ms(void)
{
	return ticks2ms(get_full_rtc());
}
#else /* RTC_32_BIT */
uint16_t get_rtc_ms(void)
{
	return ticks2ms(get_rtc());
}
#endif /* RTC_32_BIT */
#endif /* RTC_MS_RESOLUTION */

#ifdef RTC_US_RESOLUTION
#ifdef RTC_32_BIT
uint32_t ticks2us(uint32_t ticks)
{
	uint64_t tmp = ticks;
#else /* RTC_32_BIT */
uint16_t ticks2us(uint16_t ticks)
{
	uint32_t tmp = ticks;
#endif /* RTC_32_BIT */

#if (F_CPU == 8000000UL) || (F_CPU == 4000000UL) || (F_CPU == 2000000UL) ||    \
		(F_CPU == 1000000UL)
#	define QUOT (RTC_PRESCALER/(F_CPU/1000000UL))
	tmp *= QUOT;
	return (uint32_t) tmp;
#	undef QUOT
#else
	tmp *= 1000;
	tmp *= RTC_PRESCALER;

#ifdef RTC_32_BIT
	return (uint32_t)(tmp/(F_CPU/1000));
#else /* RTC_32_BIT */
	return (uint32_t)(tmp/(F_CPU/1000));
#endif /* RTC_32_BIT */
#endif /* F_CPU type */
}

#ifdef RTC_32_BIT
uint32_t get_rtc_us(void)
{
	ticks2us(get_full_rtc());
}
#else /* RTC_32_BIT */
uint16_t get_rtc_us(void)
{
	ticks2us(get_rtc());
}
#endif /* RTC_32_BIT */
#endif /* RTC_US_RESOLUTION */
