/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#include "Arduino.h"
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

void ACpower3::control()
{	
	if ((millis() - _msCheckZC) > 1000) { check_ZC(); }
	
	if (xSemaphoreTake(smphRMS, 0) == pdTRUE)
	{ 
		CounterRMS++;
		
		if (getI) 	// посчитали пока только ТОК
		{	
			getI = false;
			I[_phase] = sqrt(_summ / ACPOWER3_ADC_SAMPLES) * _Iratio;
			_Icntr = CounterADC;	// не нужно
			_pin = _pinU[_phase];
			_zerolevel = _Uzerolevel[_phase];
		}
		else		// теперь посчитали ещё и НАПРЯЖЕНИЕ
		{
			getI = true;
			U[_phase] = sqrt(_summ / ACPOWER3_ADC_SAMPLES) * _Uratio;
			_Ucntr = CounterADC;	// для совместимости

			if (_corrRMS) { correct_RMS(); }
			P[_phase] = (uint16_t)(I[_phase] * U[_phase]);	
			Pnow = P[0] +  P[1] +  P[2];
			
			if (Pset > 0) //&& (ZC[_phase]))
			{
				_angle += (Pset - Pnow) / _lag; //ACPOWER3_RMS_LAG;
				_angle = constrain(_angle, ACPOWER3_ANGLE_MIN, ACPOWER3_ANGLE_MAX - ACPOWER3_ANGLE_DELTA);
			}
			else 
			{ 
				_angle = ACPOWER3_ANGLE_MIN - 500;
			}
			
			_phase++;
			
			if (_phase == 3) 
			{
				_phase = 0;
				Pavg = (uint16_t)((Pavg + Pnow) / 2); 
			}
			
			_pin = _pinI[_phase];
			_zerolevel = _Izerolevel[_phase];
		}

		_summ = 0;
		adcAttachPin(_pin);
		adcStart(_pin);
		Angle = _angle;
		
		D(RMScore = xPortGetCoreID());
		D(RMSprio = uxTaskPriorityGet(NULL));
			
		portENTER_CRITICAL(&muxADC);
#ifdef ACPOWER3_ADC_TUNING
		if (ZC[_phase]) { CounterADC = ACPOWER3_ADC_NEXT; }
		else { CounterADC = ACPOWER3_ADC_SAMPLES; }
#else
		CounterADC = ACPOWER3_ADC_START;
#endif
		portEXIT_CRITICAL(&muxADC);
		
	}
	return;
}


void ACpower3::control(uint16_t angle_)
{	
	control();
	Angle = angle_;
	return;
}

void ACpower3::setpower(uint16_t setPower)
{	
	if (setPower > Pmax) Pset = Pmax;
	else if (setPower < ACPOWER3_MIN) Pset = 0;
	else Pset = setPower;
	_lag = ACPOWER3_RMS_LAG;

	return;
}


void ACpower3::correct_RMS()
{
	int n;
	float X_head, X_tail;
	
	if ((_pUcorr) && (U[_phase] < 240))
	{
		X_head = U[_phase] / 10;
		n = (int)X_head;
		X_tail = X_head - n;
		float Ushift = *(_pUcorr + n) + (*(_pUcorr + n + 1) - *(_pUcorr + n)) * X_tail;
		U[_phase] += Ushift;
	}

	if ((_pIcorr) && (I[_phase] < 16))
	{
		X_head = I[_phase];
		n = (int)X_head;
		X_tail = X_head - n;
		float Ishift = *(_pIcorr + n) + (*(_pIcorr + n + 1) - *(_pIcorr + n)) * X_tail;
		I[_phase] += Ishift;
	}
}


void ACpower3::check_ZC()
{
	_msCheckZC = millis();
	for (int i=0; i<3; i++)
	{
		if (_ZCcntr[i] > 5) 
		{ 
			ZC[i] = true; 
		}
		else
		{ 
			ZC[i] = false;
			timerStop(timerTriac[i]);
			digitalWrite(_pinTriac[i], LOW);
#ifdef ACPOWER3_ADC_TUNING
			if (_phase == i) 
			{
				portENTER_CRITICAL(&muxADC);
				CounterADC = ACPOWER3_ADC_SAMPLES;
				portEXIT_CRITICAL(&muxADC);
			}
#endif
		}
		_ZCcntr[i] = 0;
	}
}

#endif // ESP32
