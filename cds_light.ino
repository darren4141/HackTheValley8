int pinLDR=A0;
int val=0;
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(){
	//initialize communication at 9600 bits per second
	Serial.begin(9600);
	lcd.begin();
  	// Turn on the blacklight and print a message.
  	lcd.backlight();
  	lcd.print("Hello");
	lcd.clear();
}
	
void loop() {
	val= analogRead(pinLDR);
	//print the value on the serial monitor
	//Go to Tools->Serial Monitor to see the values
	Serial.println(val);
	lcd.setCursor(0, 0);
	lcd.print(val);
	delay(1000);	
}