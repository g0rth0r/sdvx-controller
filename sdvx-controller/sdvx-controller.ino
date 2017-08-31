#include <Mouse.h>
#include <Keyboard.h>
#include <Encoder.h>

Encoder myEncRight(2, 4);//Right Encoder
Encoder myEncLeft(3, 5);// Left Encoder
int latchPin = 8;
int dataPin = 9;
int clockPin = 7;

//Define variables to hold the data 
//for shift register.
//starting with a non-zero numbers can help

byte switchVar1 = 72;  //01001000
char button[] = {
  'a', 'b', 'c', 'd', 'l', 'r', 's','p'}; //array of char to send
boolean holding[] = {
  false,false,false,false,false,false,false,false}; //current holding button array
boolean lastHolding[] = {
  false,false,false,false,false,false,false,false}; //holded buttons during last array

long oldPositionX  = -999;
long oldPositionY  = -999;
long deltaX = 0;
long deltaY = 0;
long timer = millis();

void setup() {
  //start serial
  //Serial.begin(9600);
  //while (!Serial) ;
  Keyboard.begin();
  Mouse.begin();
  //define pin modes
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, INPUT);
  
}

void loop() {

  //Pulse the latch pin:
  digitalWrite(latchPin,1);//set it to 1 to collect parallel data
  delayMicroseconds(20);
  digitalWrite(latchPin,0);//set it to 0 to transmit data serially  
  switchVar1 = shiftIn(dataPin, clockPin,MSBFIRST);//shiftin the data and store 
  //Serial.println(switchVar1, BIN);//REMOVE FOR FINAL VERSION****************************
  
  
/************************************************************************Button Check Section*/
  for (int n=0; n<=7; n++)//read trough all 7 positions
  {
    if (switchVar1 & (1 << n) ){
      if(!holding[n]){//press if we were not holding before
        Keyboard.press(button[n]); //press the nth button
        holding[n] = true; //holding the n button
      }
    }
    else{
      if(holding[n]){
        Keyboard.release(button[n]);
      }   //if were holding before, but not anymore, release.
      holding[n] = false;//not holding anymore
    }
  }
  
/*********************************EncoderSection**************************************************/

  long newPositionX = myEncRight.read();//Reads right encodder
  long newPositionY = myEncLeft.read();//Reads left encoder
  
  
  if (newPositionX != oldPositionX) {//update delta and new position
    deltaX += newPositionX - oldPositionX;
    oldPositionX = newPositionX;
  }
  if (newPositionY != oldPositionY) {//update delta and new position
    deltaY += newPositionY - oldPositionY;
    oldPositionY = newPositionY;
  }

  if (millis() - timer > 10){ //refresh rate at which we send data to the mouse
    int deltaXM = map(deltaX,-500,500,-100,100);//sensitivity map 1/5 factor.
    int deltaYM = map(deltaY,-500,500,-100,100);
    if(deltaXM != 0 || deltaYM != 0){//only send position data if the mouse moved.
      Mouse.move(deltaXM,deltaYM);
    }
    
    deltaX = 0 ; //reset delta
    deltaY = 0;
    timer = millis();

  delay(5);
}
}
//byte shiftIn(int myDataPin, int myClockPin) {
// 
//  byte myDataIn = 0;  
//  for (int i = 7; i >= 0; i--)
//  {
//    digitalWrite(myClockPin, 0);                //pulsing the clockPin to read consequent bits/inputs of 4021
//    delayMicroseconds(1);                     //the responsiveness of 4021
//    if (digitalRead(myDataPin)) {               //if i-th input of 4021 is 1
//      myDataIn = myDataIn |(1 << i);            //sets i-th bit of myDataIn byte to 1
//    }
//    digitalWrite(myClockPin, 1);
//  }
// 
//  return myDataIn;
//}



