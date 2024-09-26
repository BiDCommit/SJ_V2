#include <Arduino.h>
/////////////////////////////////////////////////////////////
/////////// ПРОШИВКА ДЛЯ УМНОЙ ОДЕЖДЫ С ПОДОГРЕВОМ///////////
/////////////////////////////////////////////////////////////
#include "BluetoothSerial.h"//<--Подключение блютус библиотеки
#include <OneWire.h>
#include <DallasTemperature.h>
#include <GyverButton.h>

int HeatSense = 0;
// флаги для контроля зон прогрева
bool ZONE1 = false;
bool ZONE2 = false;
bool ZONE3 = false;
bool ZONE4 = false;
// флаг для котроля автопрогрева
bool AutoHeating = false;
//////////////
int ReleState1 = LOW;
int ReleState2 = LOW;
int ReleState3 = LOW;
int ReleState4 = LOW;
///////////////
long A = 5000;
long B = 10000;
long C = 15000;
long interval = A;
unsigned long previousMillis1 = 0 ;
unsigned long previousMillis2 = 0 ;
unsigned long previousMillis3 = 0 ;
unsigned long previousMillis4 = 0 ;
unsigned long previousMillis5 = 0;
/////////////////////////
unsigned long currentMillis1 = 0;
unsigned long currentMillis2 = 0;
unsigned long currentMillis3 = 0;
unsigned long currentMillis4 = 0;
unsigned long currentMillis5 = 0;
///////////////
// грудь 18
// плечи 5
//  спина 17
//  поясница 16
unsigned long currentMillisAuto = 0;



 void HeatZone1();
 void HeatZone2();
 void HeatZone3();
 void HeatZone4();
 void RelesOFF();

// подключение термодатчиков к esp32 для телеметрии 
#define ONE_WIRE_BUS1 4
OneWire oneWire1(ONE_WIRE_BUS1);
DallasTemperature sensors1(&oneWire1);
DeviceAddress insideThermometer;
#define BTN_PIN 2  // кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)
GButton butt1(BTN_PIN ,LOW_PULL, NORM_OPEN);
///////// светодиод для подключения к bluetooth 
struct Temp_DS18b20{
  byte TempInside;
  byte TempOutside;
  };


/////////
BluetoothSerial ESP_BT; // создиние объекта для Bluetooth
char incoming;//переменная для оправки
/// Статус подключения bluetooth 
bool BTConnection = false;
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
if(event == ESP_SPP_SRV_OPEN_EVT){
ESP_BT.print("Client Connected");
BTConnection=true;}
else {
BTConnection=false;}
}

void setup(){
  Serial.begin(115200); //скорость порта 115200
  ESP_BT.begin("SJ_V2"); //Название устройсва Bluetooth
  sensors1.begin();
  butt1.setDebounce(80);// 
  butt1.setTimeout(2000); 
        pinMode(18,OUTPUT);
        pinMode(5,OUTPUT);
        pinMode(17,OUTPUT);
        pinMode(16,OUTPUT);
  ESP_BT.register_callback(callback);
}

void HeatZone1(){ 
      currentMillis1=millis();
    if(currentMillis1-previousMillis1 >= interval){
      previousMillis1 = currentMillis1;
     if(ReleState1 == LOW){ReleState1 = HIGH;}
      else{ReleState1 = LOW;}
      digitalWrite(18, ReleState1);
   }
}
// метод таймера с поочередным прогревом плеч
void HeatZone2(){ 
     currentMillis2=millis();
    if(currentMillis2-previousMillis2 >= interval){
      previousMillis2 = currentMillis2;
     if(ReleState2 == LOW){ReleState2 = HIGH;}
      else{ReleState2 = LOW;}
      digitalWrite(5, ReleState2);
  }
}
// метод таймера с поочередным прогревом спина
void HeatZone3(){
      currentMillis3=millis();
    if(currentMillis3-previousMillis3 >= interval){
      previousMillis3 = currentMillis3;
     if(ReleState3 == LOW){ReleState3 = HIGH;}
      else{ReleState3 = LOW;}
      digitalWrite(17, ReleState3);
    }
  }
  // метод таймера с поочередным прогревом поясница
void HeatZone4(){ 
     currentMillis4=millis();
    if(currentMillis4-previousMillis4 >= interval){
      previousMillis4 = currentMillis4;
     if(ReleState4 == LOW){ReleState4 = HIGH;}
      else{ReleState4 = LOW;}
      digitalWrite(16, ReleState4);
    }
  }
  // метод выключения всех зон прогрева (портов)
void RelesOFF(){
  digitalWrite(18,LOW);
  digitalWrite(5,LOW);
  digitalWrite(17,LOW);
  digitalWrite(16,LOW);
}
      

void loop(){
Temp_DS18b20 Buf;
butt1.tick();
sensors1.requestTemperatures(); 
Buf.TempInside = sensors1.getTempCByIndex(0);
Buf.TempOutside = sensors1.getTempCByIndex(1);

ESP_BT.write((byte*)&Buf,sizeof(Buf));//отправляем температуру в цельсиях
  if (ESP_BT.available())//Проверяем, получаем ли мы что-нибудь по Bluetooth
 {
    incoming = ESP_BT.read(); //Считываем что мы получаем
    if (incoming == '1')//принимаем команду на включение первого реле 
        {ZONE1=true;}
    if (incoming == '0')//принимаем команду на выключение первого реле 
        {ZONE1=false;digitalWrite(18, LOW);}
    if (incoming == '2')//принимаем команду на включение второго реле
        {ZONE2=true;}
    if (incoming == '3')//принимаем команду на выключение второго реле
        {ZONE2=false;digitalWrite(5, LOW);}    
    if (incoming == '4')//принимаем команду на включение третьего реле
        {ZONE3=true;}
    if (incoming == '5')//принимаем команду на выключение третьего реле
        {ZONE3=false;digitalWrite(17, LOW);}    
    if (incoming == '6')//принимаем команду на включение четвертого реле
        {ZONE4=true;}
    if (incoming == '7')//принимаем команду на выключение четвертого реле
        {ZONE4=false;digitalWrite(16, LOW);}
    if(incoming =='8')// включение автопрогрева 
       {ZONE1=true; ZONE2=true; ZONE3=true; ZONE4=true; }
    if(incoming =='9')
       {ZONE1=false; ZONE2=false; ZONE3=false; ZONE4=false;
      RelesOFF();}
      /////////////// автивация умного прогрева
      if(incoming=='K'){
        AutoHeating = true;
        }
      if(incoming =='L'){
        AutoHeating = false;
        ZONE1=false; ZONE2=false; ZONE3=false; ZONE4=false;
        RelesOFF();
        }
        //////////// выбор времени прогрева А=10 сек. В=20сек. С=30сек.
      if(incoming =='A'){ 
       interval = A;
        }
      if(incoming =='B'){
       interval = B;
        }
      if(incoming =='C'){
         interval = C;
        }
        ///////////// выбор температуры для умного прогрева
      if(incoming =='D'){
        HeatSense = 25;
        }
      if(incoming=='E'){
        HeatSense = 26;
        }  
      if(incoming=='F'){
        HeatSense = 27;
        }
      if(incoming =='G'){
        HeatSense = 28;
        }
      if(incoming =='H'){
       HeatSense = 29;
        }
      if(incoming =='I'){
       HeatSense = 30;
        }
  }
   if(AutoHeating==true){
  if(Buf.TempInside < HeatSense && Buf.TempOutside < HeatSense ){
  ZONE1=true; ZONE2=true; ZONE3=true; ZONE4=true;
  }
  if(Buf.TempInside > HeatSense && Buf.TempOutside < HeatSense || 
  Buf.TempInside > HeatSense && Buf.TempOutside > HeatSense || 
  Buf.TempInside < HeatSense && Buf.TempOutside > HeatSense){
    ZONE1=false; ZONE2=false; ZONE3=false; ZONE4=false;
    RelesOFF();
    }
  }
      if(ZONE1==true && ReleState2==LOW && ReleState3==LOW && ReleState4==LOW){
HeatZone1();
      }
      if(ZONE2==true && ReleState1==LOW && ReleState3==LOW && ReleState4==LOW){
HeatZone2();
        }
      if(ZONE3==true && ReleState2==LOW && ReleState1==LOW && ReleState4==LOW){
HeatZone3();
        }
       if(ZONE4==true && ReleState2==LOW && ReleState3==LOW && ReleState1==LOW){
HeatZone4();
        }
        if(butt1.isPress()){ZONE1=true; ZONE2=true; ZONE3=true; ZONE4=true;}
        if(butt1.isDouble()){ ZONE1=false; ZONE2=false; ZONE3=false; ZONE4=false;
        RelesOFF();
  }      
}


