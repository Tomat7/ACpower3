/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#include "Arduino.h"
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

void ACpower3::setup_Triac(uint8_t i)
{
	if (_ShowLog)
	{
		PRINT(" . ");
		PRINT(i);
		PRINT(" TRIAC on pin ");
		PRINT(_pinTriac[i]);
	}
	Angle = 0;
	pinMode(_pinTriac[i], OUTPUT);
	digitalWrite(_pinTriac[i], LOW);
	timerTriac[i] = timerBegin(i, 80, true);
	
	if (i==0) timerAttachInterrupt(timerTriac[i], &OpenTriac_int0, true);
	else if (i==1) timerAttachInterrupt(timerTriac[i], &OpenTriac_int1, true);
	else if (i==2) timerAttachInterrupt(timerTriac[i], &OpenTriac_int2, true);
	
	timerAlarmWrite(timerTriac[i], (ANGLE_MAX + ANGLE_DELTA), true);
	timerAlarmEnable(timerTriac[i]);
	timerWrite(timerTriac[i], Angle);
	if (_ShowLog) PRINTLN(" - OK");
	return;
}

void ACpower3::setup_ZeroCross(uint8_t i)
{
	if (_ShowLog)
	{
		PRINT(" . ");
		PRINT(i);
		PRINT(" ZCross on pin ");
		PRINT(_pinZCross[i]);
	}
	
	//takeADC = false;
	_msZCmillis[i] = millis();
	pinMode(_pinZCross[i], INPUT_PULLUP);
	
	if (i==0) attachInterrupt(digitalPinToInterrupt(_pinZCross[i]), ZeroCross_int0, ZC_EDGE);
	else if (i==1) attachInterrupt(digitalPinToInterrupt(_pinZCross[i]), ZeroCross_int1, ZC_EDGE);
	else if (i==2) attachInterrupt(digitalPinToInterrupt(_pinZCross[i]), ZeroCross_int2, ZC_EDGE);
	
	if (_ShowLog) PRINTLN(" - OK");
	return;
}

void ACpower3::setup_ADC()
{
	//uint16_t ADCperSet = ADC_RATE * ADC_WAVES;
	uint16_t usADCinterval = (uint16_t)(10000 / ADC_RATE);
	smphRMS = xSemaphoreCreateBinary();
	getI = true;
	_cntr = ADC_COUNT;
	_now = 0;
	_pin = _pinI[_now];
	_zerolevel = _Izerolevel[_now];
	
	timerADC = timerBegin(TIMER_ADC, 80, true);
	timerAttachInterrupt(timerADC, &GetADC_int, true);
	timerAlarmWrite(timerADC, usADCinterval, true);
	timerAlarmEnable(timerADC);
	DELAYx;
	
	if (_ShowLog) 
	{
		PRINTLN(" + ADC Inerrupt setup OK");
		PRINTF(" . ADC microSeconds between samples: ", usADCinterval);
		PRINTF(" . ADC samples per half-wave: ", ADC_RATE);
		PRINTF(" . ADC samples per calculation set: ", ADC_COUNT);
		PRINTF(" . ADC half-waves per calculation set: ", ADC_WAVES);
	}
	return;
}

#endif // ESP32
