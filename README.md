**Warning! Сurrent version not tested with ESP32 Arduino core v3.0 and upper.**
**Use ESP32 Arduino Core v.2.0.17**

**Внимание! Текущая версия не тестировалась с ESP32 Arduino core версии 3.0 и выше.**
**Используйте ESP32 Arduino Core v.2.0.17**

# ACpower3
 Three-phase AC power regulator on ESP32
 
 __Warning! Hi voltage is dangerous! Be careful!__
=================================================

Based on 1-phase regulator.  See here https://github.com/Tomat7/ACpower for more links and details.

Current version not widely tested yet. :-)  See example "ACpower-3phase-mini03" only.
  
Pinout for ESP32 Wemos Lolin32. (updated!)
  
Line1:
* 15 // zero-crossing detector / детектор нуля
* 25 // triac / триак
* 36  // current meter / датчик тока
* 39  // voltage meter / датчик напряжения

Line2:
* 14  // детектор нуля ??
* 26  // триак 
* 32  // датчик тока
* 33  // датчик напряжения

Line3:
* 13  // детектор нуля
* 27  // триак ??
* 34  // датчик тока
* 35  // датчик напряжения
