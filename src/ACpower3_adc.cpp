/*
 * Оригинальная идея (c) Sebra
 * Базовый алгоритм регулирования (c) Chatterbox
 * Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
 */

#include <Arduino.h>
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

portMUX_TYPE ACpower3::muxADC = portMUX_INITIALIZER_UNLOCKED;

volatile SemaphoreHandle_t ACpower3::smphRMS;
volatile int16_t ACpower3::Xnow;
volatile uint32_t ACpower3::X2;
volatile uint64_t ACpower3::_summ = 0;
volatile uint16_t ACpower3::_adcCounterMax;
volatile uint8_t ACpower3::_pin;

volatile uint32_t ACpower3::_adcCounter = 1;
volatile uint16_t ACpower3::_zerolevel = 0;

void ACpower3::setup_ADC(uint16_t ADCrate, uint16_t ADCwaves)
{
	_adcCounterMax = ADCrate * ADCwaves; // + (uint16_t)(ADCrate * 0.1);
	uint16_t usADCinterval = (uint16_t)(10000 / ADCrate);

	smphRMS = xSemaphoreCreateBinary();
	getI = true;

	_adcCounter = _adcCounterMax;
	_phase = 0;
	_pin = _pinI[_phase];
	_zerolevel = _Izerolevel[_phase];

	timerADC = timerBegin(ACPOWER3_ADC_TIMER, 80, true);
	timerAttachInterrupt(timerADC, &GetADC_int, true);
	timerAlarmWrite(timerADC, usADCinterval, true);
	timerAlarmEnable(timerADC);
	DELAYx;

	log_cfg_ln(" + ADC Inerrupt setup OK");
	log_cfg_ln(" . ADC half-waves per calculation set: ", ADCwaves);
	log_cfg_ln(" . ADC samples per half-wave: ", ADCrate);
	log_cfg_ln(" . ADC samples per calculation set: ", _adcCounterMax);
	log_cfg_ln(" . ADC microSeconds between samples: ", usADCinterval);

	return;
}

void ACpower3::reset_ADC()
{
	if (_adcAlign)
	{
		if (ZC[_phase])
			_adcCounter = ACPOWER3_ADC_NEXT;
		else
			_adcCounter = _adcCounterMax;
	}
	else
		_adcCounter = ACPOWER3_ADC_START;
	
	return;
}

void IRAM_ATTR ACpower3::GetADC_int() //__attribute__((always_inline))
{
	portENTER_CRITICAL_ISR(&muxADC);

	if (_adcCounter < _adcCounterMax)
	{
		Xnow = adcEnd(_pin) - _zerolevel;
		X2 = Xnow * Xnow;
		if (X2 < ACPOWER3_ADC_NOISE)
			X2 = 0;
		_summ += X2;
		_adcCounter++;
		adcStart(_pin);
	}
	else if (_adcCounter == ACPOWER3_ADC_START)
	{
		adcEnd(_pin);
		_adcCounter = 0;
		adcStart(_pin);
	}
	else if (_adcCounter == _adcCounterMax)
	{
		adcEnd(_pin);
		_adcCounter = ACPOWER3_ADC_DONE;
		xSemaphoreGiveFromISR(smphRMS, NULL);
	}

	portEXIT_CRITICAL_ISR(&muxADC);
	D(ADCcore = xPortGetCoreID());
	D(ADCprio = uxTaskPriorityGet(NULL));
	return;
}

void ACpower3::setup_ADCzerolevel(uint16_t Scntr)
{
	log_cfg_ln(" + RMS calculating ZERO-shift for U and I...");
	Angle = 0;
	for (int i = 0; i < 3; i++)
	{
		log_cfg(" . ", i);
		log_cfg(" I-meter on pin ", _pinI[i]);
		log_cfg_ln(", U-meter on pin ", _pinU[i]);

		_Izerolevel[i] = get_ZeroLevel(_pinI[i], Scntr);
		_Uzerolevel[i] = get_ZeroLevel(_pinU[i], Scntr);

		log_cfg(" . ", i);
		log_cfg(" RMS ZeroLevel I: ", _Izerolevel[i]);
		log_cfg_ln(", U: ", _Uzerolevel[i]);
	}
	return;
}

uint16_t ACpower3::get_ZeroLevel(uint8_t z_pin, uint16_t Scntr)
{
	uint32_t ZeroShift = 0;
	adcAttachPin(z_pin);
	DELAYx;
	adcStart(z_pin);
	for (int i = 0; i < Scntr; i++)
	{
		ZeroShift += adcEnd(z_pin);
		adcStart(z_pin);
		delayMicroseconds(ZEROLEVEL_DELAY);
	}
	adcEnd(z_pin);
	return (uint16_t)(ZeroShift / Scntr);
}

#endif // ESP32
