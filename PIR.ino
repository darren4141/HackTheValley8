int sensorPin = 11;
int relayPin = 12;

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  pinMode(relayPin, OUTPUT);
  lcd.begin();
	lcd.backlight();
  lcd.print("Hello");
  lcd.clear();
}

void loop() {
  int value = digitalRead(sensorPin);
  digitalWrite(relayPin, value);
  // lcd.setCursor(0,0);
  // lcd.print("D: ");
  // lcd.print(value);
  // Serial.print(value);
}