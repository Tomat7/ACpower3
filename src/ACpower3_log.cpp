/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/


#include <Arduino.h>
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)


void ACpower3::log_cfg(String str0)
{
#ifdef DEBUG0
#ifdef DEBUG1
	PRINT(str0);
#endif
	LibConfig += str0;
#endif // DEBUG0
}

void ACpower3::log_cfg(String str0, uint16_t num1)
{
#ifdef DEBUG0
	log_cfg(str0);
	log_cfg(String(num1));
#endif // DEBUG0
}

void ACpower3::log_cfg_ln(String str0)
{
#ifdef DEBUG0
	str0 += "\r\n";
	log_cfg(str0);
#endif // DEBUG0
}

void ACpower3::log_cfg_ln(String str0, String str1)
{
#ifdef DEBUG0
	log_cfg(str0);
	log_cfg_ln(str1);
#endif // DEBUG0
}

void ACpower3::log_cfg_ln(String str0, uint16_t num1)
{
#ifdef DEBUG0
	log_cfg_ln(str0, String(num1));
#endif // DEBUG0
}

#endif // ESP32

	