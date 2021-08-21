/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#include "Arduino.h"
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

//portMUX_TYPE ACpower3::muxADC = portMUX_INITIALIZER_UNLOCKED;
//hw_timer_t *ACpower3::timerTriac = NULL;
//volatile SemaphoreHandle_t ACpower3::smphRMS;

//volatile bool ACpower3::getI = true;
//volatile bool ACpower3::takeADC = false;

//volatile uint16_t* ACpower3::_pAngle;
volatile uint16_t ACpower3::Angle;
//volatile int16_t ACpower3::Xnow;
//volatile uint32_t ACpower3::X2;

//volatile uint8_t ACpower3::_zero = 1;
volatile uint32_t ACpower3::CounterZC[3];
volatile uint32_t ACpower3::CounterZC_raw[3];
volatile uint32_t ACpower3::_ZCcntr[3];
//volatile uint32_t ACpower3::CounterTR;

volatile uint8_t ACpower3::_phase;

/*
volatile uint8_t ACpower3::_pin;
uint8_t ACpower3::_pinI;
uint8_t ACpower3::_pinU;
uint8_t ACpower3::_pinTriac;
//uint8_t ACpower3::_pinZCross;
*/
/*	
volatile uint32_t ACpower3::CounterADC = 1;
volatile uint32_t ACpower3::_Icntr = 1;
volatile uint32_t ACpower3::_Ucntr = 1;

volatile uint64_t ACpower3::_summ = 0;
volatile uint64_t ACpower3::_I2summ = 0;
volatile uint64_t ACpower3::_U2summ = 0;

volatile uint16_t ACpower3::_zerolevel = 0;
*/

/*
uint16_t ACpower3::_Izerolevel = 0;
uint16_t ACpower3::_Uzerolevel = 0;
*/
volatile uint32_t ACpower3::_msZCross[3];
//volatile bool ACpower3::trOpened;

void IRAM_ATTR ACpower3::ZeroCross_int0() //__attribute__((always_inline))
{
	const uint8_t i = 0;
	CounterZC_raw[i]++;
	if ((millis() - _msZCross[i]) > 5)
	{
		timerStop(timerTriac[i]);
		digitalWrite(_pinTriac[i], LOW);
		_msZCross[i] = millis();
		CounterZC[i]++;
		_ZCcntr[i]++;
		
		timerWrite(timerTriac[i], Angle);
		timerStart(timerTriac[i]);
		
#ifdef ACPOWER3_ADC_TUNING
		if (_phase == i) 
		{
			if (CounterADC == ACPOWER3_ADC_NEXT) 
			{ 
				portENTER_CRITICAL_ISR(&muxADC);
				CounterADC = ACPOWER3_ADC_START;
				portEXIT_CRITICAL_ISR(&muxADC);
			}
		} 
#endif
	}
	return;
}

void IRAM_ATTR ACpower3::ZeroCross_int1() //__attribute__((always_inline))
{
	const uint8_t i = 1;
	CounterZC_raw[i]++;
	if ((millis() - _msZCross[i]) > 5)
	{
		timerStop(timerTriac[i]);
		digitalWrite(_pinTriac[i], LOW);
		_msZCross[i] = millis();
		CounterZC[i]++;
		_ZCcntr[i]++;
		
		timerWrite(timerTriac[i], Angle);
		timerStart(timerTriac[i]);
		
#ifdef ACPOWER3_ADC_TUNING
		if (_phase == i) 
		{
			if (CounterADC == ACPOWER3_ADC_NEXT) 
			{ 
				portENTER_CRITICAL_ISR(&muxADC);
				CounterADC = ACPOWER3_ADC_START;
				portEXIT_CRITICAL_ISR(&muxADC);
			}
		}
#endif
	}
	return;
}

void IRAM_ATTR ACpower3::ZeroCross_int2() //__attribute__((always_inline))
{
	const uint8_t i = 2;
	CounterZC_raw[i]++;
	if ((millis() - _msZCross[i]) > 5)
	{
		timerStop(timerTriac[i]);
		digitalWrite(_pinTriac[i], LOW);
		_msZCross[i] = millis();
		CounterZC[i]++;
		_ZCcntr[i]++;
		
		timerWrite(timerTriac[i], Angle);
		timerStart(timerTriac[i]);
		
#ifdef ACPOWER3_ADC_TUNING
		if (_phase == i) 
		{
			if (CounterADC == ACPOWER3_ADC_NEXT) 
			{
				portENTER_CRITICAL_ISR(&muxADC);
				CounterADC = ACPOWER3_ADC_START;
				portEXIT_CRITICAL_ISR(&muxADC);
			}
		}
#endif		
	}
	return;
}


#endif // ESP32
