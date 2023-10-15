import serial

arduino = serial.Serial('COM12', 9600)

while True:
    data = arduino.readline().decode('utf-8').strip()
    values = data.split(",")