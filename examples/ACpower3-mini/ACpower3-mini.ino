// ТОЛЬКО для ESP32!!

// ** ВНИМАНИЕ ** будьте аккуратны - высокое напряжение опасно для жизни!

// подробности http://forum.homedistiller.ru/index.php?topic=166750.0
// каждую секунду в COM-порт показывается текущая и установленная мощность
// (при отсутствии нагрузки может выдавать ерунду :)
// для установки необходимой мощности нужно в COM-порт "дать команду" SPxxxx,
// где xxxx мощность в ваттах

#define SKETCHVERSION __FILE__ " " __DATE__ " " __TIME__
#define SERIALSPEED 115200
#define SHOWINTERVAL 1000
#define PRINTF(a, ...) Serial.print(a); Serial.println(__VA_ARGS__)

// ACpower3 PINs    ZC  TR  I   U
#define PINS_PHASE0 15, 25, 36, 39
#define PINS_PHASE1 14, 26, 32, 33
#define PINS_PHASE2 13, 27, 34, 35

// ACpower3 config
#define POWER_MAX 9000    // больше этой мощности установить не получится
#define ZCINT_MODE FALLING  // по какому фронту отрабатывать прерывание детектора нуля
#define ADC_RATE 200    // количество отсчетов АЦП на ПОЛУволну - 200-400 (для расчета прерываний)
#define ADC_WAVES 4     // количество обсчитываемых ПОЛУволн 
#define U_RATIO 0.1     // коэффициент для расчета напряжения
#define I_RATIO 0.02    // коэффициент для расчета тока
#define ANGLE_LAG 4     // "послабление" при расчете угла - чем больше тем плавнее изменение

#include "ACpower3.h"
ACpower3 TEH(PINS_PHASE0, PINS_PHASE1, PINS_PHASE2, POWER_MAX);

uint16_t inst_P = 0;  // глобальная переменная для хранения заданной мощности
                      // для установки необходимой мощности нужно в COM-порт "дать команду"
                      // SPxxxx, где xxxx мощность в ваттах  - см. chkSerial()
uint32_t msShow = 0;
String Stext, Svar;


void setup()
{
  Serial.begin(SERIALSPEED);
  delay(300);
  Serial.println(F(SKETCHVERSION));
  
  TEH.initTR();                       // один раз!
  TEH.initZC(ZCINT_MODE, false);      // один раз!
  TEH.initADC(ADC_RATE, ADC_WAVES);   // один раз!
  TEH.setADCratio(I_RATIO, U_RATIO, ANGLE_LAG); // можно запускать повторно для корректировки
}

void loop()
{
  //TEH.control(Angle); // желательно вызывать регулярно
  /*
     Так можно задавать угол открытия "вручную". Например для тестирования прибора.
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

  if ((millis() - msShow) > SHOWINTERVAL)
  {
    chkSerial();
    TEH.setpower(inst_P);
    showInfo();
    msShow = millis();
  }
}

void showInfo()
{
  PRINTF(" TEH.Pset: ", TEH.Pset);
  PRINTF(" TEH.Pnow: ", TEH.Pnow);
  PRINTF(" TEH.Angle: ", TEH.Angle);
  PRINTF(" TEH.CounterRMS: ", TEH.CounterRMS);
  TEH.CounterRMS = 0;

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
      }
      else if (Svar.substring(0, 3) == "RST")
      {
        ESP.restart();
      }
      
      Svar = "";
    }
  }
}
