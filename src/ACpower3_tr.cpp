/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#include <Arduino.h>
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

hw_timer_t *ACpower3::timerTriac[3] = { NULL, NULL, NULL };

volatile uint32_t ACpower3::CounterTR[3];
uint8_t ACpower3::_pinTR[3];


void ACpower3::setup_Triac(uint8_t i)
{
	log_cfg(" . ", i);
	log_cfg(" TRIAC on pin ", _pinTR[i]);

	Angle = 0;
	pinMode(_pinTR[i], OUTPUT);
	digitalWrite(_pinTR[i], LOW);
	timerTriac[i] = timerBegin(i, 80, true);
	
	if 		(i==0) timerAttachInterrupt(timerTriac[i], &OpenTriac_int0, true);
	else if (i==1) timerAttachInterrupt(timerTriac[i], &OpenTriac_int1, true);
	else if (i==2) timerAttachInterrupt(timerTriac[i], &OpenTriac_int2, true);
	
	timerAlarmWrite(timerTriac[i], (ACPOWER3_ANGLE_MAX + ACPOWER3_ANGLE_DELTA), true);
	timerAlarmEnable(timerTriac[i]);
	timerWrite(timerTriac[i], Angle);
	log_cfg_ln(" - OK");
	
	return;
}

	
void IRAM_ATTR ACpower3::OpenTriac_int0() //__attribute__((always_inline))
{
	const uint8_t i = 0;
	uint64_t _tmrTriacNow = timerRead(timerTriac[i]);
	if ((_tmrTriacNow > ACPOWER3_ANGLE_MIN) && (_tmrTriacNow < ACPOWER3_ANGLE_MAX))
	{
		digitalWrite(_pinTR[i], HIGH);
		D(CounterTRopen++);
		D(TimerTRopen = _tmrTriacNow);
	}
	else
	{
		digitalWrite(_pinTR[i], LOW);
		D(CounterTRclose++);
		D(TimerTRclose = _tmrTriacNow);
	}
	timerStop(timerTriac[i]);
	CounterTR[i]++;
	D(TRIACcore = xPortGetCoreID());
	D(TRIACprio = uxTaskPriorityGet(NULL));
}

void IRAM_ATTR ACpower3::OpenTriac_int1() //__attribute__((always_inline))
{
	const uint8_t i = 1;
	uint64_t _tmrTriacNow = timerRead(timerTriac[i]);
	if ((_tmrTriacNow > ACPOWER3_ANGLE_MIN) && (_tmrTriacNow < ACPOWER3_ANGLE_MAX))
	{
		digitalWrite(_pinTR[i], HIGH);
		D(CounterTRopen++);
		D(TimerTRopen = _tmrTriacNow);
	}
	else
	{
		digitalWrite(_pinTR[i], LOW);
		D(CounterTRclose++);
		D(TimerTRclose = _tmrTriacNow);
	}
	timerStop(timerTriac[i]);
	CounterTR[i]++;
	D(TRIACcore = xPortGetCoreID());
	D(TRIACprio = uxTaskPriorityGet(NULL));
}

void IRAM_ATTR ACpower3::OpenTriac_int2() //__attribute__((always_inline))
{
	const uint8_t i = 2;
	uint64_t _tmrTriacNow = timerRead(timerTriac[i]);
	if ((_tmrTriacNow > ACPOWER3_ANGLE_MIN) && (_tmrTriacNow < ACPOWER3_ANGLE_MAX))
	{
		digitalWrite(_pinTR[i], HIGH);
		D(CounterTRopen++);
		D(TimerTRopen = _tmrTriacNow);
	}
	else
	{
		digitalWrite(_pinTR[i], LOW);
		D(CounterTRclose++);
		D(TimerTRclose = _tmrTriacNow);
	}
	timerStop(timerTriac[i]);
	CounterTR[i]++;
	D(TRIACcore = xPortGetCoreID());
	D(TRIACprio = uxTaskPriorityGet(NULL));
}


#endif // ESP32
