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

#ifndef _RTC_CONFIG_H
#define _RTC_CONFIG_H

/*
	this is place for define or include
	definition of F_CPU and RTC_PRESCALER 
*/
/**
	F_CPU is used for computinf purporses

	RTC_PRESCALER will setup RTC prescaling int TC1
		allowed values are 1, 64, 256, 1024

	Optimization macros:
		RTC_MS_RESOLUTION if defined library will be
			compiled with milissecond support

		RTC_US_RESOLUTION if defined library will be
			compiled with microsecond support

		RTC_32_BIT if defined library will be compiled
			with 32 bit timer resolution support
			this will add about 4K (4/5 of all
			library) flash space.

		in case you're using RTC_US_RESOLUTION it's
		highli recommended use RTC_32_BIT too
*/

#define RTC_MS_RESOLUTION
#define RTC_US_RESOLUTION
#define RTC_32_BIT

#ifndef F_CPU
#	define F_CPU 8000000UL
#endif
#ifndef RTC_PRESCALER
#	define RTC_PRESCALER 1024
#endif

#endif /* _RTC_CONFIG_H */
