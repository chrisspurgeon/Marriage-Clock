#include <SoftwareSerial.h>
#include <TimeLib.h>  // Include the Time library



// Diaplay variables
String textString;
String displayString;
String padding = "                ";  // 16 spaces
int textStringLength = 0;

/*

TIME VARIABLES

*/
float secondsLength = 1.0;
double minutesLength = 60.0;
double hoursLength = 3600.0;
double daysLength = 86400.0;
double weeksLength = 604800.0;
double fortnightsLength = 1209600.0;
double yearsLength = 31556736.0;
double decadesLength = 315567360.0;
double centuriesLength = 3155673600.0;
//unsigned long TIME_OF_MARRIAGE = 1788642000L;      // Saturday, September 5, 2026, at 21:00 UTC
unsigned long TIME_OF_MARRIAGE = 674774091L;  // Friday, May 19, 1991, at 21:14:51 UTC
float CURRENT_MARRIAGE_DURATION;
unsigned long CURRENT_TIME;
float DURATION_IN_UNITS;
String DURATION_IN_UNITS_string;

int timer = 0;

void setup() {

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);

  delay(2000);

}  // END OF setup()



void loop()  // run over and over again
{
Serial.println("\n\n\nStarting output...");



  /*
      MAIN DISPLAY
      If we're here, we have the correct time and can start the duration displays.
  */

  CURRENT_MARRIAGE_DURATION = CURRENT_TIME - TIME_OF_MARRIAGE;

  /* WEEKS */
  DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION / weeksLength;
  DURATION_IN_UNITS_string = String(DURATION_IN_UNITS);
  DURATION_IN_UNITS_string.replace(".", "-POINT-");
  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " WEEKS" + padding, 1);

  /* DECADES */
  DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION / decadesLength;
  DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
  DURATION_IN_UNITS_string.replace(".", "-POINT-");
  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " DECADES" + padding, 1);

  /* FORTNIGHTS */
  DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION / fortnightsLength;
  DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
  DURATION_IN_UNITS_string.replace(".", "-POINT-");
  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " FORTNIGHTS" + padding, 1);

  /* CENTURIES */
  DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION / centuriesLength;
  DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
  DURATION_IN_UNITS_string.replace(".", "-POINT-");
  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " CENTURIES" + padding, 1);

  /* SECONDS */
  DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION / secondsLength;
  DURATION_IN_UNITS_string = String(DURATION_IN_UNITS);
  DURATION_IN_UNITS_string.replace(".00", "");
  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " SECONDS" + padding, 1);

  /* MINUTES */
  DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION / minutesLength;
  DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
  DURATION_IN_UNITS_string.replace(".", "-POINT-");
  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " MINUTES" + padding, 1);

  /* HOURS */
  DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION / hoursLength;
  DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
  DURATION_IN_UNITS_string.replace(".", "-POINT-");
  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " HOURS" + padding, 1); /* MINUTES */

  /* DAYS */
  DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION / daysLength;
  DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
  DURATION_IN_UNITS_string.replace(".", "-POINT-");
  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " DAYS" + padding, 1);

  /* YEARS */
  DURATION_IN_UNITS = CURRENT_MARRIAGE_DURATION / yearsLength;
  DURATION_IN_UNITS_string = String(DURATION_IN_UNITS, 4);
  DURATION_IN_UNITS_string.replace(".", "-POINT-");
  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + DURATION_IN_UNITS_string + " YEARS" + padding, 1);


  delay(5000);


} // END loop()




// approximately every 2 seconds or so, print out the current stats
// if (millis() - timer > 2000) {
//   timer = millis();  // reset the timer
//   //    CURRENT_TIME = convertToUnixTimeLib(int(GPS.year) + 2000, int(GPS.month), int(GPS.day), int(GPS.hour), int(GPS.minute), int(GPS.seconds));

// }  // end of loop()

void displayMessage(String theMessage, int scroll) {
  textString = theMessage;
  Serial.println(textString);
}


unsigned long convertToUnixTimeLib(int year, int month, int day, int hour, int minute, int second) {
  tmElements_t tm;

  tm.Year = CalendarYrToTm(year);  // Converts standard year to offset from 1970
  tm.Month = month;                // Standard Month (1-12)
  tm.Day = day;                    // Standard Day (1-31)
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;
  return makeTime(tm);  // Returns time_t (Unix timestamp)
}
