/*
	* Оригинальная идея (c) Sebra
	* Базовый алгоритм регулирования (c) Chatterbox
	* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#include "Arduino.h"
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

//uint8_t ACpower3::_pinI;
//uint8_t ACpower3::_pinU;
//uint16_t ACpower3::_Izerolevel = 0;
//uint16_t ACpower3::_Uzerolevel = 0;
//uint8_t ACpower3::_phaseQty = 0;

ACpower3::ACpower3()
{
	Pmax = ACPOWER3_MAX;		// а надо ли??
	_pinZCross[0] = PIN_LIB_ZC0;	// пин подключения детектора нуля.
	_pinTriac[0] = PIN_LIB_TR0;		// пин управляющий триаком. 
	_pinZCross[1] = PIN_LIB_ZC1;	
	_pinTriac[1] = PIN_LIB_TR1;		
	_pinZCross[2] = PIN_LIB_ZC2;	
	_pinTriac[2] = PIN_LIB_TR2;		
	_pinI[0] = PIN_LIB_I0;		// пин датчика тока.
	_pinU[0] = PIN_LIB_U0;		// пин датчика напряжения. 
	_pinI[1] = PIN_LIB_I1;		
	_pinU[1] = PIN_LIB_U1;		
	_pinI[2] = PIN_LIB_I2;		
	_pinU[2] = PIN_LIB_U2;		
	return;
}

ACpower3::ACpower3( uint8_t pinZC0, uint8_t pinTR0, uint8_t pinI0, uint8_t pinU0, \
					uint8_t pinZC1, uint8_t pinTR1, uint8_t pinI1, uint8_t pinU1, \
					uint8_t pinZC2, uint8_t pinTR2, uint8_t pinI2, uint8_t pinU2)
{
	Pmax = ACPOWER3_MAX;		// а надо ли??
	_pinZCross[0] = pinZC0;		// пин подключения детектора нуля.
	_pinTriac[0] = pinTR0;		// пин управляющий триаком. 
	_pinZCross[1] = pinZC1;		
	_pinTriac[1] = pinTR1;		 
	_pinZCross[2] = pinZC2;	
	_pinTriac[2] = pinTR2;	
	_pinI[0] = pinI0;		// пин датчика тока.
	_pinU[0] = pinU0;		// пин датчика напряжения. 
	_pinI[1] = pinI1;		
	_pinU[1] = pinU1;		
	_pinI[2] = pinI2;		
	_pinU[2] = pinU2;		
	return;
}

ACpower3::ACpower3( uint8_t pinZC0, uint8_t pinTR0, uint8_t pinI0, uint8_t pinU0, \
					uint8_t pinZC1, uint8_t pinTR1, uint8_t pinI1, uint8_t pinU1, \
					uint8_t pinZC2, uint8_t pinTR2, uint8_t pinI2, uint8_t pinU2,
					uint16_t pmax)
{
	Pmax = pmax;		// а надо ли??
	_pinZCross[0] = pinZC0;		// пин подключения детектора нуля.
	_pinTriac[0] = pinTR0;		// пин управляющий триаком. 
	_pinZCross[1] = pinZC1;		
	_pinTriac[1] = pinTR1;		 
	_pinZCross[2] = pinZC2;	
	_pinTriac[2] = pinTR2;	
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
	initADC();
	setupADCratio(Iratio, Uratio);
	setupRMScorrection(pIcorr, pUcorr);
}

void ACpower3::initTR()
{ 
	Angle = 0;
	
	log_cfg_ln(LIBVERSION);
	log_cfg_f(" + Pmax: ", Pmax);
	
	for (int i=0; i<3; i++)
	{
		DELAYx;
		setup_Triac(i);
		DELAYx;
		setup_ZeroCross(i);
		DELAYx;
	}
}

void ACpower3::initADC()
{
	for (int i=0; i<3; i++)					// TEST!! наверное можно и без этого
	{
		pinMode(_pinI[i], INPUT);           // set pin to input
		pinMode(_pinU[i], INPUT);           // set pin to input
	}
	
	delay(20);
	setup_ADCzerolevel(ZEROLEVEL_SAMPLES);
	setup_ADC();
}

void ACpower3::setupADCratio(float Iratio, float Uratio)
{  
	_Iratio = Iratio;
	_Uratio = Uratio;
	return;
}

void ACpower3::setupRMScorrection(float *pIcorr, float *pUcorr)
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
		detachInterrupt(digitalPinToInterrupt(_pinZCross[i]));
		digitalWrite(_pinTriac[i], LOW);
	}
	
	return;
}

	
#endif // ESP32
