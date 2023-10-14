#include <Arduino.h>
#include "DHT_Async.h"

#define DHT_SENSOR_TYPE DHT_TYPE_11
static const int DHT_SENSOR_PIN = 4;
DHT_Async dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(9600);
    lcd.begin();
	// Turn on the blacklight and print a message.
	lcd.backlight();
	lcd.print("Hello");
    lcd.setCursor(0,1);
    lcd.print("Moisture Sensor");
}

static bool measure_environment(float *temperature, float *humidity) {
    static unsigned long measurement_timestamp = millis();

    if (millis() - measurement_timestamp > 4000ul) {
        if (dht_sensor.measure(temperature, humidity)) {
            measurement_timestamp = millis();
            return (true);
        }
    }

    return (false);
}

void loop() {
    float temperature;
    float humidity;
    if (measure_environment(&temperature, &humidity)) {
        lcd.setCursor(0,0);
        lcd.print("T = ");
        lcd.print(temperature, 1);
        ldc.print(" deg. C");
        lcd.setCursor(0,1);
        lcd.print("H = ");
        lcd.print(humidity, 1);
        lcd.print("%")

        // Serial.print("T = ");
        // Serial.print(temperature, 1);
        // Serial.print(" deg. C, H = ");
        // Serial.print(humidity, 1);
        // Serial.println("%");
    }
}