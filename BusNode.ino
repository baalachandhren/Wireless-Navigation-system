/*
X-BUSSTOP-USERID-X ---> Bus Search with Bus Stop    EX: XAB000X ---> X (start)- AB00 (BusStopName) - 0 (UserId) - X (End)
USERID-P-X         ---> Bus Response Detected       EX: 0PX     ---> 0 (UserId) - P (Response from Bus)- X (End)
USERID-Z-X         ---> Bus Detected / Bus Stop Detected  :: Statement to Transmit from Bus Node    EX: 0ZX ---> 0 (UserId) - Z (Bus Stop Detected / to User) - X (End)
W-BUSSTOP-X        ---> Bus Stop ID :: Statement from Bus Stop to the Bus Node     EX: WAB00X ---> W(Msg from BusStop) - AB00 (BusStopName) - X (End)
*/

#define Buzzer 8

char StopName;
char BusStop[50];
int Id[50];

char BusStopName[]={'A','B','C','E','F','G','H','I'};
//String BusStopName[]={
//    "AB00","AB01",
//    "AB02","AB03",
//    "AB04","AB05",
//    "AB06","AB07"   };

int UserCnt=0,temp;
char key,RxTxt[10],i=0,j=0;    
char StopNo=0;

boolean Check(char Txt)
  {
     temp=8;
        do
        {
          if(Txt == BusStopName[temp-1])
           {
             return true;
             }
        temp--;
        }while(temp!=StopNo);
    
    return false;
  }

void CheckUserStop()
{
  temp  = UserCnt; 
  do
   {
     if(StopName == BusStop[temp-1])
     {
       Serial.print(Id[temp-1]);
       delay(1000);
       Serial.print('Z');
       delay(1000);
       Serial.print('X');
       delay(1000);
       digitalWrite(Buzzer,HIGH);
       delay(1000);
       digitalWrite(Buzzer,LOW);
     }
     temp--;
    }while(temp!=0);
 }

void setup()
{
Serial.begin(9600);
//Serial.println("Starts!");
pinMode(13,OUTPUT);
pinMode(Buzzer,OUTPUT);
}



void loop()
{
digitalWrite(13,HIGH);
      if(Serial.available())
        {
          key = Serial.read();
        
//          if(key == '1')
//          {
//            if(Serial.available())
//              key = Serial.read();
//            Serial.println("BusStop:");
//            Serial.println(BusStop[key-48]);
//            Serial.println("Id:");
//            Serial.println(Id[key-48]);
//          }
          
          if(key == 'Y')
          {
            digitalWrite(13,LOW);
                do{
                  if(Serial.available())
                  {
                    key=Serial.read();
                    if(key != 'X')
                    {
                    RxTxt[i]=key;
                    i++;
                    }
                  }
                  }while(key!='X');
      
                BusStop[UserCnt] = RxTxt[0];
                Id[UserCnt] = (int) (RxTxt[1]-48);
                i=0;
/*                    do{
                       if(Serial.available())
                        {
                        key = Serial.read();
                        RxTxt[i]=key;
                        i++;
                        }
                      }while(i<4);
                    RxTxt[i]='\0';
                    i=0;
                    BusStop[UserCnt] = RxTxt;
//                Serial.print("BusStop: ");
//                Serial.println(BusStop[UserCnt]);
            
            
                do{
                     if(Serial.available())
                      { 
                      key = Serial.read();
                      if(key)
                      {
                      RxTxt[i]=key;
                      i++;
                      }
                      }
        }while(key != 'X');
      
            RxTxt[i-1] ='\0';
            i=0;
            Id[UserCnt] = (int) (RxTxt[0]-48);*/
          
//                Serial.print("UserCnt:");
//                Serial.println(UserCnt);
//                Serial.print("UserId: ");
//                Serial.println(Id[UserCnt]);
//                Serial.print("BusStop: ");
//                Serial.println(BusStop[UserCnt]);

      if(Check(BusStop[UserCnt]))
      {
        delay(500);
      Serial.print(Id[UserCnt]);
      delay(1000);      
      Serial.print('P');
      delay(1000);
      Serial.print('X');
      delay(1000);
     digitalWrite(Buzzer,HIGH);
     delay(1000);
     digitalWrite(Buzzer,LOW);      
      UserCnt++;
      }
       }         
          
     if(key == 'W')
      {
        do{
          if(Serial.available())
          key = Serial.read();
          if(key != 'X')
          RxTxt[0]=key;
          }while(key!='X');
        /*  
        do
         {
           if(Serial.available())
            {
            key = Serial.read();
            RxTxt[i]=key;
            i++;
             }
          }while(key != 'X');
         RxTxt[i-1]='\0';*/
//         i=0;
         StopName = RxTxt[0];
         CheckUserStop();
         StopNo++;    // to prevent New User from registering the Bus Stop passed away
       }     
 }
}

