/*
X-BUSSTOP-X ---> Bus Search with Bus Stop
USERID-P-X         ---> Bus Response Detected
USERID-Z-X         ---> Bus Detected / Bus Stop Detected
*/

#include <SD.h>
#define SD_ChipSelectPin 8
#include <TMRpcm.h>
#include <Keypad.h>

 char UserId = '2',RvId;
String string1, string2;
char BusStop,i=0,RxTxt[15],stat,BusResponse = 2;
char key1,key2,key3; 
boolean BusFlag = false,BusStopFlag = false;
unsigned long DelayTime;
unsigned int DelayTiming1,DelayTiming2,DelayTiming3;
const byte ROWS = 3; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'A','B','C'},
  {'D','E','F'},
  {'G','H','X'}//,
//  {'B','8','X'}
};
byte rowPins[ROWS] = {7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

TMRpcm tmrpcm;  
 
void setup()
{
  tmrpcm.speakerPin = 9; 
  Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin))
   {
//    Serial.println("SD fail"); 
 //   while(1);   // don't do anything more if not
   }
   else
//   Serial.println("SD Success");
  delay(1000);
}

void loop()
{
      key1 = keypad.getKey();
      if(key1)
       Serial.print(key1);

   if(key1 == 'X')
   {    
  do{
      key2 = keypad.getKey();
      if(key2)
      {
        Serial.print(key2);
        if(key2!='X')
        {
        BusStop=key2;
        }
      }
     }while(key2 != 'X');
//    BusStop[i-1]='\0';
//    string1 = BusStop;
//    i=0; 
  BusResponse = 0;
  BusFlag = false;
  BusStopFlag = false;
  DelayTime = millis();
   }

    if(Serial.available())
{      key3 = Serial.read();
//     Serial.println(key);
}

    if(key3 == UserId)
    { 
//  Serial.println("I'm Here");
    do
    {
      if(Serial.available())
      {
      key3 = Serial.read();
      if(key3)
      {
        if(key3!='X')
      stat=key3;
      }
       }
     }while(key3!='X');

     if(stat == 'P')
      {
//       Serial.println("Bus Response Detected!");
       BusResponse = 1;
      tmrpcm.play("welcome.wav");
      delay(3000);
      }
     else if(stat == 'Z')
     {
//       Serial.println("Bus Detected!");
       BusFlag = true;
       BusResponse = 1;       
     }
//     else if(stat == 'Y')
//      {
//        Serial.println("Bus Stop Detected!");
//        BusStopFlag = true;
//        BusResponse = 1;
//      }
     }



   
    if((millis() - DelayTime ) > 4000)
    {
      if(BusResponse == 0)
      {
      Serial.print('Y');
      delay(1000);
      Serial.print(BusStop);
      delay(1000);
//      i=0;
//      while(BusStop[i])
//      { 
//      Serial.print(BusStop[i]);
//      i++;
//      delay(1000);
//      }
      Serial.print(UserId);
      delay(1000);
      Serial.print('X');
      }
//      Serial.println('X'+string1+UserId+'X');
      DelayTime = millis();
    }
    
    if(BusFlag)
    {
      tmrpcm.play("welcome.wav");
      delay(3000);
      BusFlag = false;
    }
    
//    if(BusStopFlag)
//    {
//      tmrpcm.play("welcome.wav");
//      BusStopFlag = false;
//    }
}
