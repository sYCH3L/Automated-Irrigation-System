#include "SoftwareSerial.h"
#include "SimpleTimer.h"
#include <stdio.h>
//Deklareerimine

SoftwareSerial btSerial(9,10);
SimpleTimer timer,tim;
int timerID, timerIDD;
int niiskustase = 0;
int timerr = 6000;
int pIrr = 0;
int isRegistered = 0;
String command;

enum CMD {HUMIDITY, REG, IRRIGATION, UNKNOWNN, INT, RESTART, NOTCON};

CMD getCommand(String command)
{
  if(command == "register"){
    return REG;
  }
  else if(command == "getHumidity"){
    return HUMIDITY;
  }
  else if(command == "irrigate"){
    return IRRIGATION;
  }
  else if(command == "restart"){
    return RESTART;
  }
  else if(command == "OK"){
    return NOTCON;
  }
  else if(command.indexOf("INT")>=0){//else if{(command.substring(0, 2) == "ARV")
    return INT;
  }
  else{
    return UNKNOWNN;
  }
}
void _setHumidityInterval(int newtime)
{
  timer.deleteTimer(timerID);
  timerID = timer.setInterval(newtime,sendHumidity);
}
void errorHandler(int err)
{
  char buf[10];
  snprintf(buf,10,"ERR:%d",err);
  btSerial.write(buf);
}
void sendHumidity()
{
  Serial.write("I'm in sendHumidity\r\n");
  char buf[10];
  snprintf(buf,10,"HUM:%d",analogRead(A0));
  btSerial.write(buf);
}
void _setTimer(int newtime)
{
   if(isRegistered)
   {
      timer.setTimeout(newtime,stopIrrigation);
   }
}
void stopIrrigation()
{
  if(isRegistered)
  {
     Serial.write("Im in Stop Irrigation\r\n");
     pIrr = 0;
     digitalWrite(13,LOW); 
  }
}
void resetFunc()
{
  if(isRegistered)
  {
     digitalWrite(4, LOW); 
  }
}

void Irrigate()
{
  if(!pIrr && isRegistered)
  {
    pIrr = 1;
    _setTimer(10000);
    digitalWrite(13,HIGH); 
  }
  else
  {
    errorHandler(1);
  }  
}

void registerProcess()
{
  btSerial.write("GER");
  isRegistered = 1;
  timerID = timer.setInterval(600000, sendHumidity);
}

void _setInterval()
{
  if(isRegistered)
  {
     timerr = command.substring(4, command.length()-2).toInt();
     _setHumidityInterval(timerr); 
  }
}

void _checkCon()
{
  btSerial.write("AT");
}

void _setCon()
{
  isRegistered = 0;
}

void setup() 
{
  //BT setup
  Serial.begin(9600); 
  btSerial.begin(9600); //?? 
  btSerial.write("AT+DEFA+ULT\r\n"); 
  btSerial.write("AT+RESET\r\n"); 
  btSerial.write("AT+NAMEKastmine\r\n"); 
  btSerial.write("AT+ROLE0\r\n");
  //Solenoid kinni
  pinMode(13, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(4,HIGH);
  digitalWrite(13,LOW);
  tim.setInterval(6000,_checkCon);
}

void loop() 
{
  timer.run();
  tim.run();
  niiskustase = analogRead(A0);
  if(btSerial.available())
  {
    command = btSerial.readString();
    command.remove(command.length()-2);
    switch(getCommand(command)){
      case REG:
        registerProcess();
        break;
      case HUMIDITY:
        sendHumidity();
        break;
      case IRRIGATION:
        Irrigate();
        break;
      case INT:
        _setInterval();
        break; 
      case RESTART:
        resetFunc();
        break;
      case NOTCON:
        _setCon();
        break;
        
    }
  }
}

