#include <Wire.h>

#include <SparkFun_Alphanumeric_Display.h>  //Click here to get the library: http://librarymanager/All#Alphanumeric_Display by SparkFun
HT16K33 display;

String titleString;

void setup() {
  Serial.begin(115200);
  Serial.println("Qwiic Alphanumeric examples");
  Wire.begin(); //Join I2C bus

  //check if displays will acknowledge
//  if (display.begin(0x70) == false)
//  if (display.begin(0x70, 0x71) == false)
//  if (display.begin(0x70, 0x71, 0x72) == false)
  if (display.begin(0x70, 0x71, 0x72, 0x73) == false)
  {
    Serial.println("Device did not acknowledge! Freezing.");
    while(1);
  }
  Serial.println("Displays acknowledged.");

  display.setBrightness(1);
//  display.setBlinkRate(2);

}

void loop() 
{
  titleString = "    CURRENT     ";
  display.print(titleString);
  delay(1000);
  titleString = "    MARRIAGE    ";
  display.print(titleString);
  delay(1000);
  titleString = "    DURATION    ";
  display.print(titleString);
  delay(1000);
}
