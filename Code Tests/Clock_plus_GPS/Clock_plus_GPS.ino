#include <Wire.h>
#include <SPI.h>  // not used here, but needed to prevent a RTClib compile error
#include "RTClib.h"

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

#include <TimeLib.h>  // Include the Time library





// Create the RTC object.
// You may use RTC_DS1307 with the DS3231, DS1337, DS1340, Chronodot
// because those clock chips all use the same basic addresses.
RTC_DS1307 rtc;

// Create the GPS object
SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();

int GPSFIX = 0;

unsigned long GPS_UNIX_TIME;

void setup() {
  Serial.begin(115200);  // Set serial port speed
  Wire.begin();          // Start the I2C
  rtc.begin();           // Init RTC
  //    rtc.adjust(DateTime(1789078543));

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

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

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);

  Serial.println("setup() complete");
}

void loop() {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA()))  // this also sets the newNMEAreceived() flag to false
      return;                        // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis();  // reset the timer



    DateTime now = rtc.now();  // Read the time and date from the clock chip
    Serial.print("Clock time : ");
    Serial.println(now.unixtime());
    Serial.print("Clock time : ");
    Serial.print(now.year(), DEC);
    Serial.print('-');
    Serial.print(now.month(), DEC);
    Serial.print('-');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    if (now.minute() < 10) {
      Serial.print("0");
    }
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    if (now.second() < 10) {
      Serial.print("0");
    }
    Serial.print(now.second(), DEC);
    Serial.println("\n");








    Serial.print("GPS Time: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC);
    Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC);
    Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC);
    Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
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
      if (GPSFIX == 0) {
        Serial.println("\n\n\nThe GPS HAS THE CORRECT TIME!!!\n\n\n");

        GPS_UNIX_TIME = convertToUnixTimeLib(int(GPS.year) + 2000, int(GPS.month), int(GPS.day), int(GPS.hour), int(GPS.minute), int(GPS.seconds));
        Serial.print("I think the GPS UNIX TIME IS ");
        Serial.println(String(GPS_UNIX_TIME));
        //     rtc.adjust(GPS_UNIX_TIME);
        rtc.adjust(DateTime(2000 + GPS.year, GPS.month, GPS.day, GPS.hour, GPS.minute, GPS.seconds));
      }
      // Serial.print("Location: ");
      // Serial.print(GPS.latitude, 4);
      // Serial.print(GPS.lat);
      // Serial.print(", ");
      // Serial.print(GPS.longitude, 4);
      // Serial.println(GPS.lon);

      // Serial.print("Speed (knots): ");
      // Serial.println(GPS.speed);
      // Serial.print("Angle: ");
      // Serial.println(GPS.angle);
      // Serial.print("Altitude: ");
      // Serial.println(GPS.altitude);
      // Serial.print("Satellites: ");
      // Serial.println((int)GPS.satellites);
      // Serial.print("Antenna status: ");
      // Serial.println((int)GPS.antenna);
    }
    GPSFIX = GPS.fix;
  }
  //  delay(1000);
}

unsigned long convertToUnixTimeLib(int year, int month, int day, int hour, int minute, int second) {
  tmElements_t tm;

  tm.Year = year;    // Converts standard year to offset from 1970
  tm.Month = month;  // Standard Month (1-12)
  tm.Day = day;      // Standard Day (1-31)
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;

  Serial.print(tm.Year);
  Serial.print("-");
  Serial.print(tm.Month);
  Serial.print("-");
  Serial.print(tm.Day);
  Serial.print(" ");
  Serial.print(tm.Hour);
  Serial.print(":");
  Serial.print(tm.Minute);
  Serial.print(":");
  Serial.println(tm.Second);


  return makeTime(tm);  // Returns time_t (Unix timestamp)
}
