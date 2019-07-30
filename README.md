# ACpower3
 Three-phase AC power regulator on ESP32

See some details here https://github.com/Tomat7/ACpower

Worked, but not widely tested yet.
  
See example to find used PINs.
  
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
