//Coded by Simon Jelley 
//Nixie clock 
//based on RTClib 3234 example
//Licence: Attribution-ShareAlike 4.0 International
//Contact Jellmeister on instructables.com

// Date and time functions using a DS3234 RTC connected via SPI

#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <RTC_DS3234.h>

// Avoid spurious warnings - Not sure what this does, but was in the example, can't hurt?!
#undef PROGMEM
#define PROGMEM __attribute__(( section(".progmem.data") ))
#undef PSTR
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];}))

//Pin for each anode seconds, minutes, hours - units, tens
uint8_t suPin = 4;
uint8_t stPin = 7;
uint8_t muPin = 3;
uint8_t mtPin = 6;
uint8_t huPin = 2;
uint8_t htPin = 5;
uint8_t DLS = 0;

// Create an RTC instance, using the chip select pin it's connected to
RTC_DS3234 RTC(10);

void setup () {
    pinMode(2, OUTPUT); 
    pinMode(3, OUTPUT); 
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT); 
    pinMode(6, OUTPUT); 
    pinMode(7, OUTPUT); 
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(14, OUTPUT); 
    pinMode(15, OUTPUT); 
    pinMode(16, OUTPUT); 
    pinMode(17, OUTPUT);
    pinMode(A5, INPUT);
    pinMode(A6, INPUT);
    pinMode(A7, INPUT);
    
    
    Serial.begin(57600);
    Serial.println("Nixieclock v1.3");
   
    SPI.begin();
    RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    Serial.print("Setting time to... ");
    Serial.print(__DATE__);
    Serial.print(' ');
    Serial.println(__TIME__);
  
  // following line sets the RTC to the date & time this sketch was compiled
  //  RTC.adjust(DateTime(__DATE__, __TIME__));
    
  }
}

void loop () {

    DateTime now = RTC.now();
    //BST adjustment logic
    if ((now.day() > 24 && now.dayOfWeek()==0 && now.month()==10 && now.hour()>0) || (now.day() > (now.dayOfWeek()+24) && now.dayOfWeek()>0 && now.month()==10) || (now.month()>10) || (now.day() > 24 && now.dayOfWeek()==0 && now.month()==3 && now.hour()<2) || (now.day() < (now.dayOfWeek()+25) && now.month()==3) || (now.month()<3))
    {
      DLS=0;//GMT
    }
    else
    {
      DLS=1;//BST
    }

    //split out each digit
    int ht=bin2bcd((now.hour()+DLS)%24)>>4;
    int hu=bin2bcd((now.hour()+DLS)%24)&B1111;
    int mt=bin2bcd(now.minute())>>4;
    int mu=bin2bcd(now.minute())&B1111;
    int st=bin2bcd(now.second())>>4;
    int su=bin2bcd(now.second())&B1111;
    
    Serial.print("(");
    Serial.print(ht, DEC);
    Serial.print(")(");
    Serial.print(hu, DEC);
    Serial.print("):(");
    Serial.print(mt, DEC);
    Serial.print(")(");
    Serial.print(mu, DEC);
    Serial.print("):(");
    Serial.print(st, DEC);
    Serial.print(")(");
    Serial.print(su, DEC);
    Serial.print(")");
    Serial.println(' ');
    Serial.println(' ');
    
    int dur = 2; //analogRead(A5)>>8;
    Serial.println(dur,DEC);
    //seconds units/tens
    outputBCDtoPin(((11-su)%10),suPin,dur); //bodged due to wiring backwards :(
    outputBCDtoPin(st,stPin,dur);
    //minutes units/tens
    outputBCDtoPin(mu,muPin,dur);
    outputBCDtoPin(mt,mtPin,dur);
    //hours units/tens
    outputBCDtoPin(hu,huPin,dur);
    outputBCDtoPin(ht,htPin,dur);
    
}

void outputBCDtoPin(unsigned int digitBCD, unsigned int pin, unsigned int milliduration)
{
  writeBCD(digitBCD);
  delayMicroseconds(190);
  digitalWrite(pin, HIGH); //activate anode
  delay(milliduration);
  digitalWrite(pin, LOW);//deactivate
  delayMicroseconds(10);  
}

void writeBCD(unsigned int digitBCD) // write BCD to A5-A2 pins
{
  
  for (int i=0; i<4; i++)
  {
    if ((digitBCD>>i) & B1)
    {
      digitalWrite((14+i), HIGH);
    }
    else
    {
      digitalWrite((14+i), LOW);
    }

  }

}
