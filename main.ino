#include <Arduino.h>
#include <math.h>

#include "DHT_Async.h"
#define DHT_SENSOR_TYPE DHT_TYPE_11
static const int DHT_SENSOR_PIN = 4;
DHT_Async dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include "LedControlMS.h"
 //pin 8 is connected to the DataIn
 //pin 10 is connected to the CLK
 //pin 9 is connected to LOAD
#define NBR_MTX 1
LedControl lc=LedControl(8,10, 9, NBR_MTX);//

int Moisture_signal = A3; //Define the Analog pin# on the Arduino for the soil moisture sensor signal

int pinLDR=A0;

void setup() {
    Serial.begin(9600); // Start the serial communication
    lcd.begin();
	  // Turn on the blacklight and print a message.
	  lcd.backlight();
	  lcd.print("Hello");
    lcd.setCursor(0,1);
    lcd.print("moisture Sensor");
    for (int i=0; i< NBR_MTX; i++){
        lc.shutdown(i,false);
        lc.setIntensity(i,8);
        lc.clearDisplay(i);
    }

    delay(1000);

}
 
void loop() {
    int moisture;
    float temperature;
    float humidity;
    int light;

    if (measure_environment(&temperature, &humidity)) {
        lcd.clear();

        moisture = analogRead(Moisture_signal);
        light = analogRead(pinLDR);
        lcd.setCursor(0,0);
        lcd.print("T: ");
        lcd.print(temperature, 1);
        lcd.print("C");

        lcd.setCursor(0,1);
        lcd.print("H: ");
        lcd.print(humidity, 1);
        lcd.print("%");

        lcd.setCursor(9, 0);
        lcd.print("M: ");
        lcd.print(moisture);

        lcd.setCursor(9, 1);
        lcd.print("L: ");
        lcd.print(light);
        
        //((value-min)/(max-min))*8

        int rowsT = round((double)((temperature/26)*8));
        // int rowsM = round((double)((moisture-1024)/(400-1024)*8));
        int rowsH = round((double)((humidity/60)*8));
        // int rowsL = round((double)((light-900)/(600-900)*8));
        
        double diffM = 1024-moisture;
        double diffL = 900-light;

        double rowsM = (double)((diffM/624)*8);
        double rowsL = (double)((diffL/400)*8);



        lc.clearDisplay(0);

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

        Serial.print(temperature);
        Serial.print(",");
        Serial.print(moisture);
        Serial.print(",");
        Serial.print(humidity);
        Serial.print(",");
        Serial.print(light);
    }
}

static bool measure_environment(float *temperature, float *humidity) {
    static unsigned long measurement_timestamp = millis();

    if (millis() - measurement_timestamp > 3000ul) {
        if (dht_sensor.measure(temperature, humidity)) {
            measurement_timestamp = millis();
            return (true);
        }
    }

    return (false);
}