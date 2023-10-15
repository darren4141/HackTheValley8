import cv2 as cv
import numpy as np

import openai

from gtts import gTTS
import os

import serial
from time import sleep

from twilio.rest import Client

arduino = serial.Serial('/dev/cu.usbserial-1410', 9600)
sleep(2)


import speech_recognition as sr
r = sr.Recognizer() 


classes = ["background", "person", "bicycle", "car", "motorcycle",
  "airplane", "bus", "train", "truck", "boat", "traffic light", "fire hydrant",
  "unknown", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse",
  "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "unknown", "backpack",
  "umbrella", "unknown", "unknown", "handbag", "tie", "suitcase", "frisbee", "skis",
  "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard",
  "surfboard", "tennis racket", "bottle", "unknown", "wine glass", "cup", "fork", "knife",
  "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog",
  "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed", "unknown", "dining table",
  "unknown", "unknown", "toilet", "unknown", "tv", "laptop", "mouse", "remote", "keyboard",
  "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "unknown",
  "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush" ]


colors = np.random.uniform(0, 255, size=(len(classes), 3))
cam = cv.VideoCapture(0)

pb  = 'frozen_inference_graph.pb'
pbt = 'ssd_inception_v2_coco_2017_11_17.pbtxt'

cvNet = cv.dnn.readNetFromTensorflow(pb,pbt)   


humidity = 9
temperature = 8
light = 7
moisture = 2


max = 8
min = 3
misfortunes = 0

# 3-8 is good
if(temperature<min):
   misfortunes+=1

elif(temperature>max):
    misfortunes+=1

if(light<min):
   light+=1

elif(light>max):
    light+=1

if(moisture<min):
   misfortunes+=1

elif(moisture>max):
    misfortunes+=1

clock=0

mood="happy"
if(misfortunes==1):
   mood="angry"
elif(misfortunes==2):
   mood="sad"
elif(misfortunes==3):
   mood="depressed"


language = 'zh-CN'

print("enter the character you want me to be: ")
character = str(input())

if character=="cat":
   language = 'zh-CN'
else:
   language = 'en'

messages = [ {"role": "system", "content": "You are plant that speaks in short sentences and is given the personality of a " + character + ". You are a needy plant and want to ask your human for help to keep your stats high. Your name is Mao, act as if you are talking to a good friend. You can make up any past experience. You are meant to only have brief discussions, don't act like an assistant. Make sure to keep your response below 25 words. If your stats are too low, ask for help from your human"} ]

response = ""

openai.api_key = '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'



def getLatestStatus():
    while arduino.inWaiting() > 0:
        status = arduino.readline().decode('utf-8').strip()
    return status

def engageConversation(response):
    first = True
    while response!="bye":

        data = getLatestStatus()
            
            
        values = data.split(",")
        temperature = ((int(float(values[0])))/48)*10
        moisture = 10 - (int(float(values[1])))/100
        humidity = int(float(values[2]))/10
        light = (10-(((int(float(values[3]))/100)/11)*10))
        print(data)
        print(temperature, moisture, humidity, light)

        if first:
            if(response!=""):
                response = " holding " + response
                messages.append( {"role": "user", "content": "Your friend is holding a " + response+ ". Respond to a conversation based on your mood: " + mood + " Your mood is based off your stats"+""+" The current Stats at this moment out of 10 with 5 being normal: humidity " + str(humidity) + ", temperature " + str(temperature) + ", light " + str(light) + ", moisture" + str(moisture)+"don't mention the numbers and don't behave like an assistant."}, )
                first = False
            else:
                messages.append( {"role": "user", "content": "If they ask what you need or how you're feeling, mention your how they can help with your stats (without saying stats, only the actual stats themselves), and how they can help,such as watering the plant or moving to different environment (or anything else). You can also incorporate previous parts of the conversation but make sure to use the current stats.  Otherwise, Respond to a conversation based on your mood: " + mood + " Your mood is based off your stats"+" The current Stats at the moment out of 10 with 5 being normal: humidity, " + str(humidity) + ", temperature " + str(temperature) + ", light " + str(light) + ", moisture" + str(moisture)+"don't mention the numbers and don't behave like an assistant."}, )
                first = False
        else:
            messages.append( {"role": "user", "content": response}, )
 
        chat = openai.ChatCompletion.create( 
            model="gpt-3.5-turbo", 
            messages=messages,
        ) 
        reply = chat.choices[0].message.content 
        print(reply)

        tts = gTTS(text=reply, lang=language, slow=False)

        tts.save("temp.mp3")  # Save the generated speech as a temporary MP3 file
        os.system("afplay temp.mp3")  # Use the 'afplay' command to play the MP3 file

        os.remove("temp.mp3")

        while(1): 
	
            try:

                with sr.Microphone() as source2:
                    
                    r.adjust_for_ambient_noise(source2, duration=0.2)
                    audio2 = r.listen(source2)
                    
                    # Using google to recognize audio
                    MyText = r.recognize_google(audio2)
                    MyText = MyText.lower()
                    
                    response = MyText
                    print(response)
                    break
                    
            except sr.RequestError as e:
                print("Could not request results; {0}".format(e))
                
            except sr.UnknownValueError:
                print("unknown error occurred")

   

while True:
  ret_val, img = cam.read()
  rows = img.shape[0]
  cols = img.shape[1]
  cvNet.setInput(cv.dnn.blobFromImage(img, size=(300, 300), swapRB=True, crop=False))

  cvOut = cvNet.forward()

  seen =[]
  master = False

  for detection in cvOut[0,0,:,:]:

    score = float(detection[2])
    if score > 0.66:
        idx = int(detection[1])   # prediction class index. 
        if classes[idx]!="person":
            seen.append(classes[idx])
        if classes[idx] == "person":
            master = True
        
        left = detection[3] * cols
        top = detection[4] * rows
        right = detection[5] * cols
        bottom = detection[6] * rows
        cv.rectangle(img, (int(left), int(top)), (int(right), int(bottom)), (23, 230, 210), thickness=2)
            

        label = "{}: {:.2f}%".format(classes[idx],score * 100)
        y = top - 15 if top - 15 > 15 else top + 15
        cv.putText(img, label, (int(left), int(y)),cv.FONT_HERSHEY_SIMPLEX, 0.5, colors[idx], 2)

  cv.imshow('my webcam', img)
  if(master):

    clock+=1
    #send a promp to openAI here

    if(clock>20):
        print("I see you are holding ", seen)
        my_string = ', '.join(map(str, seen))
        if(mood=="thirsty"):
            if "bottle" in seen:
                print("I see you are holding a waterbottle. Is that for me? I'm pretty thirsty")
        else:
           engageConversation(my_string)
           #clock=0
           break #start engaging in conversation than CV
  else:
    clock=0
        

    
  if cv.waitKey(1) == 27: 
    break 
cam.release()
cv.destroyAllWindows()
