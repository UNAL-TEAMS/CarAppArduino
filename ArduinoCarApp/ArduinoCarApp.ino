#include <SoftwareSerial.h>
#include <Button.h>
#include <TimerOne.h>
SoftwareSerial SerialESP8266(3,2); // RX, TX

#include "U8glib.h"
bool printMen=true;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);  // Dev 0, Fast I2C / TWI

//----BUTTON---
#include <Button.h>   

#include <EEPROM.h>
#include <Arduino.h>  // for type definitions

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}


template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}

#define NONE 0
#define PRESS 1
String WiFiName = "Ivan Q";
String WiFiPass = "HideOnBush99";
String plate = "GKX037";
#define HOST "192.168.0.3"


Button button(4, true, true, 25); 
//Button button(4);
uint8_t keyCode;


//----GENERAL----

unsigned long Km =0; 
unsigned long lastMillis =0; 
bool isRunning=false;


void sendToEsp8266(String toSend){
    SerialESP8266.println(toSend);
    delay(3000);
    /*String readString1 = ""; 
    while (SerialESP8266.available()) {
      delay(3);  //delay to allow buffer to fill
      if (SerialESP8266.available() >0) {
        char c = SerialESP8266.read();  //gets one byte from serial buffer
        readString1 += c; //makes the string readString
      }
    }
    Serial.println(readString1);
*/}

bool setupCard(){
  //Verificamos si el ESP8266 responde
  sendToEsp8266("AT");
  if(!SerialESP8266.find("OK")){
    Serial.println("ERROR WITH ESP8266");
    return false;  
  }
  //Serial.println("Conection with ESP8266");
  sendToEsp8266("AT+CWMODE=1");
  if(!SerialESP8266.find("OK")){
    Serial.println("Error on CWMODE");
    return false;  
  }
  return true;
}

void setupWifi(){  
  
  sendToEsp8266("AT+CWJAP=\"" + WiFiName + "\",\"" + WiFiPass  + "\"");
  
  //while(!SerialESP8266.available()){};
  if(!SerialESP8266.find("OK")){
    Serial.println("Error conecting to WIFI");
    return;  
  }
  //EEPROM_writeAnything(0,WiFiName);
  //EEPROM_writeAnything(50,WiFiPass);
  Serial.println("Conected to wifi"); 
}

void sendRequestToServer(){
  sendToEsp8266("AT+CIPSTART=\"TCP\",\"" + String(HOST) + "\",12345");
  if(!SerialESP8266.find("OK")){
    Serial.println("Error conection to TCP");
     
  }
  Serial.println("Conected to TCP");
  String req = "GET /device/km/" + String(plate) + "/" + Km + " HTTP/1.1\nHost: " + String(HOST) + "\n\n";
  Serial.println("Sending:\n" + req + "\nlen: " + String(req.length()));
  
  sendToEsp8266("AT+CIPSEND=" + String(req.length()));
  if(!SerialESP8266.find(">")){
    Serial.println("Error sending size");
    return;  
  }
  sendToEsp8266(req); 
}

void setup() {
  SerialESP8266.begin(9600);
  Serial.begin(9600);
  SerialESP8266.setTimeout(2000);
  setupCard();
  
  //u8g.setFont(u8g_font_osr29);
  u8g.setFont(u8g_font_9x18);
  //button.begin();
  keyCode = NONE;

  //EEPROM_readAnything(0,WiFiName);
  //EEPROM_readAnything(50,WiFiPass);
  //EEPROM_readAnything(100,plate);

  //Timer1.initialize(10000000);//timing for 1000 ms
  //Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR
}
void readKeys() {
  button.read();
  if (button.wasPressed()) isRunning = !isRunning;  
}

void taskCount(){
  if (isRunning){
    int sensorRead=analogRead(A0);
    int value=map(sensorRead,0,1023,0,10);
      Km+=value;
  }
}

void pageCount(){
  u8g.setPrintPos(45, 36);
  u8g.print(Km);
}
void printStatus(){
  Serial.print("Wifi: ");
  Serial.println(WiFiName);
  Serial.println("Contraseña: ");
  //Serial.println(WiFiPass);
  Serial.print("Placa: ");
  Serial.println(plate);
}
void printMenu(){
  Serial.println("Menu:");
  Serial.println("1. Configurar WIFI");
  Serial.println("2. Configurar Placa");
  Serial.println("3. Imprimir Estado ");
  Serial.println("4. Test Wifi");
  Serial.println("5. Empezar");
  printMen=false;
}
void setWifi(){
  Serial.print("Wifi: ");
  Serial.readString();
  while(!Serial.available()){};
  WiFiName=Serial.readString();
  WiFiName.trim();
  Serial.print(WiFiName);
  Serial.print("\nPass: ");
  while(!Serial.available()){};
  WiFiPass=Serial.readString();
  WiFiPass.trim();
  Serial.print(WiFiPass);
  Serial.println("");
  
  setupWifi();
}
void setPlate(){
  Serial.print("Placa: ");
  Serial.readString();
  while(!Serial.available()){};
  plate = Serial.readString();
  plate.trim();
  Serial.println(plate);
  Serial.read();
  
  //EEPROM_writeAnything(100,plate);
}

void loop() {

    /*if (SerialESP8266.available()) Serial.write(SerialESP8266.read());
  if (Serial.available()) SerialESP8266.write(Serial.read());
  */
  if(printMen)printMenu();
  static bool menu = true;
  while(menu && (!Serial.available())){};
  if(menu){ 
    switch(Serial.read()){
      
      case '1':
        setWifi(); 
        printMen=true;
        break;
      case '2':
        setPlate();
        printMen=true;
      break;
      case '3':
        printStatus();
        printMen=true;
      break;
      case '5':
      
      menu=false;
      break;
      case '4':
        setupWifi();
        printMen=true;
      break;
    }
  }
  if(!menu && millis()-lastMillis>20000){
    
    sendRequestToServer(); 
    lastMillis=millis();
  }

  u8g.firstPage();
  do pageCount();
  while( u8g.nextPage() ); 

  readKeys();
  taskCount();
}
