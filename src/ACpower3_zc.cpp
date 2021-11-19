/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#include <Arduino.h>
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

//volatile uint16_t ACpower3::Angle;
volatile uint8_t ACpower3::_phase;
volatile bool ACpower3::_adcAlign;

volatile uint16_t ACpower3::A[3];		
volatile uint32_t ACpower3::CounterZC[3];
volatile uint32_t ACpower3::CounterZC_raw[3];
volatile uint32_t ACpower3::_zcCounter[3];
volatile uint32_t ACpower3::_zcMillis[3];


void ACpower3::setup_ZeroCross(uint8_t i)
{
	log_cfg(" . ", i);
	log_cfg(" ZCross on pin ", _pinZC[i]);
	
	_zcMillis[i] = millis();
	pinMode(_pinZC[i], INPUT_PULLUP);
	
	if 		(i==0) attachInterrupt(digitalPinToInterrupt(_pinZC[i]), ZeroCross_int0, _zcEdge);
	else if (i==1) attachInterrupt(digitalPinToInterrupt(_pinZC[i]), ZeroCross_int1, _zcEdge);
	else if (i==2) attachInterrupt(digitalPinToInterrupt(_pinZC[i]), ZeroCross_int2, _zcEdge);

	log_cfg_ln(" - OK");

	return;
}


void IRAM_ATTR ACpower3::ZeroCross_int0() //__attribute__((always_inline))
{
	const uint8_t i = 0;
	CounterZC_raw[i]++;
	if ((millis() - _zcMillis[i]) > 5)
	{
		timerStop(timerTriac[i]);
		digitalWrite(_pinTR[i], LOW);
		_zcMillis[i] = millis();
		CounterZC[i]++;
		_zcCounter[i]++;
		
		timerWrite(timerTriac[i], A[i]);
		timerStart(timerTriac[i]);
		
		if (_adcAlign && (_phase == i) && (_adcCounter == ACPOWER3_ADC_NEXT))
		{
			portENTER_CRITICAL_ISR(&muxADC);
			_adcCounter = ACPOWER3_ADC_START;
			portEXIT_CRITICAL_ISR(&muxADC);
		} 
	}
	return;
}

void IRAM_ATTR ACpower3::ZeroCross_int1() //__attribute__((always_inline))
{
	const uint8_t i = 1;
	CounterZC_raw[i]++;
	if ((millis() - _zcMillis[i]) > 5)
	{
		timerStop(timerTriac[i]);
		digitalWrite(_pinTR[i], LOW);
		_zcMillis[i] = millis();
		CounterZC[i]++;
		_zcCounter[i]++;
		
		timerWrite(timerTriac[i], A[i]);
		timerStart(timerTriac[i]);
		
		if (_adcAlign && (_phase == i) && (_adcCounter == ACPOWER3_ADC_NEXT))
		{
			portENTER_CRITICAL_ISR(&muxADC);
			_adcCounter = ACPOWER3_ADC_START;
			portEXIT_CRITICAL_ISR(&muxADC);
		} 
	}
	return;
}

void IRAM_ATTR ACpower3::ZeroCross_int2() //__attribute__((always_inline))
{
	const uint8_t i = 2;
	CounterZC_raw[i]++;
	if ((millis() - _zcMillis[i]) > 5)
	{
		timerStop(timerTriac[i]);
		digitalWrite(_pinTR[i], LOW);
		_zcMillis[i] = millis();
		CounterZC[i]++;
		_zcCounter[i]++;
		
//		timerWrite(timerTriac[i], Angle);
		timerWrite(timerTriac[i], A[i]);
		timerStart(timerTriac[i]);
		
		if (_adcAlign && (_phase == i) && (_adcCounter == ACPOWER3_ADC_NEXT))
		{
			portENTER_CRITICAL_ISR(&muxADC);
			_adcCounter = ACPOWER3_ADC_START;
			portEXIT_CRITICAL_ISR(&muxADC);
		} 
	}
	return;
}


#endif // ESP32
