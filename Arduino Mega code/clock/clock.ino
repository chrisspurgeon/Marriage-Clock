#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SparkFun_Alphanumeric_Display.h>
#include "RTClib.h"

#include <TimeLib.h>  // Include the Time library

// GPS wiring
// VIN gets 5V
// TX on the GPS unit goes to RX1 (PIN 19) on the MEGA
// RX on the GPS unit goes to TX1 (PIN 18) on the MEGA

// DISPLAY WIRING
// Black wire goes to GND
// Red wire goes to 5V
// Blue wire goes to SDA (PIN 20)
// Yellow wire goes to SCL (PIN 21)

// Create the Real Time Clock object.
// You may use RTC_DS1307 with the DS3231, DS1337, DS1340, Chronodot
// because those clock chips all use the same basic addresses.
RTC_DS1307 rtc;

// Create the GPS object
Adafruit_GPS GPS(&Serial1);
HardwareSerial mySerial = Serial1;

// Create the display object
HT16K33 display;


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences.
// Set DEBUGGER to 'true' if you want to see the date & time output.
#define GPSECHO false
#define GPSDEBUGGER false


// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean);  // Func prototype keeps Arduino 0023 happy


// Display variables
String textString;
String displayString;
String padding = "                ";  // 16 spaces
int textStringLength = 0;

// Brightness and speed variables
int brightnessPin = A0;  // select the input pin for the potentiometer
int speedPin = A1;       // select the input pin for the potentiometer
int brightness = 4;      // variable to store the value coming from the sensor
int speed = 500;         // variable to store the value coming from the sensor

// GPS and RTC status
int last_GPS_status = 0;
int current_GPS_status = 0;
int RTC_status = 0;


// DEBUGGER
bool DEBUGGER_FLAG = 1;





/* TIME VARIABLES */
uint32_t timer = millis();
float secondsLength = 1.0;
double minutesLength = 60.0;
double hoursLength = 3600.0;
double daysLength = 86400.0;
double weeksLength = 604800.0;
double fortnightsLength = 1209600.0;
double yearsLength = 31536000.0;
double decadesLength = 315360000.0;
double centuriesLength = 3153600000.0;


// Saturday, September 5, 2026, at 17:55:50 EDT converts to Saturday, September 5, 2026, at 21:55:50 UTC, which converts to 1788645350 unix time
unsigned long TIME_OF_MARRIAGE = 1788645350L;  // Saturday, September 5, 2026, at 21:55:50 UTC

unsigned long CURRENT_MARRIAGE_DURATION_IN_SECONDS;
unsigned long CURRENT_TIME;
double DURATION_IN_UNITS;
String DURATION_IN_UNITS_string;
float LEAP_YEAR_ADJUSTMENT;

// Get the current time from the clock. Later we'll check to make sure the clock has a reasonable time, that is after the wedding time.
DateTime RTC_now;

void setup() {

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  Serial.print("DEBUGGER IS ");
  Serial.println(DEBUGGER_FLAG);

  Serial.print("Time of marriage is ");
  Serial.println(TIME_OF_MARRIAGE);


  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);

  delay(2000);


  // SET UP DISPLAY CONNECTIONS
  Wire.begin();  //Join I2C bus

  //check if displays will acknowledge
  //  if (display.begin(0x70) == false)
  //  if (display.begin(0x70, 0x71) == false)
  //  if (display.begin(0x70, 0x71, 0x72) == false)
  if (display.begin(0x70, 0x71, 0x72, 0x73) == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1)
      ;
  }
  Serial.println("Displays acknowledged.");
  brightness = map(analogRead(brightnessPin), 0, 1024, 0, 10);
  display.setBrightness(brightness);  //14

  delay(1000);

  /*

  INITIAL WELCOME DISPLAY

  */

  textString = padding + "MARRIAGE CLOCK" + padding + "DESIGNED AND BUILT IN CALIFORNIA IN AUGUST 2026 BY CHRIS SPURGEON" + padding;
  textStringLength = textString.length();
  for (int i = 0; i < textStringLength - 15; i++) {
    displayString = textString.substring(i, i + 16);
    brightness = map(analogRead(brightnessPin), 0, 1024, 0, 16);
    speed = map(analogRead(speedPin), 0, 1024, 400, 45);
    display.setBrightness(brightness);  //14
    display.print(displayString);
    delay(speed);
  }
  for (int i = 0; i < 5; i++) {
    textString = "  INITIALIZING:";
    display.print(textString);
    delay(500);
    textString = "  INITIALIZING";
    display.print(textString);
    delay(500);
  }
}  // END OF setup()


/*

    GPS PROCESSING

*/

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;
      // writing direct to UDR0 is much much faster than Serial.print
      // but only one character can be written at a time.
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

/*

END OF INITIAL GPS PROCESSING

*/



void loop() {  // run over and over again

  // GPS stuff

  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (!usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    //  if (GPSECHO) {
    //    if (c) {
    //     Serial.print(c);
    //    }
    //  }
  }


  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA()))  // this also sets the newNMEAreceived() flag to false
      return;                        // we can fail to parse a sentence in which case we should just wait for another
  }
  // End of GPS stuff


  // Check the time
  RTC_now = rtc.now();  // Read the time and date from the clock chip
  CURRENT_TIME = (unsigned long)RTC_now.unixtime();

  if (CURRENT_TIME < TIME_OF_MARRIAGE) {
    RTC_status = 0;
  } else {
    RTC_status = 1;
  }


  // Get GPS fix status
  if ((int)GPS.fix != 0) {
    current_GPS_status = 1;
  }

  // If we don't have a valid time...
  if (RTC_status == 0 && current_GPS_status == 0) {
    displayMessage(padding + "WAITING FOR CLOCK SIGNAL" + padding, 1);
    displayMessage(padding + "THIS MAY TAKE AN HOUR OR EVEN MORE" + padding, 1);
    displayMessage(padding + "HAVE PATIENCE" + padding, 1);
    displayMessage(padding + "IF THIS MESSAGE CONTINUES FOR MORE THAN AN HOUR MOVE THE DEVICE CLOSER TO A WINDOW AND TRY AGAIN" + padding, 1);
    delay(2100);
  }

  //If we don't have a good clock time, but we DO have a GPS fix.
  if (current_GPS_status == 1 && last_GPS_status == 0 && RTC_status == 0) {
    displayMessage(padding + "UPDATED CLOCK SIGNAL ACQUIRED" + padding, 1);
    last_GPS_status = 1;

    // Let's wait to get a second GPS fix just to be sure.
    delay(3000);

    // Now, set the clock
    rtc.adjust(DateTime(2000 + GPS.year, GPS.month, GPS.day, GPS.hour, GPS.minute, GPS.seconds));
    RTC_now = rtc.now();  // Read the time and date from the clock chip
    CURRENT_TIME = (unsigned long)RTC_now.unixtime();

    RTC_status = 1;


    // DISPLAY INITIAL DATE AND TIME
    displayMessage("  CURRENT DATE:", 0);
    delay(2000);

    // DATE
    if (int(RTC_now.month()) < 10) {
      textString = "  0" + String(RTC_now.month()) + "-";
    } else {
      textString = "  " + String(RTC_now.month()) + "-";
    }
    if (int(RTC_now.day()) < 10) {
      textString += "0" + String(RTC_now.day()) + "-";
    } else {
      textString += String(RTC_now.day()) + "-";
    }
    textString += String(RTC_now.year()) + " UTC";

    displayMessage(textString, 0);
    delay(4000);

    // TIME
    displayMessage("  CURRENT TIME:", 0);
    delay(2000);

    if (int(RTC_now.hour()) < 10) {
      textString = "    0" + String(RTC_now.hour()) + ":";
    } else {
      textString = "    " + String(RTC_now.hour()) + ":";
    }
    if (int(GPS.minute) < 10) {
      textString += "0" + String(RTC_now.minute()) + " UTC";
    } else {
      textString += String(RTC_now.minute()) + " UTC";
    }

    displayMessage(textString, 0);
    delay(4000);
  }

  if (RTC_status == 1) {
    /*
      MAIN DISPLAY
      If we're here, we have the correct time and can start the duration displays.
  */


    if (DEBUGGER_FLAG) {
      Serial.println("DEBUGGER IS ON.");
      Serial.println("The date is " + String(RTC_now.month()) + " / " + String(RTC_now.day()) + " / " + String(RTC_now.year()));
      Serial.print("CURRENT_TIME is ");
      Serial.println(CURRENT_TIME);
      Serial.print("TIME_OF_MARRIAGE is ");
      Serial.println(TIME_OF_MARRIAGE);
    }

    /* SECONDS */
    CURRENT_MARRIAGE_DURATION_IN_SECONDS = CURRENT_TIME - TIME_OF_MARRIAGE;
    if (DEBUGGER_FLAG) {
      Serial.println("Marriage duration in seconds is " + String(CURRENT_MARRIAGE_DURATION_IN_SECONDS));
    }
    if (CURRENT_TIME > TIME_OF_MARRIAGE) {
      displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + String(CURRENT_MARRIAGE_DURATION_IN_SECONDS) + " SECONDS" + padding, 1);

      /* MINUTES */
      DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION_IN_SECONDS / minutesLength;
      DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 0);
      DURATION_IN_UNITS_string.replace(".", "-POINT-");
      displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " MINUTES" + padding, 1);

      /* HOURS */
      DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION_IN_SECONDS / hoursLength;
      DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 2);
      DURATION_IN_UNITS_string.replace(".", "-POINT-");
      displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " HOURS" + padding, 1);

      /* DAYS */
      DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION_IN_SECONDS / daysLength;
      DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
      DURATION_IN_UNITS_string.replace(".", "-POINT-");
      displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " DAYS" + padding, 1);

      /* WEEKS */
      DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION_IN_SECONDS / weeksLength;
      DURATION_IN_UNITS_string = String(DURATION_IN_UNITS);
      DURATION_IN_UNITS_string.replace(".", "-POINT-");
      displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " WEEKS" + padding, 1);

      /* FORTNIGHTS */
      DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION_IN_SECONDS / fortnightsLength;
      DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
      DURATION_IN_UNITS_string.replace(".", "-POINT-");
      displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " FORTNIGHTS" + padding, 1);

      /* YEARS */
      LEAP_YEAR_ADJUSTMENT = leapYearCheck(RTC_now.year(), RTC_now.month());
      if (DEBUGGER_FLAG) {
        Serial.print("I think LEAP_YEAR_ADJUSTMENT is ");
        Serial.println(LEAP_YEAR_ADJUSTMENT, 8);
      }
      DURATION_IN_UNITS = (CURRENT_MARRIAGE_DURATION_IN_SECONDS / yearsLength) - LEAP_YEAR_ADJUSTMENT;
      DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
      DURATION_IN_UNITS_string.replace(".", "-POINT-");
      displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " YEARS" + padding, 1);

      /* DECADES */
      DURATION_IN_UNITS = DURATION_IN_UNITS / 10.0000L;  // 10 years in a decade
      DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
      DURATION_IN_UNITS_string.replace(".", "-POINT-");
      displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " DECADES" + padding, 1);

      /* CENTURIES */
      DURATION_IN_UNITS = DURATION_IN_UNITS / 10.0000L;  // 10 decades in a century
      DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
      DURATION_IN_UNITS_string.replace(".", "-POINT-");
      displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " CENTURIES" + padding, 1);


    } else {
      displayMessage(padding + "NOT MARRIED YET! COME BACK AFTER THE WEDDING!" + padding, 1);
    }
  }




  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis();  // reset the timer
    if (DEBUGGER_FLAG) {
      Serial.println("\nThe current time according to the GPS is ");
      Serial.print("\nTime: ");
      Serial.print(GPS.hour, DEC);
      Serial.print(':');
      Serial.print(GPS.minute, DEC);
      Serial.print(':');
      Serial.print(GPS.seconds, DEC);
      Serial.print('.');
      Serial.println(GPS.milliseconds);
      Serial.print("Date: ");
      Serial.print(GPS.day, DEC);
      Serial.print('/');
      Serial.print(GPS.month, DEC);
      Serial.print("/20");
      Serial.println(GPS.year, DEC);
      Serial.print("Fix: ");
      Serial.print((int)GPS.fix);
      Serial.print(" quality: ");
      Serial.println((int)GPS.fixquality);
      if (GPS.fix) {
        //   Serial.print("Location: ");
        //   //      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
        //   //      Serial.print(", ");
        //   //      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
        //   Serial.print("Location (in degrees, works with Google Maps): ");
        //   Serial.print(GPS.latitudeDegrees, 6);
        //   Serial.print(", ");
        //   Serial.println(GPS.longitudeDegrees, 6);

        //      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
        //      Serial.print("Angle: "); Serial.println(GPS.angle);
        //      Serial.print("Altitude: "); Serial.println(GPS.altitude);
        Serial.print("Satellites: ");
        Serial.println((int)GPS.satellites);
      }  // end GPS.fix

      Serial.println("\nThe current time according to the clock is ");
      Serial.print("\nTime: ");
      Serial.print(RTC_now.hour(), DEC);
      Serial.print(':');
      if (RTC_now.minute() < 10) {
        Serial.print("0");
      }
      Serial.print(RTC_now.minute(), DEC);
      Serial.print(':');
      if (RTC_now.second() < 10) {
        Serial.print("0");
      }
      Serial.println(RTC_now.second(), DEC);
      Serial.print("Date: ");
      Serial.print(RTC_now.day(), DEC);
      Serial.print('/');
      Serial.print(RTC_now.month(), DEC);
      Serial.print('/');
      Serial.println(RTC_now.year(), DEC);


      Serial.print("\nRTC clock CURRENT_TIME: ");
      Serial.println(CURRENT_TIME);
    }
  }  // end DEBUGGER
}  // end of loop()

void displayMessage(String theMessage, int scroll) {
  if (DEBUGGER_FLAG) {
    Serial.println(textString);
  }

  if (scroll) {
    textString = theMessage;
    textStringLength = textString.length();
    for (int i = 0; i < textStringLength - 15; i++) {
      if (analogRead(brightnessPin) > 20) {
        displayString = textString.substring(i, i + 16);
        brightness = map(analogRead(brightnessPin), 0, 1024, 0, 16);
        speed = map(analogRead(speedPin), 0, 1024, 400, 70);
        display.setBrightness(brightness);  //14
        display.print(displayString);
        delay(speed);
      } else {
        display.print(padding);
      }
    }
  } else {
    display.print(theMessage);
  }
}


float leapYearCheck(int year, int month) {

  Serial.print("\n\n\nInside leapYearCheck year is ");
  Serial.println(year);
  float leapYearAdjustmentFactor = 0.00273224;

  if (year >= 2028 && month >= 3) {
    return leapYearAdjustmentFactor;
  }
  if (year >= 2032 && month >= 3) {
    return 2.0 * leapYearAdjustmentFactor;
  }
  if (year >= 2036 && month >= 3) {
    return 3.0 * leapYearAdjustmentFactor;
  }
  if (year >= 2040 && month >= 3) {
    return 4.0 * leapYearAdjustmentFactor;
  }
  if (year >= 2044 && month >= 3) {
    return 5.0 * leapYearAdjustmentFactor;
  }
  if (year >= 2048 && month >= 3) {
    return 6.0 * leapYearAdjustmentFactor;
  }
  if (year >= 2052 && month >= 3) {
    return 7.0 * leapYearAdjustmentFactor;
  }
  if (year >= 2056 && month >= 3) {
    return 8.0 * leapYearAdjustmentFactor;
  }
  if (year >= 2060 && month >= 3) {
    return 9.0 * leapYearAdjustmentFactor;
  }
  if (year >= 2064 && month >= 3) {
    return 10.0 * leapYearAdjustmentFactor;
  }

  return 0.0;
}
