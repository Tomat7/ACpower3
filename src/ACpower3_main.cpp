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
	if (xSemaphoreTake(smphRMS, 0) == pdTRUE)
	{ 
		CounterRMS++;
		
		//PRINTF((uint32_t)_summ, _cntr);
		
		if (getI) 
		{	
<<<<<<< Updated upstream
			//I[_phase] = sqrt(_summ / _cntr) * _Iratio;
			I[_phase] = sqrt(_summ / ADC_COUNT) * _Iratio;
=======
			//I[_phase] = sqrt(_summ / CounterADC) * _Iratio;
			I[_phase] = sqrt(_summ / ACPOWER3_ADC_SAMPLES) * _Iratio;
>>>>>>> Stashed changes
			getI = false;
			_Icntr = _cntr;	// не нужно
		}
		else
		{
<<<<<<< Updated upstream
			//U[_phase] = sqrt(_summ / _cntr) * _Uratio;
			U[_phase] = sqrt(_summ / ADC_COUNT) * _Uratio;
=======
			//U[_phase] = sqrt(_summ / CounterADC) * _Uratio;
			U[_phase] = sqrt(_summ / ACPOWER3_ADC_SAMPLES) * _Uratio;
>>>>>>> Stashed changes
			getI = true;
			_Ucntr = _cntr;	// для совместимости
		}
		
		correctRMS();
		P[_phase] = (uint16_t)(I[_phase] * U[_phase]);
		
		if (getI)
		{
			_phase++;
			if (_phase == 3) _phase = 0;
			_pin = _pinI[_phase];
			_zerolevel = _Izerolevel[_phase];
		}
		else
		{
			_pin = _pinU[_phase];
			_zerolevel = _Uzerolevel[_phase];
		}
		
		adcAttachPin(_pin);
		_summ = 0;
		
		if (_phase == 0) 
		{
			Pold = Pprev;
			Pprev = Pnow;
		}
		
		Pnow = P[0] +  P[1] +  P[2];
		
		if (_phase == 0) 
		{ 
			Pavg = (uint16_t)((Pnow + Pprev + Pold) / 3); 
		}
		
		adcStart(_pin);
		
		if (Pset > 0)
		{
			//int16_t Pdiff = Pset - Pnow;
			_angle += (Pset - Pnow) / _lag;
			_angle = constrain(_angle, ACPOWER3_ANGLE_MIN, ACPOWER3_ANGLE_MAX - ACPOWER3_ANGLE_DELTA);
		}
		else _angle = ACPOWER3_ANGLE_MIN - 500;
		
		Angle = _angle;
		D(RMScore = xPortGetCoreID());
		D(RMSprio = uxTaskPriorityGet(NULL));
		
		portENTER_CRITICAL(&muxADC);
<<<<<<< Updated upstream
		_cntr = ADC_COUNT + 10;
		portEXIT_CRITICAL(&muxADC);
=======
//		if (ZC[_phase]) { CounterADC = ACPOWER3_ADC_NEXT; }
//		else { CounterADC = ACPOWER3_ADC_START; }
		CounterADC = ACPOWER3_ADC_START;
		portEXIT_CRITICAL(&muxADC);

		check_ZC();
>>>>>>> Stashed changes
	}
	return;
}


void ACpower3::control(uint16_t angle_)
{	
		
	Angle = angle_;
	return;
}

<<<<<<< Updated upstream
=======
void ACpower3::setpower(uint16_t setPower)
{	
	if (setPower > Pmax) Pset = Pmax;
	else if (setPower < ACPOWER3_MIN) Pset = 0;
	else Pset = setPower;
//	_lag = 1 / (0.3 + (abs(ACPOWER3_ANGLE_MIDDLE - Angle) / 10000));
	_lag = 4;
	
/*
	float xP = abs((Pset / Pmax) - 0.5);
	if (xP > 0.2) _lag = 2;
	else if (xP > 0.1) _lag = 3;
	else _lag = 4;
*/	
	return;
}

>>>>>>> Stashed changes

void ACpower3::correctRMS()
{
	if (_corrRMS)
	{
		int n;
		float X_head, X_tail;
		
		if ((getI) && (_pUcorr) && (U[_phase] < 240))
		{
			X_head = U[_phase] / 10;
			n = (int)X_head;
			X_tail = X_head - n;
			float Ushift = *(_pUcorr + n) + (*(_pUcorr + n + 1) - *(_pUcorr + n)) * X_tail;
			U[_phase] += Ushift;
		}

		if ((!getI) && (_pIcorr) && (I[_phase] < 16))
		{
			X_head = I[_phase];
			n = (int)X_head;
			X_tail = X_head - n;
			float Ishift = *(_pIcorr + n) + (*(_pIcorr + n + 1) - *(_pIcorr + n)) * X_tail;
			I[_phase] += Ishift;
		}
	}
}

void ACpower3::setpower(uint16_t setPower)
{	
	if (setPower > Pmax) Pset = Pmax;
	else if (setPower < ACPOWER3_MIN) Pset = 0;
	else Pset = setPower;
	
	float xP = abs((Pset / Pmax) - 0.5);
	
	if (xP > 0.2) _lag = 2;
	else if (xP > 0.1) _lag = 3;
	else _lag = 4;
	
	return;
}

#endif // ESP32
