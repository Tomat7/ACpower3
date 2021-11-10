/*
	* Оригинальная идея (c) Sebra
	* Базовый алгоритм регулирования (c) Chatterbox
	* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#include <Arduino.h>
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)


ACpower3::ACpower3()
{
	Pmax = ACPOWER3_MAX;		// а надо ли??
	_pinZC[0] = ACPOWER3_PIN_ZC0;	// пин подключения детектора нуля.
	_pinTR[0] = ACPOWER3_PIN_TR0;		// пин управляющий триаком. 
	_pinZC[1] = ACPOWER3_PIN_ZC1;	
	_pinTR[1] = ACPOWER3_PIN_TR1;		
	_pinZC[2] = ACPOWER3_PIN_ZC2;	
	_pinTR[2] = ACPOWER3_PIN_TR2;		
	_pinI[0] = ACPOWER3_PIN_I0;		// пин датчика тока.
	_pinU[0] = ACPOWER3_PIN_U0;		// пин датчика напряжения. 
	_pinI[1] = ACPOWER3_PIN_I1;		
	_pinU[1] = ACPOWER3_PIN_U1;		
	_pinI[2] = ACPOWER3_PIN_I2;		
	_pinU[2] = ACPOWER3_PIN_U2;
	return;
}

ACpower3::ACpower3( uint8_t pinZC0, uint8_t pinTR0, uint8_t pinI0, uint8_t pinU0, \
					uint8_t pinZC1, uint8_t pinTR1, uint8_t pinI1, uint8_t pinU1, \
					uint8_t pinZC2, uint8_t pinTR2, uint8_t pinI2, uint8_t pinU2,
					uint16_t pmax)
{
	Pmax = pmax;				// а надо ли??
	_pinZC[0] = pinZC0;		// пин подключения детектора нуля.
	_pinTR[0] = pinTR0;		// пин управляющий триаком. 
	_pinZC[1] = pinZC1;		
	_pinTR[1] = pinTR1;		 
	_pinZC[2] = pinZC2;	
	_pinTR[2] = pinTR2;	
	_pinI[0] = pinI0;		// пин датчика тока.
	_pinU[0] = pinU0;		// пин датчика напряжения. 
	_pinI[1] = pinI1;		
	_pinU[1] = pinU1;		
	_pinI[2] = pinI2;		
	_pinU[2] = pinU2;
	return;
}


void ACpower3::init(float Iratio, float Uratio, float *pIcorr, float *pUcorr)
{  
	initTR();
	initZC();
	initADC();
	setADCratio(Iratio, Uratio);
	setRMScorrection(pIcorr, pUcorr);
}

void ACpower3::initTR()
{ 
	Angle = 0;
	
	log_cfg_ln(ACPOWER3_LIBVERSION);
	log_cfg_ln(" + Pmax: ", Pmax);
	
	for (int i=0; i<3; i++)
	{
		setup_Triac(i);
		DELAYx;
	}
}

void ACpower3::initZC(int zcIntMode, bool AdcZcAlignment)
{ 
	_zcEdge = zcIntMode;
	_adcAlign = AdcZcAlignment;
	
	for (int i=0; i<3; i++)
	{
		setup_ZeroCross(i);
		DELAYx;
	}
}

void ACpower3::initADC(uint16_t ADCrate, uint16_t ADCwaves)
{
	for (int i=0; i<3; i++)					// TEST!! наверное можно и без этого
	{
		pinMode(_pinI[i], INPUT);           // set pin to input
		pinMode(_pinU[i], INPUT);           // set pin to input
	}
	
	delay(20);
	setup_ADCzerolevel(ZEROLEVEL_SAMPLES);
	setup_ADC(ADCrate, ADCwaves);
}

void ACpower3::setADCratio(float Iratio, float Uratio, float lag)
{  
	_Iratio = Iratio;
	_Uratio = Uratio;
	_lag = lag;
	return;
}

void ACpower3::setRMScorrection(float *pIcorr, float *pUcorr)
{
	_pIcorr = pIcorr;
	_pUcorr = pUcorr;
	_corrRMS = true;
	return;
}

void ACpower3::stop()
{
	Angle = 0;
	delay(20);

	timerStop(timerADC);
	timerDetachInterrupt(timerADC);
	
	for (int i=0; i<3; i++)
	{
		timerStop(timerTriac[i]);
		timerDetachInterrupt(timerTriac[i]);
		detachInterrupt(digitalPinToInterrupt(_pinZC[i]));
		digitalWrite(_pinTR[i], LOW);
	}
	
	return;
}

	
#endif // ESP32
