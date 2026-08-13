#include <Wire.h>

#include <SparkFun_Alphanumeric_Display.h>  //Click here to get the library: http://librarymanager/All#Alphanumeric_Display by SparkFun
HT16K33 display;

String titleString;
String durationString;
String padding = "                ";
String displayString;

void setup() {
  Serial.begin(115200);
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

  display.setBrightness(1);  //14

  // titleString = padding + "CURRENT MARRIAGE DURATION" + padding;
  // int titleStringLength = titleString.length();
  // for (int i = 0; i < titleStringLength - 15; i++) {
  //   displayString = titleString.substring(i, i + 16);
  //   display.print(displayString);
  //   delay(200);
//  }



}

void loop() {
  // displayDurationTitle("    IN SECONDS:");
  // displayDurationTitle("    IN MINUTES:");
  // displayDurationTitle("      IN HOURS:");
  // displayDurationTitle("       IN DAYS:");
  // displayDurationTitle("      IN WEEKS:");
  // displayDurationTitle(" IN FORTNIGHTS:");
  // displayDurationTitle("     IN MONTHS:");
  // displayDurationTitle("      IN YEARS:");
  // displayDurationTitle("    IN DECADES:");
  // displayDurationTitle("  IN CENTURIES:");
  // displayDurationTitle("   IN MILLENIA:");
  // displayDurationTitle("  CURRENT DATE:");
  // displayDurationTitle("   08-07-2026  ");
  displayDurationTitle("  CURRENT DATE:");
  displayDurationTitle(" 07-28-2028 UTC");
  displayDurationTitle("  CURRENT TIME:");
  displayDurationTitle("    12:34 UTC  ");


  titleString = padding + "CURRENT   MARRIAGE   DURATION" + padding;
  int titleStringLength = titleString.length();
  for (int i = 0; i < titleStringLength - 15; i++) {
    displayString = titleString.substring(i, i + 16);
    display.print(displayString);
    delay(100);
  }



  durationString = padding + "YOU HAVE BEEN MARRIED FOR 32-POINT-45632 WEEKS" + padding;
//  durationString = padding + "You have been married for 32-POINT-45632 weeks!!!" + padding;
  int durationStringLength = durationString.length();
  for (int i = 0; i < durationStringLength - 15; i++) {
    displayString = durationString.substring(i, i + 16);
    display.print(displayString);
    delay(100);
  }

  durationString = padding + "YOU HAVE BEEN MARRIED FOR 0-POINT-00032 CENTURIES" + padding;
//  durationString = padding + "You have been married for 32-POINT-45632 weeks!!!" + padding;
  durationStringLength = durationString.length();
  for (int i = 0; i < durationStringLength - 15; i++) {
    displayString = durationString.substring(i, i + 16);
    display.print(displayString);
    delay(100);
  }





}


void displayDurationTitle(String title) {
  display.print(title);
  delay (3000);
  display.clear();
  delay(500);
}

