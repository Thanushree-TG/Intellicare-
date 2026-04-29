#include <LiquidCrystal.h>
// LCD pins: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);  // Change these pins according to your wiring

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 15
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperature;

char mystr[20];
char tempStr[8];

int flux1 = 36;
int flux2 = 39;
int flux3 = 34;
int flux4 = 35;

#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1000

//int Buzzer=PB12;

//// Timer variables
//unsigned long lastTime = 0;
//unsigned long timerDelay = 30000;

float BPM, SpO2,BP;

PulseOximeter pox;
uint32_t tsLastReport = 0;


// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;


void Init_spo2();

void onBeatDetected()
{
  Serial.println("Beat Detected!");
}


// ADXL345 Accelerometer
#include <Adafruit_Sensor.h> 
#include <Adafruit_ADXL345_U.h>
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
String motionDetected = "Motion: Normal";

void Init_spo2();



#define sw1  4

void setup() {
  Serial.begin(9600);
   Serial2.begin(9600);
  
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("IntelliCare Band");
  lcd.setCursor(0, 1);
  lcd.print(" Device ");
  delay(1500);  
  
  pinMode(flux1, INPUT);
  pinMode(flux2, INPUT);
  pinMode(flux3, INPUT);
  pinMode(flux4, INPUT);
  pinMode(sw1, INPUT_PULLUP);
  
  lcd.clear();
  lcd.print("Initializing SPO2..");
  delay(500);
   if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {  
    Serial.println("SUCCESS");

    pox.setOnBeatDetectedCallback(onBeatDetected);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  if(!accel.begin())
  {
    lcd.clear();
    lcd.print("ADXL sensor found");
    delay(500);
    while(1);
  }
}

void Init_spo2()
{
   Serial.print("Initializing pulse oximeter..");
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");

    pox.setOnBeatDetectedCallback(onBeatDetected);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}

void loop() 
{
  // Check motion detection
  if(digitalRead(sw1) == LOW){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Command Mode");
    delay(1000);

    String cmd = "No commands";
    int flux1val = 0;
    int flux2val = 0;
    int flux3val = 0;
    int flux4val = 0;
    while(1)
    {
      flux1val = analogRead(flux1);
      flux2val = analogRead(flux2);
      flux3val = analogRead(flux3);
      flux4val = analogRead(flux4);
      
      if (flux1val <= 2000) {
        cmd = "I Need Water";
         lcd.clear();
      lcd.print(cmd);
      delay(2000);
        break;
      }
      if (flux2val <= 2000) {
        cmd = "I want GO Outside";
         lcd.clear();
      lcd.print(cmd);
      delay(2000);
        break;
      }
      if (flux3val <= 2000) {
        cmd = "I Need Medicine";
         lcd.clear();
      lcd.print(cmd);
      delay(2000);
        break;
      }
      if (flux4val <= 2000) {
        cmd = "Emergency";
         lcd.clear();
      lcd.print(cmd);
      delay(2000);
        break;
      }
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("F1:" + String(flux1val) + " F2:" + String(flux2val));
      lcd.setCursor(0, 1);
      lcd.print("F3:" + String(flux3val) + " F4:" + String(flux4val));
      delay(500);
      
     
    }
    Serial.println(String(flux1val) + "," + String(flux2val) + "," + String(flux3val) + "," + String(flux4val) + ",0,0,0," + String(cmd)+ "," + String(motionDetected));
    delay(500);
     Serial2.println(String(flux1val) + "," + String(flux2val) + "," + String(flux3val) + "," + String(flux4val) + ",0,0,0," + String(cmd)+ "," + String(motionDetected));
    delay(500);
  }
  
  BP_SPO2_Check();
  TEMPERATURE();
  checkMotion();
  Serial.println("0,0,0,0," + String(BP) + "," + String(SpO2) + "," + String(temperature) + ",No commands," + String(motionDetected));
  delay(500);
  Serial2.println("0,0,0,0," + String(BP) + "," + String(SpO2) + "," + String(temperature) + ",No commands," + String(motionDetected));
  delay(500);
}

void BP_SPO2_Check()
{
  
 int i=0;
Init_spo2();
    while(i<10000)
    {
        pox.update();
        BPM = pox.getHeartRate();
        SpO2 = pox.getSpO2();
      
        if (millis() - tsLastReport > REPORTING_PERIOD_MS)
        {
      
          Serial.print("BP: ");
          Serial.print(BPM);
          Serial.println("");
      
          Serial.print("SpO2: ");
          Serial.print(SpO2);
          Serial.println("%");

         
          lcd.clear();
          lcd.print("BP:");
          lcd.print(BPM);   
          lcd.setCursor(0,1);
          lcd.print("SpO2:");
          lcd.print(SpO2);   
          lcd.print("%");
      
//          Serial.println("*********************************");
//          Serial.println();
      
          tsLastReport = millis();
        }
//        delay(500);
        i++;
    }
  
   
    lastTime = millis();

}


void TEMPERATURE() 
{
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  
  lcd.clear();
  lcd.setCursor(0, 0);  // Ensure it starts at the beginning of line 0
  lcd.print("TEMP: " + String(temperature) + "C");
  delay(1000);
  dtostrf(temperature, 5, 3, tempStr);
}

void checkMotion() 
{
  sensors_event_t event; 
  accel.getEvent(&event);
  float X_val=event.acceleration.x;
  float Y_val=event.acceleration.y;
  float Z_val=event.acceleration.z;
  if((X_val<-7.50)||(X_val>7.5)||(Y_val>7.5)||(Y_val<-7.50))
  {
    motionDetected = "Motion: Detected"; 
  }else{
    motionDetected = "Motion: Normal";
  }
  lcd.clear();
  lcd.print(motionDetected);
  delay(1000);
}