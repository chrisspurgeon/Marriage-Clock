#include <TimeLib.h>  // Include the Time library

double durationInSeconds;
double durationInMinutes;
double durationInHours;
double durationInDays;
double durationInWeeks;
double durationInFortnights;
double durationInYears;
double durationInDecades;
double durationInCenturies;
double durationInMillennia;

double minutesLength = 60.0;
double hoursLength = 3600.0;
double daysLength = 86400.0;
double weeksLength = 604800.0;
double fortnightsLength = 1209600.0;
double yearsLength = 31556736.0;
double decadesLength = 315567360.0;
double centuriesLength = 3155673600.0;
double millenniaLength = 31556736000;
unsigned long TIMEOFMARRIAGE = 1788642000; // Saturday, September 5, 2026, at 21:00 UTC
unsigned long DURATION;
unsigned long CURRENTTIME;

// STATIC VALUES FOR TESTING
int currentYear = 2029;
int currentMonth = 4;
int currentDay = 17;
int currentHour = 20;
int currentMinute = 59;
int currentSecond = 30;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("THIS IS duration_01!");
  Serial.println(minutesLength);
  Serial.println(hoursLength);
  Serial.println(daysLength);
  Serial.println(weeksLength);
  Serial.println(fortnightsLength);
  Serial.println(yearsLength);
  Serial.println(decadesLength);
  Serial.println(centuriesLength);
  Serial.println(millenniaLength);


  CURRENTTIME = convertToUnixTimeLib(currentYear, currentMonth, currentDay, currentHour, currentMinute, currentSecond);
//  CURRENTTIME = double(t);
  Serial.print("\nThe current time is ");
//  Serial.println(t);
  Serial.println(CURRENTTIME + "\n");
}

void loop() {
  // put your main code here, to run repeatedly
  DURATION = CURRENTTIME - TIMEOFMARRIAGE;
  Serial.print("The current marriage duration is ");
  Serial.println(DURATION);
  Serial.print("CURRENTTIME is ");
  Serial.println(CURRENTTIME);

  // Marriage in seconds
  Serial.print("You have been married for ");
  Serial.print(DURATION,6);
  Serial.println(" seconds.");

  // Marriage in minutes
  Serial.print("You have been married for ");
  Serial.print(DURATION / minutesLength,6);
  Serial.println(" minutes.");

  // Marriage in hours
  Serial.print("You have been married for ");
  Serial.print(DURATION / hoursLength,6);
  Serial.println(" hours.");

  // Marriage in days
  Serial.print("You have been married for ");
  Serial.print(DURATION / daysLength,6);
  Serial.println(" days.");

  // Marriage in weeks
  Serial.print("You have been married for ");
  Serial.print(DURATION / weeksLength,6);
  Serial.println(" weeks.");

  // Marriage in fortnights
  Serial.print("You have been married for ");
  Serial.print(DURATION / fortnightsLength,6);
  Serial.println(" fortnights.");

  // Marriage in years
  Serial.print("You have been married for ");
  Serial.print(DURATION / yearsLength,6);
  Serial.println(" years.");

  // Marriage in decades
  Serial.print("You have been married for ");
  Serial.print(DURATION / decadesLength,6);
  Serial.println(" decades.");

  // Marriage in centuries
  Serial.print("You have been married for ");
  Serial.print(DURATION / centuriesLength,6);
  Serial.println(" centuries.");

  // Marriage in millennia
  Serial.print("You have been married for ");
  Serial.print(DURATION / millenniaLength,6);
  Serial.println(" millennia.");





  delay(1000);
  CURRENTTIME++;
}



unsigned long convertToUnixTimeLib(int year, int month, int day, int hour, int minute, int second) {
  tmElements_t tm;
  
  tm.Year = CalendarYrToTm(year); // Converts standard year to offset from 1970
  tm.Month = month;               // Standard Month (1-12)
  tm.Day = day;                   // Standard Day (1-31)
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;

  return makeTime(tm); // Returns time_t (Unix timestamp)
}
