#include <SoftwareSerial.h>
SoftwareSerial SerialESP8266(3,2); // RX, TX

#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);  // Dev 0, Fast I2C / TWI

//----BUTTON---
#include <Button.h>   

#define NONE 0
#define PRESS 1

#define HOST "192.168.43.124"

Button button(4);
uint8_t keyCode;


//----GENERAL----
unsigned long count; 


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

void setupWifi(String wifiName, String password){  
  
  sendToEsp8266("AT+CWJAP=\"" + wifiName + "\",\"" + password  + "\"");
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
  setupWifi("FLDSMDFR", "2334445555");
  sendRequestToServer("GKX037", 600);
  /*
  u8g.setFont(u8g_font_osr29);
  //u8g.setFont(u8g_font_9x18);
  button.begin();
  keyCode = NONE;

  count = 0;*/

}

void loop() {}

void readKeys() {
  if (!button.read()) keyCode = PRESS;
  else keyCode = NONE;  
}

void taskCount(){
  switch(keyCode){
    case PRESS: count += 10;
  }
}


void pageCount(){
  u8g.setPrintPos(0, 40);
  u8g.print(count);
}
