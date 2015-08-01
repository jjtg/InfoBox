
#include <LiquidCrystal.h>
#include "DHT.h"

#define backlightPin A0
#define greenPin 5
#define redPin 6
#define btnPin1 2
#define btnPin2 3
#define DHTPIN 13
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int charCount = 0;
int charCountNews = 0;

long debouncing_time = 5;
volatile unsigned long last_micros;
long newsRefreshRate = 20000;
long weatherRefreshRate = 300000;
long newsMillis = 0;
long weatherMillis = 0;
long countMillis = 0; 

volatile int displayOn = 0;

char message[64];
char news[64];

boolean secondphasenews = false;
boolean secondphase = false;
boolean canPrint = false;
boolean canPrintNews = false;
boolean state = false;
boolean isNews = false;
//boolean displayOn = true;

void setup() {
  lcd.begin(16, 2);
  dht.begin();
  Serial.begin(9600);
  lcd.clear(); 
  pinMode(btnPin1, INPUT_PULLUP);
  pinMode(btnPin2, INPUT_PULLUP);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(backlightPin, OUTPUT);
  attachInterrupt(0, btnPress, FALLING);
  attachInterrupt(1, toggleDisplay, FALLING);

  analogWrite(backlightPin, 130);
  state = true;
  digitalWrite(redPin, HIGH);
  localData();
}

void loop() {
  countMillis = millis();
  //-----------------------------------------------------
//  volatile int btn1 = digitalRead(btnPin1);
//  //if((btn <1008)&&(btn > 100)){
//    if(btn1 == HIGH){
//      state = true;
//      digitalWrite(greenPin, LOW);
//      digitalWrite(redPin, HIGH);
//      if (canPrintNews){
//      displayMessage();
//      }
//  //  }else if (btn >1008){
//     }else if (btn2 == HIGH){
//      state = false;
//      digitalWrite(greenPin, HIGH);
//      digitalWrite(redPin, LOW);
//      if (canPrint){
//      displayMessage();
//      }
//    }
    //--------------------------------------------------  
  if (Serial.available()){
    delay(200);
      while (Serial.available() > 0) {
          char incomingByte = Serial.read();
          if (incomingByte == '^'){
              isNews = false;
              charCount = 0;
              break;
          } else if(incomingByte == '~'){
              isNews = true;
              charCountNews = 0;
              break;
          } else if(incomingByte == '{'){
            canPrint = true;
            break;
          } else if(incomingByte == '}'){
            canPrintNews = true;
            break;
          }       
      if (isNews == false){
       if (!charCount <= 63){
        if (!secondphase){
          message[charCount] = incomingByte;
          charCount++;
         }else if (secondphase){
          message[charCount] = incomingByte;
          charCount++;
         }
        } else {
          if (secondphase){
              secondphase = false;
            }else if (!secondphase){
              secondphase = true;
            }
            delay(200);
          message[charCount] = incomingByte;
        }
       canPrint = true;
     } else if (isNews){
         if (!charCountNews <= 63){
        if (!secondphasenews){
          news[charCountNews] = incomingByte;
          charCountNews++;
          newsRefreshRate = ((charCount * 200)*2);
         }else if (secondphasenews){
          news[charCountNews] = incomingByte;
          charCountNews++;
         }
        } else {
          if (secondphasenews){
              secondphasenews = false;
            }else if (!secondphasenews){
              secondphasenews = true;
            }
            delay(200);
          news[charCountNews] = incomingByte;
        }
       canPrintNews = true;
      }
    }
  }
    if (canPrint||canPrintNews){
      if (state){
        if ((countMillis - newsMillis) > newsRefreshRate){
            newsMillis = countMillis;
            displayMessage();
          }
      }else if (!state){
        if ((countMillis - weatherMillis) > weatherRefreshRate){
            weatherMillis = countMillis;
            displayMessage();
          }
      }
      }else if((countMillis%60000) <= 10){
      localData();
    }
  delay(10);
}

void displayMessage(){

int charSpeed = 200;
int weatherCharSpeed = 10;
int scrShiftSpeed = 200;
int i = 0;
int c = 0;

if (!state){
  int countDown = charCount; 

canPrint = false;
lcd.clear();

while (countDown != 0){
     if (c==15){  
        lcd.print(message[i]);
        lcd.setCursor(0,1);
        countDown--;
        i++;
        c++;
        delay(weatherCharSpeed);
      }else if(c==31){
        lcd.print(message[i]);

        delay(scrShiftSpeed);
        //lcd.clear();
        lcd.setCursor(0,0);
        countDown--;
        i++;
        c = 0; 
        delay(weatherCharSpeed);
    }else {
       lcd.print(message[i]);
  countDown --; 
  i++;
  c++;
  delay(weatherCharSpeed);
    }
  }
  
lcd.setCursor(0,0);
canPrint = true; 
  } else if(state){
  int countDown = charCountNews; 

canPrintNews = false;
lcd.clear();
while (countDown != 0){
  
     if (c==15){  
        lcd.print(news[i]);
        lcd.setCursor(0,1);
        countDown--;
        i++;
        c++;
        delay(charSpeed);
     }else if(c==31){
        lcd.print(news[i]);

        delay(scrShiftSpeed);
        lcd.clear();
        lcd.setCursor(0,0);
        countDown--;
        i++;
        c = 0; 
        delay(charSpeed);
    }else {
       lcd.print(news[i]);
       countDown --; 
       i++;
       c++;
       delay(charSpeed);
     }
    }
  
    lcd.setCursor(0,0);
    canPrint = true; 
  }
}
void btnPress(){
 // unsigned long micr = micros();
 // if((micr - last_micros) >= debouncing_time * 150){
  //  last_micros = micr;    
    volatile int  btn1 = digitalRead(btnPin1);
      if(btn1 != HIGH){
       if(!state){
        state = true;
        digitalWrite(greenPin, LOW);
        digitalWrite(redPin, HIGH);
        if (canPrintNews){
          displayMessage();
        } 
      } else if (state){
        state = false;
        digitalWrite(greenPin, HIGH);
        digitalWrite(redPin, LOW);
        if (canPrint){
          displayMessage();
        }
   //   }
    } 
  }

}

void toggleDisplay(){
  unsigned long micr = micros();
  //if((micr - last_micros) >= debouncing_time * 100){ //!! FIX
 //   last_micros = micr;    
    volatile int  btn2 = digitalRead(btnPin2);
    if(btn2 != HIGH){
       if (displayOn == 2){
        lcd.noDisplay();
        analogWrite(backlightPin, 0);
        displayOn = 0;
       } else if (displayOn == 1) {
        lcd.display();
        analogWrite(backlightPin, 0);  
        displayOn ++;
       } else if (displayOn == 0){
        lcd.display();
        analogWrite(backlightPin, 130);  
        displayOn ++;
        }
    }
 // }
}

void localData(){
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      lcd.print("No data to print");
      delay(1000);
      lcd.setCursor(0,0);
      lcd.print("Temp:    ");
      lcd.print(t);
      lcd.print(char(223));
      lcd.print("C");
      lcd.setCursor(0,1);
      lcd.print("Humidity: ");
      lcd.print(h);
      lcd.print("%");
      lcd.setCursor(0,0);
}

