#include <require_cpp11.h>
#include <MFRC522.h>
#include <deprecated.h>
#include <MFRC522Extended.h>


//فراخوانی کتابخانه ها
#include "SevenSegmentTM1637.h"
#include <Servo.h>
#include <MFRC522.h> // کتابخانه RFID
#include <SPI.h> // پروتکل SPI


//تعریف پین ها
#define trigPin1 7
#define echoPin1 6
#define trigPin2 9
#define echoPin2 8
int Led1 = 0;
int Led2 = 1;
Servo myservo;

#define SS_PIN 10 // انتخاب پایه های فرمان RFID
#define RST_PIN 2 // انتخاب پایه های RFID
MFRC522 mfrc522(SS_PIN, RST_PIN); // فراخوانی متغیرهای RFID برای خواندن دیتای تگ ها 
#ifdef AVR  // پیاده سازی ترجمه ی شرطی دستورات
  #include <avr/power.h>
#endif


SevenSegmentTM1637    display(4, 5);

int count = 999;
int state = 0;
int flag = 0;
unsigned long r, x;

void setup() {
  //تنظیمات اولیه
  display.begin();
  display.print(count);
  myservo.attach(3);
  SPI.begin(); // فعال شدن پین SPI
  mfrc522.PCD_Init(); 
//  Serial.println("RFID DOOR LOCK");
  //ما در اینجا سروو را در وضعیت اولیه 90 درجه تنظیم کرده ایم، شما روی درجه دلخواه خود تنظیم کنید
  myservo.write(90);
  Serial.begin (9600);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(Led1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(Led2, OUTPUT);
}

void loop() {
//  Serial.println("bitch");
  Serial.println(state);
  //چک کردن جهت حرکت با چک کردن ترتیب فعال شدن ماژول های التراسونیک
  switch (state) {
    case 0:
      r = millis();
      flag = 0;
      if (sersor1()) {
        if (sersor2()) {
          state=0;
        }else{
          state=1;
        }
      }
      if (sersor2()) {
        if (sersor1()) {
          state=0;
        }else{
          state=2;
        }
      }
      break;
    case 1:
      x = millis() - r;
      if (flag == 0){
        flag = check_card();
      }
//      Serial.println(flag);
      if (sersor2()==1 && flag) {
        Check(1);
        state=0;
      }
      if(sersor1()==0 && sersor2() == 0){
        state=0;
       
      }
      break;
    case 2:
      x = millis() - r;
      if (sersor1()==1 && x < 1500 && x > 100) {
        Check(2);
        delay(2000);
        state=0;
      }
      if(x > 1500){
        state=0;
      }
      break;
  }
}

void Check(int Check_state) {
  //انجام دستورات مربوط به جهت های مختلف
  switch (Check_state) {
    case 1:
      Serial.println("INSIDE WAY");
      count--;
//      Serial.println(count);
      display.print(count);
      //حالت ثانویه سروو روی 180 تنظیم شده ولی شما میتوانید تغییر دهید
      myservo.write(180);
      delay(2000);
      myservo.write(90);
      break;
    case 2:
      Serial.println("OUTSIDE WAY");
      if (count<999){
        count++;
//        Serial.println(count);
        display.print(count);
      }
      break;
  }
  delay (500);
}


//تابعهای فعال شدن ماژولهای التراسونیک در فاصله 5 تا 15 سانتی متری

int sersor1(){
  float duration1, distance1;
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn (echoPin1, HIGH);
  distance1 = (duration1 / 2) * 0.0344;
  if (distance1 >= 5 && distance1 <= 15) {
    return 1;
  }else{
    return 0;
  }
}

int sersor2(){
  float duration2, distance2;
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn (echoPin2, HIGH);
  distance2 = (duration2 / 2) * 0.0344;
  if (distance2 >= 5 && distance2 <= 15) {
    return 1;
  }else{
    return 0;
  }
}

int check_card(){
 if ( ! mfrc522.PICC_IsNewCardPresent() ){
  return 0;
 }
 if ( ! mfrc522.PICC_ReadCardSerial() ) {
  return 0;
 }
  Serial.println("Time to open");
  return 1;
}
