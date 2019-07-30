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

void ACpower3::control()
{	
	if (xSemaphoreTake(smphRMS, 0) == pdTRUE)
	{ 
		CounterRMS++;
		
		//PRINTF((uint32_t)_summ, _cntr);
		
		if (getI) 
		{	
			I[_now] = sqrt(_summ / _cntr) * _Iratio;
			getI = false;
		}
		else
		{
			U[_now] = sqrt(_summ / _cntr) * _Uratio;
			getI = true;
		}		
		correctRMS();
		P[_now] = (uint16_t)(I[_now] * U[_now]);
		
		if (getI)
		{
			_now++;
			if (_now == 3) _now = 0;
			_pin = _pinI[_now];
		}
		else _pin = _pinU[_now];
		
		adcAttachPin(_pin);
		_summ = 0;
		Pnow = P[0] +  P[1] +  P[2];		
		adcStart(_pin);
		
		if (Pset > 0)
		{
			_angle += Pset - Pnow;
			_angle = constrain(_angle, ANGLE_MIN, ANGLE_MAX - ANGLE_DELTA);
		}
		else _angle = ANGLE_MIN - 500;
		
		Angle = _angle;
		D(RMScore = xPortGetCoreID());
		D(RMSprio = uxTaskPriorityGet(NULL));
		
		portENTER_CRITICAL(&muxADC);
		_cntr = ADC_COUNT + 10;
		portEXIT_CRITICAL(&muxADC);
	}
	return;
}

void ACpower3::control(uint16_t angle_)
{	
		
	Angle = angle_;
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
		
		if ((getI) && (_pUcorr) && (U[_now] < 240))
		{
			X_head = U[_now] / 10;
			n = (int)X_head;
			X_tail = X_head - n;
			float Ushift = *(_pUcorr + n) + (*(_pUcorr + n + 1) - *(_pUcorr + n)) * X_tail;
			U[_now] += Ushift;
		}

		if ((!getI) && (_pIcorr) && (I[_now] < 16))
		{
			X_head = I[_now];
			n = (int)X_head;
			X_tail = X_head - n;
			float Ishift = *(_pIcorr + n) + (*(_pIcorr + n + 1) - *(_pIcorr + n)) * X_tail;
			I[_now] += Ishift;
		}
	}
}

#endif // ESP32
