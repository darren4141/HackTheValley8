//---SETUP---

//GENERAL
#include <Arduino.h>
#include <math.h>

//HUMIDITY & TEMPERATURE SENSOR
#include "DHT_Async.h"
#define DHT_SENSOR_TYPE DHT_TYPE_11
static const int DHT_SENSOR_PIN = 4;
DHT_Async dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

//LCD SCREEN
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//8X8 MATRIX
#include "LedControlMS.h"
 //pin 8 is connected to the DataIn
 //pin 10 is connected to the CLK
 //pin 9 is connected to LOAD
#define NBR_MTX 1
LedControl lc=LedControl(8,10, 9, NBR_MTX);//

//MOISTURE SENSOR
int Moisture_signal = A3;

//LIGHT SENSOR
int pinLDR=A0;

//VAR
static double moisture;
static float temperature;
static float humidity;
static double light;

static int rowsT;
static double rowsM;
static int rowsH;
static double rowsL;

static int count = 0;
static bool swap = true; 

void setup() {
    Serial.begin(9600); // Start the serial communication
    lcd.begin();
	  // Turn on the blacklight and print a message.
	  lcd.backlight();
	  lcd.print("Welcome");
    lcd.setCursor(0,1);
    lcd.print("Botanical Bestie");
    for (int i=0; i< NBR_MTX; i++){
        lc.shutdown(i,false);
        lc.setIntensity(i,8);
    }
    lcLogo();
    delay(1000);
}
 
void loop(){
  //read
  if (measure_environment(&temperature, &humidity)) {
    moisture = analogRead(Moisture_signal);
    light = analogRead(pinLDR);
    //send to python code
    serialSend();

    //timer swap between stat mode and cat mode
    if(swap){
      statmode();
      displayChart();
      count = count + 1;
      if(count > 2){
        swap = false;
        count = 0;
      }
    }else{
      charactermode();
      displayChart();
      count = count + 1;
      if(count > 2){
        swap = true;
        count = 0;
      }
    }
  }
}

void statmode() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("T: ");
    lcd.print((int)temperature, 1);
    lcd.print("C");
        
    lcd.setCursor(0,1);
    lcd.print("H: ");
    lcd.print((int)humidity, 1);
    lcd.print("%");

    lcd.setCursor(9, 0);
    lcd.print("M: ");
    lcd.print((int)moisture);

    lcd.setCursor(9, 1);
    lcd.print("L: ");
    lcd.print((int)light);
}

void displayChart(){
    lc.clearDisplay(0);
    
    //CALCULATIONS FOR BAR GRAPH. FULL BAR IS IDEAL
    //((value-min)/(max-min))*8
    rowsT = (temperature/26)*8;
    rowsM = 8-(((moisture-300)/724)*8);
    rowsH = ((humidity-10)/50)*8;
    rowsL = 8-(((light-400)/624)*8);

    //out
    for(int i = 0; i < rowsT; i++){
      lc.setLed(0, 0, i, 1);
      lc.setLed(0, 1, i, 1);
    }
    for(int i = 0; i < rowsM; i++){
      lc.setLed(0, 2, i, 1);
      lc.setLed(0, 3, i, 1);
    }
    for(int i = 0; i < rowsH; i++){
      lc.setLed(0, 4, i, 1);
      lc.setLed(0, 5, i, 1);        
    }
    for(int i = 0; i < rowsL; i++){
      lc.setLed(0, 6, i, 1);
      lc.setLed(0, 7, i, 1);        
    }
}

void charactermode(){
  int mood = rowsT + rowsM + rowsH + rowsL;
  cat();
  
  if(mood > 24){
    excitedFace();
  }else if(mood > 16){
    neutralFace();
  }else if(mood > 8){
    sleepyFace();
  }else{
    angryFace();
  }

}

void serialSend(){//for python
    Serial.print(temperature);
    Serial.print(",");
    Serial.print(moisture);
    Serial.print(",");
    Serial.print(humidity);
    Serial.print(",");
    Serial.print(light);
    Serial.println();
}

static bool measure_environment(float *temperature, float *humidity) {
    static unsigned long measurement_timestamp = millis();

    if (millis() - measurement_timestamp > 2000ul) {//2 second delay between reads
        if (dht_sensor.measure(temperature, humidity)) {
            measurement_timestamp = millis();
            return (true);
        }
    }

    return (false);
}

void cat() {
  lcd.clear();

  byte image30[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B00000};
  byte image31[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B00000};
  byte image16[8] = {B00000, B01000, B11000, B01000, B01000, B01000, B00100, B00100};
  byte image15[8] = {B00000, B00000, B00000, B00001, B00010, B11100, B00000, B00000};
  byte image14[8] = {B00000, B00000, B00000, B00000, B10000, B01111, B00000, B00000};
  byte image13[8] = {B00000, B00100, B00110, B00101, B00100, B00100, B01000, B01000};
  byte image29[8] = {B10000, B11100, B10000, B11100, B01000, B00110, B00001, B00000};
  byte image32[8] = {B00010, B01111, B00010, B01111, B00100, B11000, B00000, B00000};

  lcd.createChar(0, image30);
  lcd.createChar(1, image31);
  lcd.createChar(2, image16);
  lcd.createChar(3, image15);
  lcd.createChar(4, image14);
  lcd.createChar(5, image13);
  lcd.createChar(6, image29);
  lcd.createChar(7, image32);

  lcd.setCursor(13, 1);
  lcd.write(byte(0));
  lcd.setCursor(14, 1);
  lcd.write(byte(1));
  lcd.setCursor(15, 0);
  lcd.write(byte(2));
  lcd.setCursor(14, 0);
  lcd.write(byte(3));
  lcd.setCursor(13, 0);
  lcd.write(byte(4));
  lcd.setCursor(12, 0);
  lcd.write(byte(5));
  lcd.setCursor(12, 1);
  lcd.write(byte(6));
  lcd.setCursor(15, 1);
  lcd.write(byte(7));
}

void neutralFace() {
  byte image30[8] = {B10000, B11000, B00001, B00110, B00000, B00000, B11111, B00000};
  byte image31[8] = {B00100, B00110, B00000, B11000, B00000, B00000, B11111, B00000};

  lcd.createChar(0, image30);
  lcd.createChar(1, image31);

  lcd.setCursor(13, 1);
  lcd.write(byte(0));
  lcd.setCursor(14, 1);
  lcd.write(byte(1));

}

void sleepyFace() {
  byte image30[8] = {B00000, B11000, B00001, B00110, B00000, B00000, B11111, B00000};
  byte image31[8] = {B00000, B00110, B00000, B11000, B00000, B00000, B11111, B00000};

  lcd.createChar(0, image30);
  lcd.createChar(1, image31);

  lcd.setCursor(13, 1);
  lcd.write(byte(0));
  lcd.setCursor(14, 1);
  lcd.write(byte(1));
}

void excitedFace() {
  byte image30[8] = {B10000, B11000, B00001, B00111, B00001, B00000, B11111, B00000};
  byte image31[8] = {B00100, B00110, B00000, B11000, B10000, B00000, B11111, B00000};

  lcd.createChar(0, image30);
  lcd.createChar(1, image31);

  lcd.setCursor(13, 1);
  lcd.write(byte(0));
  lcd.setCursor(14, 1);
  lcd.write(byte(1));
}

void angryFace() {
  byte image30[8] = {B00000, B11000, B01000, B00001, B00011, B00000, B11111, B00000};
  byte image31[8] = {B00000, B00110, B00010, B00000, B10000, B00000, B11111, B00000};
  byte image14[8] = {B00000, B00000, B00000, B00000, B10000, B01111, B10000, B01000};
  byte image15[8] = {B00000, B00000, B00000, B00001, B00010, B11100, B00010, B00100};

  lcd.createChar(0, image30);
  lcd.createChar(1, image31);
  lcd.createChar(2, image14);
  lcd.createChar(3, image15);

  lcd.setCursor(13, 1);
  lcd.write(byte(0));
  lcd.setCursor(14, 1);
  lcd.write(byte(1));
  lcd.setCursor(13, 0);
  lcd.write(byte(2));
  lcd.setCursor(14, 0);
  lcd.write(byte(3));
}

void lcLogo(){
  lc.setLed(0, 1, 2, 1);
  lc.setLed(0, 2, 1, 1);
  lc.setLed(0, 3, 1, 1);
  lc.setLed(0, 4, 1, 1);
  lc.setLed(0, 5, 1, 1);
  lc.setLed(0, 6, 2, 1);
  lc.setLed(0, 2, 4, 1);
  lc.setLed(0, 2, 5, 1);
  lc.setLed(0, 5, 4, 1);
  lc.setLed(0, 5, 5, 1);
}