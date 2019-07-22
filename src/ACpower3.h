/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
* Version 3.1 (ESP32 support added starting v3)
* http://forum.homedistiller.ru/index.php?topic=166750.0
* https://tomat.visualstudio.com/ESP32-AC-power
*	
* ESP32 pin connections (ESP32 Wemos Lolin32):
* 39 - Voltage meter (https://learn.openenergymonitor.org/electricity-monitoring/voltage-sensing/measuring-voltage-with-an-acac-power-adapter )
* 36 - Current transformer (https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/interface-with-arduino )
* 25 - ZeroCross detector ()
* 26 - Triac ()
*
* ACpower3(uint16_t Pm) = ACpower3(MAXPOWER, 25, 26, 39, 36)
*/
#ifndef ACpower3_h
#define ACpower3_h

#if defined(ESP32)

#define LIBVERSION "ACpower3_v20190719 "

#define ZC_CRAZY		// если ZeroCross прерывание выполняется слишком часто :-(
#define ZC_EDGE RISING	// FALLING, RISING

#define ADC_RATE 200    // количество отсчетов АЦП на ПОЛУволну - 200 (для прерываний)
#define ADC_WAVES 10    // количество обсчитываемых ПОЛУволн - 4
#define ADC_NOISE 1000  // попробуем "понизить" шум АЦП

#define U_ZERO 1931     //2113
#define I_ZERO 1942     //1907

#define PIN_U 39
#define PIN_I 36
#define PIN_ZCROSS 25
#define PIN_TRIAC 26

#define ANGLE_MIN 1000		// минимальный угол открытия - определяет MIN возможную мощность
#define ANGLE_MAX 10100		// максимальный угол открытия триака - определяет MAX возможную мощность
#define ANGLE_DELTA 100		// запас по времени для открытия триака
#define POWER_MAX 3000		// больше этой мощности установить не получится
#define POWER_MIN 50		// минимально допустимая устанавливаемая мощность (наверное можно и меньше)

#define TIMER_TRIAC 0
#define TIMER_ADC 3
#define SHIFT_CHECK_SAMPLES 10000	// количество отсчетов для определения "нулевого" уровня

//#define DEBUG1
//#define DEBUG2


class ACpower3
{
public:
	ACpower3();
	ACpower3(uint8_t pinZC0, uint8_t pinTR0, uint8_t pinZC1, uint8_t pinTR1, uint8_t pinZC2, uint8_t pinTR2);
	//ACpower3(uint16_t Pm, uint8_t pinZeroCross, uint8_t pinTriac, uint8_t pinVoltage, uint8_t pinCurrent);
	//ACpower3(uint16_t Pm, uint8_t pinZeroCross, uint8_t pinTriac, uint8_t pinVoltage, uint8_t pinCurrent, bool ShowLog);
	
	float Inow = 0;   		// переменная расчета RMS тока
	float Unow = 0;   		// переменная расчета RMS напряжения

	uint16_t Pnow;
	uint16_t Pset = 0;
	uint16_t Pmax = 0;
	
	volatile static uint32_t CounterZC[3];
	volatile static uint32_t CounterTR[3];

	uint32_t CounterRMS = 0;

	volatile static int16_t Xnow;
	volatile static uint32_t X2;
	volatile static uint16_t Angle; 
	
	//void init(float Iratio, float Uratio);
	//void init(float Iratio, float Uratio, bool NeedCalibrate);
	//void init(float Iratio, float Uratio, uint8_t phaseN);	// 3-phase
	void init(uint16_t* pAngle, bool NeedCalibrate);			// 3-phas
	void init(uint16_t* pAngle);			// 3-phase
	
	void control();
	void check();
	void stop();
	void setpower(uint16_t setP);
	void printConfig(uint8_t i);
	void calibrate();
	void calibrate(uint16_t Scntr);
	void setRMScorrection(float *pIcorr, float *pUcorr);
	//=== Прерывания
	

	//static void CloseTriac_int(); //__attribute__((always_inline));
	// === test

	volatile static uint32_t _Icntr;
	volatile static uint32_t _Ucntr;
	
	uint8_t PinTriac;

protected:
	static void ZeroCross_int0();
	static void OpenTriac_int0(); 
	static void ZeroCross_int1();
	static void OpenTriac_int1(); 
	static void ZeroCross_int2();
	static void OpenTriac_int2(); 
	static void GetADC_int(); 
	
	static hw_timer_t* timerTriac[3];
	static uint8_t _pinTriac[3];
	uint8_t _pinZCross[3];
	volatile static uint32_t _msZCmillis[3];
	
	void setup_ZeroCross(uint8_t i);
	void setup_Triac(uint8_t i);
	void setup_ADC();
	void correctRMS();
	uint16_t get_ZeroLevel(uint8_t z_pin, uint16_t Scntr);
	
	int16_t _angle = 0;
	uint8_t _phaseQty;
	float _Uratio;
	float _Iratio;
	
	bool _ShowLog;
	bool _corrRMS = false;
	float *_pUcorr = NULL, *_pIcorr = NULL;
	
	hw_timer_t* timerADC = NULL;
	
	volatile static SemaphoreHandle_t smphRMS;
	static portMUX_TYPE muxADC;
	
	volatile static bool getI;
	volatile static bool takeADC;

	volatile static uint8_t _zero;
	volatile static uint8_t _pin;
	static uint8_t _pinI;
	static uint8_t _pinU;
	
	volatile static uint16_t* _pAngle;
	volatile static uint64_t _summ;
	volatile static uint64_t _I2summ;
	volatile static uint64_t _U2summ;

	volatile static uint32_t _cntr;

	volatile static uint16_t _zerolevel;
	static uint16_t _Izerolevel;
	static uint16_t _Uzerolevel;



};

#endif // ESP32

#endif //ACpower3_h
