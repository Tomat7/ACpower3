/*
 * Оригинальная идея (c) Sebra
 * Базовый алгоритм регулирования (c) Chatterbox
 * Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
 */

#include <Arduino.h>
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

void ACpower3::calculate()
{
	CounterRMS++;

	if (getI) // посчитали пока только ТОК
	{
		getI = false;
		I[_phase] = sqrt(_summ / _adcCounterMax) * _Iratio;
		CounterI = _adcCounter; // не нужно
		_pin = _pinU[_phase];
		_zerolevel = _Uzerolevel[_phase];
	}
	else // теперь посчитали ещё и НАПРЯЖЕНИЕ
	{
		getI = true;
		U[_phase] = sqrt(_summ / _adcCounterMax) * _Uratio;
		CounterU = _adcCounter; // для совместимости

		calc_power();
		calc_angle();
		calc_phase(); // следующая фаза

		_pin = _pinI[_phase];
		_zerolevel = _Izerolevel[_phase];
	}

	_summ = 0;
	adcAttachPin(_pin);
	adcStart(_pin);

	set_angle();

	D(RMScore = xPortGetCoreID());
	D(RMSprio = uxTaskPriorityGet(NULL));

	portENTER_CRITICAL(&muxADC);
	reset_ADC();
	portEXIT_CRITICAL(&muxADC);

	MillisRMS = millis();

	return;
}

void ACpower3::set_angle()
{
	Angle = _angle;

	for (int i = 0; i < 3; i++)
	{
		if (ZC[i])
			A[i] = Angle;
		else
			A[i] = ACPOWER3_ANGLE_MIN - 500;
	}

	if (_corrAngle && ZC[0] && ZC[1] && ZC[2])
		A[2] = ACPOWER3_ANGLE_CORR;
}

void ACpower3::calc_angle()
{
	if (Pset > 0)
	{
		_angle += (Pset - Pnow) / _lag;
		_angle = constrain(_angle, ACPOWER3_ANGLE_MIN, ACPOWER3_ANGLE_MAX - ACPOWER3_ANGLE_DELTA);
	}
	else
	{
		_angle = ACPOWER3_ANGLE_MIN - 500;
	}
	return;
}

void ACpower3::calc_power()
{
	if (_corrRMS)
		correct_RMS();

	P[_phase] = (uint16_t)(I[_phase] * U[_phase]);
	Pnow = P[0] + P[1] + P[2];

	if (_phase == 2) // если обсчитали третью фазу - считаем среднее за все три 
	{
		Pavg = (uint16_t)((Pavg + Pnow) / 2);						 // Pavg "меняется" очень медленно
		Pmid = (uint16_t)((Pold[0] + Pold[1] + Pold[2] + Pnow) / 4); 
		Pold[2] = Pold[1];
		Pold[1] = Pold[0];
		Pold[0] = Pnow;
	}
	return;
}

void ACpower3::calc_phase()
{
	_phase++; // следующая фаза
	if (_phase == 3)
		_phase = 0; // если "перебор" - считаем среднее и начинаем опять с нулевой фазы
	return;
}

void ACpower3::control()
{
	if ((millis() - _zcCheckMillis) > 1000)
		check_ZC();
	if (xSemaphoreTake(smphRMS, 0) == pdTRUE)
		calculate();
	return;
}

void ACpower3::control(uint16_t angle_)
{
	control();
	Angle = angle_;

	for (int i = 0; i < 3; i++)
	{
		if (ZC[i])
			A[i] = Angle;
		else
			A[i] = ACPOWER3_ANGLE_MIN - 500;
	}

	return;
}

void ACpower3::setpower(uint16_t setPower)
{
	if (setPower > Pmax)
		Pset = Pmax;
	else if (setPower < ACPOWER3_MIN)
		Pset = 0;
	else
		Pset = setPower;

	if ((Pset > Pmax / 6.9) && (Pset < Pmax / 4.5))
		_corrAngle = true;
	else
		_corrAngle = false;
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
	_zcCheckMillis = millis();
	for (int i = 0; i < 3; i++)
	{
		if (_zcCounter[i] > 5)
		{
			ZC[i] = true;
		}
		else
		{
			ZC[i] = false;
			timerStop(timerTriac[i]);
			digitalWrite(_pinTR[i], LOW);

			if (_adcAlign)
			{
				if (_phase == i)
				{
					portENTER_CRITICAL(&muxADC);
					_adcCounter = _adcCounterMax;
					portEXIT_CRITICAL(&muxADC);
				}
			}
		}
		_zcCounter[i] = 0;
	}
}

#endif // ESP32
