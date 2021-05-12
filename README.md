**Warning! Сurrent version now working with ESP32 Arduino core v1.0.6 and upper.**

**Внимание! Текущая версия уже работае с ESP32 Arduino core версии 1.0.6 и выше.**

# ACpower3
 Three-phase AC power regulator on ESP32
 
 __Warning! Hi voltage is dangerous! Be careful!__
=================================================

Based on 1-phase regulator.  See here https://github.com/Tomat7/ACpower for more links and details.

Current version not widely tested yet. :-)  See example "ACpower-3phase-mini03" only.
  
Pinout for ESP32 Wemos Lolin32.
  
Line1:
* 25 // zero-crossing detector / детектор нуля
* 26 // triac / триак
* 36  // current meter / датчик тока
* 39  // voltage meter / датчик напряжения

Line2:
* 14  // детектор нуля ??
* 27  // триак 
* 32  // датчик тока
* 33  // датчик напряжения

Line3:
* 13  // детектор нуля
* 12  // триак ??
* 34  // датчик тока
* 35  // датчик напряжения
