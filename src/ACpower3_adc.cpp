/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#include "Arduino.h"
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

portMUX_TYPE ACpower3::muxADC = portMUX_INITIALIZER_UNLOCKED;
//hw_timer_t *ACpower3::timerTriac = NULL;
volatile SemaphoreHandle_t ACpower3::smphRMS;

volatile bool ACpower3::getI = true;
volatile bool ACpower3::takeADC = false;

//volatile uint16_t* ACpower3::_pAngle;
//volatile uint16_t ACpower3::Angle;
volatile int16_t ACpower3::Xnow;
volatile uint32_t ACpower3::X2;

//volatile uint8_t ACpower3::_zero = 1;
//volatile uint32_t ACpower3::CounterZC[3];
//volatile uint32_t ACpower3::CounterTR;

volatile uint8_t ACpower3::_pin;
/*
	uint8_t ACpower3::_pinI;
uint8_t ACpower3::_pinU;
uint8_t ACpower3::_pinTriac;
//uint8_t ACpower3::_pinZCross;
*/

volatile uint32_t ACpower3::_cntr = 1;
//volatile uint32_t ACpower3::_Icntr = 1;
//volatile uint32_t ACpower3::_Ucntr = 1;

volatile uint64_t ACpower3::_summ = 0;
//volatile uint64_t ACpower3::_I2summ = 0;
//volatile uint64_t ACpower3::_U2summ = 0;

volatile uint16_t ACpower3::_zerolevel = 0;
/*
uint16_t ACpower3::_Izerolevel = 0;
uint16_t ACpower3::_Uzerolevel = 0;
*/
//volatile uint32_t ACpower3::_msZCmillis[3];
//volatile bool ACpower3::trOpened;


void IRAM_ATTR ACpower3::GetADC_int() //__attribute__((always_inline))
{
	portENTER_CRITICAL_ISR(&muxADC);
	
	if (_cntr < ADC_COUNT)
	{
		Xnow = adcEnd(_pin) - _zerolevel;
		X2 = Xnow * Xnow;
		if (X2 < ADC_NOISE) X2 = 0;
		_summ += X2;
		_cntr++;
		adcStart(_pin);
	}
	else if (_cntr == (ADC_COUNT + 10))
	{
		adcEnd(_pin);
		_cntr = 0;
		adcStart(_pin);
	}
	else if (_cntr == ADC_COUNT)
	{
		xSemaphoreGiveFromISR(smphRMS, NULL);
		_cntr++;
	}
	
	portEXIT_CRITICAL_ISR(&ACpower3::muxADC);
	D(ADCcore = xPortGetCoreID());
	D(ADCprio = uxTaskPriorityGet(NULL));
	return;
}

void ACpower3::setRMSzerolevel(uint16_t Scntr)
{
	PRINTLN(" + RMS calculating ZERO-shift for U and I...");
	Angle = 0;
	for (int i = 0; i < 3; i++) 
	{
		if (_ShowLog)
		{
			
			PRINT(" . ");
			PRINT(i);
			PRINT(" I-meter on pin ");
			PRINT(_pinI[i]);
			PRINTF(", U-meter on pin ", _pinI[i]);
		}
		
		_Izerolevel[i] = get_ZeroLevel(_pinI[i], Scntr);
		_Uzerolevel[i] = get_ZeroLevel(_pinU[i], Scntr);

		if (_ShowLog)
		{
			PRINT(" . ");
			PRINT(i);
			PRINT(" RMS ZeroLevel I: ");
			PRINT(_Izerolevel[i]);
			PRINTF(", U: ", _Uzerolevel[i]);
		}
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
		delayMicroseconds(50);
	}
	adcEnd(z_pin);
	return (uint16_t)(ZeroShift / Scntr);
}


#endif // ESP32
