import serial

arduino = serial.Serial('COM12', 9600)

while True:
    data = arduino.readline().decode('utf-8').strip()
    #Serial.print(temperature + "," + moisture + "," + humidity + "," + light);
    values = data.split(",")