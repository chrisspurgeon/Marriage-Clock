
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SparkFun_Alphanumeric_Display.h>

// GPS wiring
// VIN gets 5V
// TX on the GPS unit goes to RX1 (PIN 19) on the MEGA
// RX on the GPS unit goes to TX1 (PIN 18) on the MEGA

// DISPLAY WIRING
// Black wire goes to GND
// Red wire goes to 5V
// Blue wire goes to SDA (PIN 20)
// Yellow wire goes to SCL (PIN 21)


Adafruit_GPS GPS(&Serial1);
HardwareSerial mySerial = Serial1;
HT16K33 display;


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences.
#define GPSECHO false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean);  // Func prototype keeps Arduino 0023 happy



// Diaplay variables
String textString;
String displayString;
String padding = "                ";  // 16 spaces
int textStringLength = 0;

// Brightness and speed variables
int brightnessPin = A0;  // select the input pin for the potentiometer
int speedPin = A1;       // select the input pin for the potentiometer
int brightness = 4;      // variable to store the value coming from the sensor
int speed = 500;         // variable to store the value coming from the sensor

// GPS lock variables
int lastGPSlock = 0;
int currentGPSlock = 0;




void setup() {

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);

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

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);




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
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);



  /*

  INITIAL WELCOME DISPLAY

  */

  textString = padding + "MARRIAGE CLOCK" + padding + "DESIGNED AND BUILT IN AUGUST 2026 BY CHRIS SPURGEON" + padding;
  //  Serial.print("Right now I thing displayString is ");
  //  Serial.println(textString);
  textStringLength = textString.length();
  //  Serial.print("and I think the length is ");
  //  Serial.println(textStringLength);
  for (int i = 0; i < textStringLength - 15; i++) {
    displayString = textString.substring(i, i + 16);
    brightness = map(analogRead(brightnessPin), 0, 1024, 0, 16);
    speed = map(analogRead(speedPin), 0, 1024, 400, 10);
    //    Serial.println(brightness);
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

uint32_t timer = millis();

/*

END OF GPS PROCESSING

*/



void loop()  // run over and over again
{
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

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();


  // Set GPS fix status
  if ((int)GPS.fix != 0 && (int)GPS.satellites != 0) {
    currentGPSlock = 1;
  } else {
    currentGPSlock = 0;
    lastGPSlock = 0;
    displayMessage(padding + "WAITING FOR CLOCK SIGNAL" + padding, 1);
  }

  // GOT A GOOD FIX. DISPLAY INITIAL DATE AND TIME.
  if (currentGPSlock == 1 && lastGPSlock == 0) {
    displayMessage(padding + "BOOYAH -- CLOCK SIGNAL ACQUIRED" + padding, 1);
    lastGPSlock = 1;

    // DISPLAY INITIAL DATE AND TIME
    displayMessage("  CURRENT DATE:", 0);
    delay(2000);

    // DATE
    if (int(GPS.month) < 10) {
      textString = "  0" + String(GPS.month) + "-";
    } else {
      textString = "  " + String(GPS.month) + "-";
    }
    if (int(GPS.day) < 10) {
      textString += "0" + String(GPS.day) + "-";
    } else {
      textString += String(GPS.day) + "-";
    }
    textString += String(GPS.year) + " UTC";

    displayMessage(textString, 0);
    delay(4000);

    // TIME
    displayMessage("  CURRENT TIME:", 0);
    delay(2000);

    if (int(GPS.hour) < 10) {
      textString = "    0" + String(GPS.hour) + ":";
    } else {
      textString = "    " + String(GPS.hour) + ":";
    }
    if (int(GPS.minute) < 10) {
      textString += "0" + String(GPS.minute) + " UTC";
    } else {
      textString += String(GPS.minute) + " UTC";
    }

    displayMessage(textString, 0);
    delay(4000);
  }



  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis();  // reset the timer

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
    }
  }
}

void displayMessage(String theMessage, int scroll) {
  if (scroll) {
    textString = theMessage;
    textStringLength = textString.length();
    for (int i = 0; i < textStringLength - 15; i++) {
      displayString = textString.substring(i, i + 16);
      brightness = map(analogRead(brightnessPin), 0, 1024, 0, 16);
      speed = map(analogRead(speedPin), 0, 1024, 400, 10);
      display.setBrightness(brightness);  //14
      display.print(displayString);
      delay(speed);
    }
  } else {
    display.print(theMessage);
  }
}
