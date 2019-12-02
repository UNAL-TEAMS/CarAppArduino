#include <SoftwareSerial.h>
#include <Button.h>
#include <TimerOne.h>
SoftwareSerial SerialESP8266(3,2); // RX, TX

#include "U8glib.h"
bool printMen=true;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);  // Dev 0, Fast I2C / TWI

//----BUTTON---
#include <Button.h>   

#define NONE 0
#define PRESS 1
String WiFiName = "";
String WiFiPass = "";
String plate = "";
#define HOST "192.168.43.124"


Button button(4, true, true, 25); 
//Button button(4);
uint8_t keyCode;


//----GENERAL----
unsigned long Km ; 


void sendToEsp8266(String toSend){
    SerialESP8266.println(toSend);
    delay(3000);
}

bool setupCard(){
  //Verificamos si el ESP8266 responde
  sendToEsp8266("AT");
  if(!SerialESP8266.find("OK")){
    Serial.println("ERROR WITH ESP8266");
    return false;  
  }
  Serial.println("Conection with ESP8266");
  sendToEsp8266("AT+CWMODE=1");
  if(!SerialESP8266.find("OK")){
    Serial.println("Error on CWMODE");
    return false;  
  }
  return true;
}

void setupWifi(){  
  
  sendToEsp8266("AT+CWJAP=\"" + WiFiName + "\",\"" + WiFiPass  + "\"");
  if(!SerialESP8266.find("OK")){
    Serial.println("Error conecting to WIFI");
    return;  
  }
  Serial.println("Conected to wifi"); 
}

void sendRequestToServer(String placa, int km){
  sendToEsp8266("AT+CIPSTART=\"TCP\",\"" + String(HOST) + "\",12345");
  if(!SerialESP8266.find("OK")){
    Serial.println("Error conectiong to TCP");
    return;  
  }
  Serial.println("Conected to TCP");
  String req = "GET /device/km/" + placa + "/" + km + " HTTP/1.1\nHost: " + String(HOST) + "\n\n";
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
  
  u8g.setFont(u8g_font_osr29);
  //u8g.setFont(u8g_font_9x18);
//button.begin();
  keyCode = NONE;

  //Timer1.initialize(10000000);//timing for 1000 ms
//  Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR
  

}
void printStatus(){
  Serial.print("Wifi: ");
  Serial.println(WiFiName);
  Serial.print("Contraseña: ");
  Serial.println(WiFiPass);
  Serial.print("Placa: ");
  Serial.println(plate);
}
void printPlateKm(){
  Serial.print("Placa: ");
  Serial.println(plate);
  Serial.print("Km: ");
  Serial.println(Km);
  
}
void printMenu(){
  Serial.println("Menu:");
  Serial.println("1. Configurar WIFI");
  Serial.println("2. Configurar Placa");
  Serial.println("3. Imprimir Estado ");
  Serial.println("4. Empezar");
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
}

void loop() {
  if(printMen)printMenu();
  static bool menu = true;
  while((!Serial.available())&&menu){};
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
      case '4':
      //Timer1.initialize(10000000);//timing for 1000 ms
  //  Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR
        menu=false;
      break;
    }
  }

  u8g.firstPage();
  do pageCount();
  while( u8g.nextPage() ); 

  readKeys();
  taskCount();
}

void readKeys() {
  if (!button.read()) keyCode = PRESS;
  else keyCode = NONE;  
}

void taskCount(){
  switch(keyCode){
    case PRESS: Km += 10;
  }
}

void pageCount(){
  u8g.setPrintPos(0, 40);
  u8g.print(Km);
}
