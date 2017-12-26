/* 
 *  PAD CAL V.1.0.0
 *  
 */

#include <Wire.h>
#include <SD.h>
#include "Sodaq_DS3231.h"

const int chipSelect = 53;
const int FSR_PIN[] = {A0, A1, A2, A3}; // Pin connected to FSR/resistor divider
// Measure the voltage at 5V and resistance of your 3.3k resistor, and enter
// their value's below:
int delayTime = 250;
char weekDay[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
int sequenceNumber = 0;
String fileName = "Results.csv"; //This is the File Name
File myFile;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  pinMode(53, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  checkSD();
  setExcelTable();
  //setTime(); //comment out if time does not need to bet set
}

void loop() {
  myFile = SD.open(fileName, FILE_WRITE);
  //get number of pins being used
  int numberOfPins = sizeof(FSR_PIN) / sizeof(FSR_PIN[0]);
  int fsrADC[numberOfPins];

  //record the time
  recordTime();
  
  //collect data from all analog sources
  for (int i = 0; i < numberOfPins; i++) {
    fsrADC[i] = analogRead(FSR_PIN[i]);
  }

  //convert and store the data
  for (int i = 0; i < numberOfPins; i++) {
    if (fsrADC[i] != 0) //If the analog reading is non-zero
    {
      // Use ADC reading to calculate voltage:

      float force;
      force = fsrADC[i]*.0746-37.881;

      debugPAD(i,force,numberOfPins); //uncomment if you want to debug
      myFile.print(String(force));
      myFile.print(",");
    }
  }
  //start a new row
  myFile.println("");
  myFile.close();
  delay(delayTime);
}

void checkSD() {
  Serial.print("Initializing SD Card...");
  //check to see if  SD card is present, if not - abort
  if (!SD.begin(4)) {
    Serial.println("Card failed, or not present.");
    exit;
  }
  Serial.println("Card Initialized");
  Serial.println("");
}

void setTime() {
  //Below Line Format: year, month, date, hour, min, sec and week-day(starts from 0 and goes to 6)
  DateTime dt(2017, 11, 27, 16, 30, 0, 1);
  rtc.setDateTime(dt);
}

void setExcelTable() {
  String TableElements[] = {"Sequence", "Day", "Month", "Date", "Year", "Hour", "Minute", "Second", "Temp F", "Sensor 1", "Sensor 2", "Sensor 3", "Sensor 4"};
  myFile = SD.open(fileName, FILE_WRITE);

  for (int i = 0; i < (sizeof(TableElements) / sizeof(TableElements[0])); i++) {
    myFile.print(TableElements[i]);
    myFile.print(",");
  }
  myFile.println("");
  myFile.close();
}

void recordTime() {
  DateTime now = rtc.now();  //get current date-time
  rtc.convertTemperature(); //convert current temperature into registers
  //store data
  myFile.print(sequenceNumber);
  myFile.print(",");
  myFile.print(weekDay[now.dayOfWeek()]);
  myFile.print(",");
  myFile.print(now.month(), DEC);
  myFile.print(",");
  myFile.print(now.date(), DEC);
  myFile.print(",");
  myFile.print(now.year(), DEC);
  myFile.print(",");
  myFile.print(now.hour(), DEC);
  myFile.print(",");
  myFile.print(now.minute(), DEC);
  myFile.print(",");
  myFile.print(now.second(), DEC);
  myFile.print(",");
  myFile.print(rtc.getTemperature() * 1.8 + 32);
  myFile.print(",");
  sequenceNumber = sequenceNumber + 1; 
}

void debugPAD(int SensorNumber, float reading, int numberOfSensors){
  Serial.print("Sensor ");
  Serial.print(SensorNumber+1);
  Serial.print(": ");
  Serial.println(String(reading));

  if (SensorNumber == numberOfSensors-1)
    Serial.println("");

      
  
}



