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
	log_cfg(" . ", i);
	log_cfg(" TRIAC on pin ", _pinTriac[i]);

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
	log_cfg_ln(" - OK");
	
	return;
}

void ACpower3::setup_ZeroCross(uint8_t i)
{
	log_cfg(" . ", i);
	log_cfg(" ZCross on pin ", _pinZCross[i]);
	
	//takeADC = false;
	_msZCmillis[i] = millis();
	pinMode(_pinZCross[i], INPUT_PULLUP);
	
	if (i==0) attachInterrupt(digitalPinToInterrupt(_pinZCross[i]), ZeroCross_int0, ZC_EDGE);
	else if (i==1) attachInterrupt(digitalPinToInterrupt(_pinZCross[i]), ZeroCross_int1, ZC_EDGE);
	else if (i==2) attachInterrupt(digitalPinToInterrupt(_pinZCross[i]), ZeroCross_int2, ZC_EDGE);

	log_cfg_ln(" - OK");

	return;
}

void ACpower3::setup_ADC()
{
	//uint16_t ADCperSet = ADC_RATE * ADC_WAVES;
	uint16_t usADCinterval = (uint16_t)(10000 / ADC_RATE);
	smphRMS = xSemaphoreCreateBinary();
	getI = true;
	_cntr = ADC_COUNT;
	_phase = 0;
	_pin = _pinI[_phase];
	_zerolevel = _Izerolevel[_phase];
	
	timerADC = timerBegin(TIMER_ADC, 80, true);
	timerAttachInterrupt(timerADC, &GetADC_int, true);
	timerAlarmWrite(timerADC, usADCinterval, true);
	timerAlarmEnable(timerADC);
	DELAYx;
	
	log_cfg_ln(" + ADC Inerrupt setup OK");
	log_cfg_f(" . ADC microSeconds between samples: ", usADCinterval);
	log_cfg_f(" . ADC samples per half-wave: ", ADC_RATE);
	log_cfg_f(" . ADC samples per calculation set: ", ADC_COUNT);
	log_cfg_f(" . ADC half-waves per calculation set: ", ADC_WAVES);
	
	return;
}


void ACpower3::log_cfg(String str0)
{
#ifdef DEBUG0
#ifdef DEBUG1
	PRINT(str0);
#endif
	LibConfig += str0;
#endif // DEBUG0
}

void ACpower3::log_cfg(String str0, uint16_t num1)
{
#ifdef DEBUG0
	log_cfg(str0);
	log_cfg(String(num1));
#endif // DEBUG0
}

void ACpower3::log_cfg_ln(String str0)
{
#ifdef DEBUG0
	str0 += "\r\n";
	log_cfg(str0);
#endif // DEBUG0
}

void ACpower3::log_cfg_f(String str0, String str1)
{
#ifdef DEBUG0
	log_cfg(str0);
	log_cfg_ln(str1);
#endif // DEBUG0
}

void ACpower3::log_cfg_f(String str0, uint16_t num1)
{
#ifdef DEBUG0
	log_cfg_f(str0, String(num1));
#endif // DEBUG0
}

#endif // ESP32
