#include <SoftwareSerial.h>
SoftwareSerial SerialESP8266(3,2); // RX, TX

#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);  // Dev 0, Fast I2C / TWI

//----BUTTON---
#include <Button.h>   

#define NONE 0
#define PRESS 1

Button button(4);
uint8_t keyCode;


//----GENERAL----
unsigned long count; 

void printSerial(String str){
  SerialESP8266.println(str);
  Serial.println(str);
  if (SerialESP8266.available()) {
    Serial.write(SerialESP8266.read());
  }
}

void setupWeb(){
  SerialESP8266.begin(9600);
  Serial.begin(9600);
  SerialESP8266.setTimeout(2000);
  
  //Verificamos si el ESP8266 responde
  printSerial("AT");
  printSerial("AT+CWMODE=1");  
  printSerial("AT+CWJAP=\"FLIASMARIAGUERR\",\"GNLCW151212**\"");
  SerialESP8266.setTimeout(10000); //Aumentar si demora la conexion
  printSerial("AT+CIPMUX=0");
  printSerial("AT+CIPSTART=\"TCP\",\"localhost\",12345");
  printSerial("AT+CIPSEND=83");
}

void setup() {
  /*setupWeb();
  
  u8g.setFont(u8g_font_osr29);
  //u8g.setFont(u8g_font_9x18);
  button.begin();
  keyCode = NONE;

  count = 0;*/
  SerialESP8266.begin(9600);
  Serial.begin(9600);
  SerialESP8266.setTimeout(2000);
}

void sendData(){
    /*String st = "GET /device/test/" + count ;
    st = st + " HTTP/1.1";
    printSerial("AT+CIPSEND=" + st.length());
    if(SerialESP8266.find(">")){
      printSerial(st);
    }*/
    
}

void loop() {

  if (SerialESP8266.available()) Serial.write(SerialESP8266.read());
  if (Serial.available()) SerialESP8266.write(Serial.read());
  /*u8g.firstPage();
  do pageCount();
  while( u8g.nextPage() ); 

  sendData();
  readKeys();
  taskCount();*/
}

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