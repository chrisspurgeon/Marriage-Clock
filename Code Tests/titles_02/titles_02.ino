#include <Wire.h>

#include <SparkFun_Alphanumeric_Display.h>  //Click here to get the library: http://librarymanager/All#Alphanumeric_Display by SparkFun
HT16K33 display;

String titleString;
String padding = "                ";
String displayString;

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

  display.setBrightness(14);

  titleString = padding + "CURRENT MARRIAGE DURATION" + padding;
  int titleStringLength = titleString.length();
  for (int i = 0; i < titleStringLength - 15; i++) {
    displayString = titleString.substring(i, i + 16);
    display.print(displayString);
    delay(200);
  }



}

void loop() {
  titleString = padding + "CURRENT   MARRIAGE   DURATION" + padding;
  int titleStringLength = titleString.length();
  for (int i = 0; i < titleStringLength - 15; i++) {
    displayString = titleString.substring(i, i + 16);
    display.print(displayString);
    delay(100);
  }
  displayDurationTitle("    IN SECONDS:");
  displayDurationTitle("    IN MINUTES:");
  displayDurationTitle("      IN HOURS:");
  displayDurationTitle("       IN DAYS:");
  displayDurationTitle("      IN WEEKS:");
  displayDurationTitle(" IN FORTNIGHTS:");
  displayDurationTitle("     IN MONTHS:");
  displayDurationTitle("      IN YEARS:");
  displayDurationTitle("    IN DECADES:");
  displayDurationTitle("  IN CENTURIES:");
  displayDurationTitle("   IN MILLENIA:");
}


void displayDurationTitle(String title) {
  display.print(title);
  delay (3000);
  display.clear();
  delay(500);
}

