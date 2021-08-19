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

#include "esp32-adc-nowait.h"

#if defined(ESP32)

<<<<<<< Updated upstream
#define LIBVERSION "ACpower3_v20210319 " 

#define ZC_CRAZY		// если ZeroCross прерывание выполняется слишком часто :-(
#define ZC_EDGE RISING	// FALLING, RISING

#define ADC_RATE 400    // количество отсчетов АЦП на ПОЛУволну - 200 (для прерываний)
#define ADC_WAVES 8    // количество обсчитываемых ПОЛУволн 
#define ADC_NOISE 20	// попробуем "понизить" шум АЦП
#define ADC_COUNT (ADC_RATE * ADC_WAVES)	// количество отсчетов после которого пересчитываем угол
#define ADC_I_RATIO 0.02	// значение по умолчанию
#define ADC_U_RATIO 0.2 	// значение по умолчанию

//#define U_ZERO 1931     //2113
//#define I_ZERO 1942     //1907

// default PINs config
// phase 0
#define PIN_ZC0 25  // детектор нуля
#define PIN_TR0 26  // триак
#define PIN_I0 36  // датчик тока
#define PIN_U0 39  // датчик напряжения
// phase 1
#define PIN_ZC1 14  // детектор нуля ??
#define PIN_TR1 27  // триак 
#define PIN_I1 32  // датчик тока
#define PIN_U1 33  // датчик напряжения
// phase 2
#define PIN_ZC2 13  // детектор нуля
#define PIN_TR2 12  // триак ??
#define PIN_I2 34  // датчик тока
#define PIN_U2 35  // датчик напряжения

#define ANGLE_MIN 1000		// минимальный угол открытия - определяет MIN возможную мощность
#define ANGLE_MAX 10100		// максимальный угол открытия триака - определяет MAX возможную мощность
#define ANGLE_DELTA 100		// запас по времени для открытия триака
#define ACPOWER3_MAX 3000		// больше этой мощности установить не получится
=======
#define ACPOWER3_LIBVERSION "ACpower3_v20210709 " 

#define ACPOWER3_ZC_CRAZY		// если ZeroCross прерывание выполняется слишком часто :-(
#define ACPOWER3_ZC_EDGE FALLING	// FALLING, RISING

#define ACPOWER3_ADC_NOISE 20	// попробуем "понизить" шум АЦП

#define ACPOWER3_ADC_RATE 400    // количество отсчетов АЦП на ПОЛУволну - 200 (для прерываний)
#define ACPOWER3_ADC_WAVES 10    // количество обсчитываемых ПОЛУволн 
#define ACPOWER3_ADC_SAMPLES (ACPOWER3_ADC_RATE * ACPOWER3_ADC_WAVES)	// количество отсчетов после которого пересчитываем угол
#define ACPOWER3_ADC_DONE (ACPOWER3_ADC_SAMPLES + 1)
#define ACPOWER3_ADC_NEXT (ACPOWER3_ADC_SAMPLES + 5)
#define ACPOWER3_ADC_START (ACPOWER3_ADC_SAMPLES + 10)
#define ACPOWER3_ADC_TIMER 3		// номер таймера для АЦП
//define TRIAC_TIMER 0	// в 3-х фазной версии для управления триаками используются таймеры 0, 1, 2

//#define ACPOWER3_ADC_I_RATIO 0.02	// значение по умолчанию
//#define ACPOWER3_ADC_U_RATIO 0.2 	// значение по умолчанию

// default PINs config
// phase 0
#define ACPOWER3_PIN_ZC0 25  // детектор нуля
#define ACPOWER3_PIN_TR0 26  // триак
#define ACPOWER3_PIN_I0 36  // датчик тока
#define ACPOWER3_PIN_U0 39  // датчик напряжения
// phase 1
#define ACPOWER3_PIN_ZC1 14  // детектор нуля ??
#define ACPOWER3_PIN_TR1 27  // триак 
#define ACPOWER3_PIN_I1 32  // датчик тока
#define ACPOWER3_PIN_U1 33  // датчик напряжения
// phase 2
#define ACPOWER3_PIN_ZC2 13  // детектор нуля
#define ACPOWER3_PIN_TR2 12  // триак ??
#define ACPOWER3_PIN_I2 34  // датчик тока
#define ACPOWER3_PIN_U2 35  // датчик напряжения

#define ACPOWER3_ANGLE_MIN 1000		// минимальный угол открытия - определяет MIN возможную мощность
#define ACPOWER3_ANGLE_MAX 10100		// максимальный угол открытия триака - определяет MAX возможную мощность
#define ACPOWER3_ANGLE_DELTA 100		// запас по времени для открытия триака
#define ACPOWER3_ANGLE_MIDDLE 5000	// экспериментально...
#define ACPOWER3_MAX 3500		// больше этой мощности установить не получится
>>>>>>> Stashed changes
#define ACPOWER3_MIN 150		// минимально допустимая устанавливаемая мощность (наверное можно и меньше)

//efine TIMER_TRIAC 0			// в 3-х фазной версии для управления триаками используются таймеры 0, 1, 2
#define TIMER_ADC 3				// номер таймера для АЦП
#define ZEROLEVEL_SAMPLES 32000	// количество отсчетов для определения "нулевого" уровня
#define ZEROLEVEL_DELAY 20		// интервал в микросекундах между отсчетами при определении "нулевого" уровня

//#define DEBUG0
#define DEBUG1
//#define DEBUG2
	
class ACpower3
{
public:
	ACpower3(); // default PINs config will be used. see #define above.
	
	ACpower3(uint8_t pinZC0, uint8_t pinTR0, uint8_t pinI0, uint8_t pinU0, \
	 		 uint8_t pinZC1, uint8_t pinTR1, uint8_t pinI1, uint8_t pinU1, \
			 uint8_t pinZC2, uint8_t pinTR2, uint8_t pinI2, uint8_t pinU2);
	
	ACpower3(uint8_t pinZC0, uint8_t pinTR0, uint8_t pinI0, uint8_t pinU0, \
	 		 uint8_t pinZC1, uint8_t pinTR1, uint8_t pinI1, uint8_t pinU1, \
			 uint8_t pinZC2, uint8_t pinTR2, uint8_t pinI2, uint8_t pinU2,
			 uint16_t pmax,	 bool useADC,	 bool showLog);
		 
	float I[3];   		// переменная расчета RMS тока
	float U[3];   		// переменная расчета RMS напряжения
	uint16_t P[3];		// мощность по каждой фазе
	uint16_t Pnow;		// суммарная мощность
	uint16_t Pavg;		// среднее двух измерений
	uint16_t Pset = 0;
	uint16_t Pmax = 0;
	
	volatile static uint32_t CounterZC[3];
	volatile static uint32_t CounterTR[3];
<<<<<<< Updated upstream

=======
	volatile static uint32_t CounterADC;	
>>>>>>> Stashed changes
	uint32_t CounterRMS = 0;
	
	String LibVersion = ACPOWER3_LIBVERSION;
	String LibConfig;

	volatile static int16_t Xnow;
	volatile static uint32_t X2;
	volatile static uint16_t Angle; 
	
	//void init(float Iratio, float Uratio, bool NeedCalibrate);
	//void init(float Iratio, float Uratio, uint8_t phaseN);	// 3-phase
	//void init(uint16_t* pAngle, bool NeedCalibrate);			// 3-phas
	void init();
	void init(float Iratio, float Uratio);
	
	void initADC();
	//void initADC(uint8_t pinI0, uint8_t pinU0, uint8_t pinI1, uint8_t pinU1, uint8_t pinI2, uint8_t pinU2);

	void control();					// 
	void control(uint16_t angle_);  // для "ручного" управления триаком - MIN=0, MAX=10000. Без стабилизации!!
	//void check();
	void stop();
	void setpower(uint16_t setP);
	void printConfig(uint8_t i);
	
	void setRMSzerolevel();
	void setRMSzerolevel(uint16_t Scntr);
	void setRMSratio(float Iratio, float Uratio);
	void setRMScorrection(float *pIcorr, float *pUcorr);
	//static void CloseTriac_int(); //__attribute__((always_inline));

	//volatile static 
	uint32_t _Icntr;
	//volatile static 
	uint32_t _Ucntr;
	
<<<<<<< Updated upstream
	volatile static uint32_t _cntr;
=======
>>>>>>> Stashed changes
	//uint8_t PinTriac;

protected:
	//=== Прерывания
	static void ZeroCross_int0();
	static void ZeroCross_int1();
	static void ZeroCross_int2();
	
	static void OpenTriac_int0(); 
	static void OpenTriac_int1(); 
	static void OpenTriac_int2(); 
	
	static void GetADC_int(); 
	
	static hw_timer_t* timerTriac[3];
	static uint8_t _pinTriac[3];
	
	uint8_t _pinZCross[3];
	volatile static uint32_t _msZCmillis[3];
	
	uint8_t _pinI[3];
	uint8_t _pinU[3];
	uint16_t _Izerolevel[3];
	uint16_t _Uzerolevel[3];
	
	void setup_ZeroCross(uint8_t i);
	void setup_Triac(uint8_t i);
	
	void setup_ADC();
	void correctRMS();
	uint16_t get_ZeroLevel(uint8_t z_pin, uint16_t Scntr);
	
	uint16_t Pprev = 0, Pold = 0;
	int16_t _angle = 0;
	//uint8_t _phaseQty;
	uint8_t _phase;		// current phase - ADC calculate THIS phase
	uint8_t _lag = 4;
	
	float _Uratio;
	float _Iratio;
	
	bool _ShowLog = true;
	bool _useADC = true;
	bool _corrRMS = false;
	
	float *_pUcorr = NULL, *_pIcorr = NULL;
	
	hw_timer_t* timerADC = NULL;
	
	volatile static SemaphoreHandle_t smphRMS;
	static portMUX_TYPE muxADC;
	
	volatile static bool getI;
	//volatile static bool takeADC;

	volatile static uint8_t _zero;
<<<<<<< Updated upstream
	volatile static uint8_t _pin;
=======
	volatile static uint8_t _pin;		// current pin - ADC collect U/I on THIS pin
	volatile static uint8_t _phase;		// current phase - ADC calculate THIS phase
>>>>>>> Stashed changes
	
	//volatile static uint16_t* _pAngle;
	volatile static uint64_t _summ;
	volatile static uint64_t _I2summ;
	volatile static uint64_t _U2summ;
<<<<<<< Updated upstream

	//volatile static uint32_t _cntr;
=======
>>>>>>> Stashed changes
	volatile static uint16_t _zerolevel;

	void log_cfg(String str0);
	void log_cfg(String str0, uint16_t num1);
	void log_cfg_ln(String str0);
	void log_cfg_f(String str0, String str1);
	void log_cfg_f(String str0, uint16_t num1);

};

#endif // ESP32

#endif //ACpower3_h
