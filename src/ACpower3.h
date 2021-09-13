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

#include "esp32-adc-nowait.h"	// necessary with Arduino ESP32 >= 1.0.6

#if defined(ESP32)

#define ACPOWER3_LIBVERSION "ACpower3_v20210912 " 

#define ACPOWER3_ZC_CRAZY		// если ZeroCross прерывание выполняется слишком часто :-(
#define ACPOWER3_ZC_EDGE RISING	// FALLING, RISING

//#define ACPOWER3_ADC_TUNING		// попытка "привязать" старт сбора с АЦП к ZeroCross
#define ACPOWER3_ADC_NOISE 100	// попробуем "понизить" шум АЦП

#define ACPOWER3_ADC_RATE 200    // количество отсчетов АЦП на ПОЛУволну - 200 (для прерываний)
#define ACPOWER3_ADC_WAVES 4    // количество обсчитываемых ПОЛУволн 
#define ACPOWER3_RMS_LAG 4

//#define ACPOWER3_ADC_SAMPLES (ACPOWER3_ADC_RATE * ACPOWER3_ADC_WAVES)	// количество отсчетов после которого пересчитываем угол
#define ACPOWER3_ADC_SAMPLES _ADCsamples
//#define ACPOWER3_ADC_DONE (ACPOWER3_ADC_SAMPLES + 10)
//#define ACPOWER3_ADC_NEXT (ACPOWER3_ADC_SAMPLES + 99)
//#define ACPOWER3_ADC_START (ACPOWER3_ADC_SAMPLES + 1000)
#define ACPOWER3_ADC_DONE 59000
#define ACPOWER3_ADC_NEXT 64000
#define ACPOWER3_ADC_START 65000

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
#define ACPOWER3_MIN 150		// минимально допустимая устанавливаемая мощность (наверное можно и меньше)

#define ZEROLEVEL_SAMPLES 10000	// количество отсчетов для определения "нулевого" уровня
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
			 uint8_t pinZC2, uint8_t pinTR2, uint8_t pinI2, uint8_t pinU2,
			 uint16_t pmax = ACPOWER3_MAX);
		 
	float I[3];   		// переменная расчета RMS тока
	float U[3];   		// переменная расчета RMS напряжения
	uint16_t P[3];		// мощность по каждой фазе
	uint16_t Pnow;		// суммарная мощность
	uint16_t Pavg;		// среднее двух измерений
	uint16_t Pold = 0;	
	uint16_t Pset = 0;
	uint16_t Pmax = 0;
	bool ZC[3];
	
	volatile static uint32_t CounterZC_raw[3];
	volatile static uint32_t CounterZC[3];
	volatile static uint32_t CounterTR[3];
	volatile static uint32_t CounterADC;	
	uint32_t CounterRMS = 0;
	
	String LibVersion = ACPOWER3_LIBVERSION;
	String LibConfig;

	volatile static int16_t Xnow;
	volatile static uint32_t X2;
	volatile static uint16_t Angle; 
	
	void init(float Iratio, float Uratio, float *pIcorr, float *pUcorr); // all in one
	void initTR();
	void initZC(int zcIntMode = ACPOWER3_ZC_EDGE);
	void initADC(uint16_t ADCrate = ACPOWER3_ADC_RATE, uint16_t ADCwaves = ACPOWER3_ADC_WAVES);
	void setADCratio(float Iratio, float Uratio);
	void setRMScorrection(float *pIcorr, float *pUcorr);
	
	void control();					// 
	void control(uint16_t angle_);  // для "ручного" управления триаком - MIN=0, MAX=10000. Без стабилизации!!

	void stop();
	void setpower(uint16_t setP);
	void printConfig(uint8_t i);
	
	//static void CloseTriac_int(); //__attribute__((always_inline));

	//volatile static 
	uint32_t _Icntr;
	//volatile static 
	uint32_t _Ucntr;
	
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
	uint32_t _msCheckZC = 0;
	volatile static uint32_t _msZCross[3];
	volatile static uint32_t _ZCcntr[3];
	
	int _ZCmode;
	uint8_t _pinI[3];
	uint8_t _pinU[3];
	uint16_t _Izerolevel[3];
	uint16_t _Uzerolevel[3];
	
	void setup_ZeroCross(uint8_t i);
	void setup_Triac(uint8_t i);
	void setup_ADC(uint16_t ADCrate, uint16_t ADCwaves);
	void setup_ADCzerolevel(uint16_t Scntr);
	
	void correct_RMS();
	void check_ZC();
	
	uint16_t get_ZeroLevel(uint8_t z_pin, uint16_t Scntr);
	
	uint16_t Pprev = 0; //, Pold = 0;
	int16_t _angle = 0;
	float _lag = 5.0;
	
	float _Uratio;
	float _Iratio;
	
	bool _corrRMS = false;
	
	float *_pUcorr = NULL, *_pIcorr = NULL;
	
	hw_timer_t* timerADC = NULL;
	
	volatile static SemaphoreHandle_t smphRMS;
	static portMUX_TYPE muxADC;
	
	//volatile static bool getI;
	bool getI;
	//volatile static bool takeADC;

	volatile static uint8_t _zero;
	volatile static uint8_t _pin;		// current pin - ADC collect U/I on THIS pin
	volatile static uint8_t _phase;		// current phase - ADC calculate THIS phase
	
	//volatile static uint16_t* _pAngle;
	volatile static uint64_t _summ;
	volatile static uint64_t _I2summ;
	volatile static uint64_t _U2summ;
	volatile static uint16_t _zerolevel;
	volatile static uint16_t _ADCsamples;

	void log_cfg(String str0);
	void log_cfg(String str0, uint16_t num1);
	void log_cfg_ln(String str0);
	void log_cfg_f(String str0, String str1);
	void log_cfg_f(String str0, uint16_t num1);

};

#endif // ESP32

#endif //ACpower3_h
