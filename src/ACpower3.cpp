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
uint16_t ACpower3::_Izerolevel = 0;
uint16_t ACpower3::_Uzerolevel = 0;
//uint8_t ACpower3::_phaseQty = 0;

ACpower3::ACpower3()
{
}

ACpower3::ACpower3(uint8_t pinZC0, uint8_t pinTR0, uint8_t pinZC1, uint8_t pinTR1, uint8_t pinZC2, uint8_t pinTR2)
{
	Pmax = POWER_MAX * 3;		// а надо ли??
	_pinZCross[0] = pinZC0;		// пин подключения детектора нуля.
	_pinTriac[0] = pinTR0;		// пин управляющий триаком. 
	_pinZCross[1] = pinZC1;		// пин подключения детектора нуля.
	_pinTriac[1] = pinTR1;		// пин управляющий триаком. 
	_pinZCross[2] = pinZC2;		// пин подключения детектора нуля.
	_pinTriac[2] = pinTR2;		// пин управляющий триаком. 
	_ShowLog = true;
	return;
}


void ACpower3::init()
{  
	Angle = 0;
	if (_ShowLog)
	{
		Serial.println(F(LIBVERSION));
		PRINTF(" + Pmax: ", Pmax);
	}
	for (int i=0; i<3; i++)
	{
		
		//if (_ShowLog) printConfig(i);
		DELAYx;
		setup_Triac(i);
		DELAYx;
		setup_ZeroCross(i);
		DELAYx;
	}
	return;
}

void ACpower3::initADC(uint8_t pinI0, uint8_t pinU0, uint8_t pinI1, uint8_t pinU1, uint8_t pinI2, uint8_t pinU2)
{
	_pinI[0] = pinI0;		// пин подключения детектора нуля.
	_pinU[0] = pinU0;		// пин управляющий триаком. 
	_pinI[1] = pinI1;		// пин подключения детектора нуля.
	_pinU[1] = pinU1;		// пин управляющий триаком. 
	_pinI[2] = pinI2;		// пин подключения детектора нуля.
	_pinU[2] = pinU2;		// пин управляющий триаком. 
	_useADC = true;
	setup_ADC();
}
/*
void ACpower3::init(uint16_t* pAngle, bool NeedCalibrate)
{  
	init();
	_pAngle = pAngle;
	//_Iratio = Iratio;
	//_Uratio = Uratio;
	//_pAngle = (uint16_t*) malloc(sizeof(uint16_t));

	if (_phaseQty == 1)
	{
		_Iratio = Iratio;
		_Uratio = Uratio;
		if (NeedCalibrate) calibrate(SHIFT_CHECK_SAMPLES);
		setup_ADC();
		DELAYx;
	} 
	return;
}
*/

void ACpower3::stop()
{
	Angle = 0;
	delay(20);
	if (_useADC)
	{
		timerStop(timerADC);
		timerDetachInterrupt(timerADC);
	}
	for (int i=0; i<3; i++)
	{
		timerStop(timerTriac[i]);
		timerDetachInterrupt(timerTriac[i]);
		detachInterrupt(digitalPinToInterrupt(_pinZCross[i]));
		digitalWrite(_pinTriac[i], LOW);
	}
}

void ACpower3::setpower(uint16_t setPower)
{	
	if (setPower > Pmax) Pset = Pmax;
	else if (setPower < POWER_MIN) Pset = 0;
	else Pset = setPower;
	return;
}


void ACpower3::printConfig(uint8_t i)
{
	Serial.print(F(" . ZeroCross on pin "));
	Serial.print(_pinZCross[i]);
	Serial.print(F(", Triac on pin "));
	Serial.println(_pinTriac[i]);
		
/*		if (_phaseQty == 1)
		{
			Serial.print(F(" . U-meter on pin "));
			Serial.print(_pinU);
			Serial.print(F(", I-meter on pin "));
			Serial.println(_pinI);
		}
*/

}

#endif // ESP32
