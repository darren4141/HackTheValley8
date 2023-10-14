#include "LedControlMS.h"
 
 //pin 8 is connected to the DataIn
 //pin 10 is connected to the CLK
 //pin 9 is connected to LOAD
 
#define NBR_MTX 1
LedControl lc=LedControl(8,10, 9, NBR_MTX);//
 
void setup()
{
  for (int i=0; i< NBR_MTX; i++)
  {
    lc.shutdown(i,false);
    lc.setIntensity(i,8);
    lc.clearDisplay(i);
    delay(500);
  }
}
 
void loop()
 
{
     //lc.writeString(0,"HELLO JEREMY");//sending characters to display
     lc.setRow(0, 0, 1023);
     delay(1000);
}