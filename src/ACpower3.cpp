/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#include "Arduino.h"
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

uint8_t ACpower3::_pinI;
uint8_t ACpower3::_pinU;
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


void ACpower3::init(uint16_t* pAngle)
{  
	//PRINTF("_pAngle=", (uint32_t)_pAngle, HEX);
	init(pAngle, true);
	//init(Iratio, Uratio, true);
	return;
}

void ACpower3::init(uint16_t* pAngle, bool NeedCalibrate)
{  
	_pAngle = pAngle;
	//_Iratio = Iratio;
	//_Uratio = Uratio;
	//_pAngle = (uint16_t*) malloc(sizeof(uint16_t));
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
	/*if (_phaseQty == 1)
	{
		_Iratio = Iratio;
		_Uratio = Uratio;
		if (NeedCalibrate) calibrate(SHIFT_CHECK_SAMPLES);
		setup_ADC();
		DELAYx;
	} */
	return;
}

void ACpower3::control()
{	
	if (xSemaphoreTake(smphRMS, 0) == pdTRUE)
	{ 
		CounterRMS++;
		if (getI) Unow = sqrt(_U2summ / _Ucntr) * _Uratio;
		else Inow = sqrt(_I2summ / _Icntr) * _Iratio;
		
		correctRMS();
		Pnow = (uint16_t)(Inow * Unow);
		
		if (Pset > 0)
		{
			_angle += Pset - Pnow;
			_angle = constrain(_angle, ANGLE_MIN, ANGLE_MAX - ANGLE_DELTA);
		}
		else _angle = ANGLE_MIN - 500;
		
		*_pAngle = _angle;
		Angle = *_pAngle;
		D(RMScore = xPortGetCoreID());
		D(RMSprio = uxTaskPriorityGet(NULL));
	}
	return;
}


void ACpower3::setRMScorrection(float *pIcorr, float *pUcorr)
{
	_pIcorr = pIcorr;
	_pUcorr = pUcorr;
	_corrRMS = true;
}

void ACpower3::correctRMS()
{
	if (_corrRMS)
	{
		int n;
		float X_head, X_tail;
		
		if ((getI) && (_pUcorr) && (Unow < 240))
		{
			X_head = Unow / 10;
			n = (int)X_head;
			X_tail = X_head - n;
			float Ushift = *(_pUcorr + n) + (*(_pUcorr + n + 1) - *(_pUcorr + n)) * X_tail;
			Unow += Ushift;
		}

		if ((!getI) && (_pIcorr) && (Inow < 16))
		{
			X_head = Inow;
			n = (int)X_head;
			X_tail = X_head - n;
			float Ishift = *(_pIcorr + n) + (*(_pIcorr + n + 1) - *(_pIcorr + n)) * X_tail;
			Inow += Ishift;
		}
	}
}

void ACpower3::stop()
{
	*_pAngle = 0;
	delay(20);
	if (_phaseQty == 1)
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
