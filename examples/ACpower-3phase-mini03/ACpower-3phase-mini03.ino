// ТОЛЬКО для ESP32!!

// ** ВНИМАНИЕ ** будьте аккуратны - высокое напряжение опасно для жизни!

// подробности http://forum.homedistiller.ru/index.php?topic=166750.0
// каждую секунду в COM-порт выдается текущая и установленная мощность
// (при отсутствии нагрузки может выдавать ерунду :)
// для установки необходимой мощности нужно в COM-порт "дать команду" SPxxxx,
// где xxxx мощность в ваттах

#define SKETCHVERSION __FILE__ " " __DATE__ " " __TIME__
#define MAXPOWER 3000
#define SERIALSPEED 115200
#define SHOWINTERVAL 1000
#define PRINTF(a, ...) Serial.print(a); Serial.println(__VA_ARGS__)

// Коэффициенты ниже справедливы только для Arduino c 10-битным АЦП и 5-вольтовым питанием!
// Коэффициенты датчика ACS712 |5А - 0.024414063 | 20А - 0.048828125 | 30A - 0.073242188 |

#include "ACpower3.h"

#if defined(ESP32)

// phase 0
#define PIN_ZC0 25  // детектор нуля
#define PIN_TR0 26  // триак
#define PIN_I0 36  // датчик тока
#define PIN_U0 39  // датчик напряжения
#define PINS_PHASE0 PIN_ZC0, PIN_TR0, PIN_I0, PIN_U0
// phase 1
#define PIN_ZC1 14  // детектор нуля ??
#define PIN_TR1 27  // триак 
#define PIN_I1 32  // датчик тока
#define PIN_U1 33  // датчик напряжения
#define PINS_PHASE1 PIN_ZC1, PIN_TR1, PIN_I1, PIN_U1
// phase 2
#define PIN_ZC2 13  // детектор нуля
#define PIN_TR2 12  // триак ??
#define PIN_I2 34  // датчик тока
#define PIN_U2 35  // датчик напряжения
#define PINS_PHASE2 PIN_ZC2, PIN_TR2, PIN_I2, PIN_U2

#else
#error "Chip not supported. Use ESP32."
#endif

ACpower3 TEH(PINS_PHASE0, PINS_PHASE1, PINS_PHASE2);

uint16_t inst_P = 0;
unsigned long msShow = 0;
String Stext, Svar;
uint16_t Angle = 0;
int msWait;

void setup()
{
  Serial.begin(SERIALSPEED);
  delay(300);
  Serial.println(F(SKETCHVERSION));
  TEH.init(); // = init() + initADC() + setRMSratio(0.02, 0.2)
  //ТЕН.setRMSratio(0.02, 0.2); // может понадобится если "зашитые" коэффициенты не подходят
  PRINTF(" _cntr2 ", TEH._cntr);
}

void loop()
{
  //TEH.control(Angle); // желательно вызывать регулярно
  /*
     Так можно задавать угол открыти "вручную". Например для тестирования прибора.
     Или когда измерением мощности занимается другой прибор.
     Angle=0 - миниммальная мощность, триак не открывается вообще
     Angle=10000 - максимальная мощность
     В этом случае мощность не стабилизируется и "плавает" с напряжением в сети.
  */

  TEH.control();  // нужно вызывать регулярно для пересчета мощности и угла открытия триака
  /*
    В этом случае для расчета используются измеренные значение токов и напряжений
    АЦП должно быть скофигурированно по initADC(...)
  */
  //msWait = random(90);
  //delay(msWait);

  if ((millis() - msShow) > SHOWINTERVAL)
  {
    chkSerial();
    showInfo();
    Angle = int(millis() / 10);
    msShow = millis();
  }
}

void showInfo()
{
  //PRINTF("&Angle=", (uint32_t)&Angle, HEX);
  PRINTF(" TEH.Pset: ", TEH.Pset);
  PRINTF(" TEH.Pnow ", TEH.Pnow);
  PRINTF(" TEH.Angle: ", TEH.Angle);
  PRINTF(" TEH.CounterRMS: ", TEH.CounterRMS);
  TEH.CounterRMS = 0;
  PRINTF(" TEH._cntr ", TEH._cntr);

  for (int i = 0; i < 3; i++)
  {
    Serial.print(i);
    PRINTF(" TEH.CounterZC: ", TEH.CounterZC[i]);
    TEH.CounterZC[i] = 0;
    PRINTF("  TEH.CounterTR: ", TEH.CounterTR[i]);
    TEH.CounterTR[i] = 0;

    PRINTF("  TEH.I: ", TEH.I[i]);
    PRINTF("  TEH.U: ", TEH.U[i]);
    PRINTF("  TEH.P: ", TEH.P[i]);
  }

  Serial.print("+++");
  Serial.println(millis());
}

void chkSerial()
{
  while (Serial.available()) //Serial port, пока не конец сообщения, читаем данные и формируем строку
  {
    char ch = Serial.read();
    Svar += ch;
    if (ch == '\n')
    {
      Svar.toUpperCase();
      
      if (Svar.substring(0, 2) == "SP")
      {
        Stext = Svar.substring(Svar.indexOf("SP", 2) + 3); //команда
        inst_P = Stext.toFloat();          //Выставленная мощность с Serial
        TEH.setpower(inst_P);
      }
      else if (Svar.substring(0, 3) == "RST")
      {
        ESP.restart();
      }
      
      Svar = "";
    }
  }
}
