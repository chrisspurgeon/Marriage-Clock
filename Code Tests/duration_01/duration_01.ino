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
double fortnightsLength = 12096000.0;
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
int currentHour = 21;
int currentMinute = 9;
int currentSecond = 0;

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
  Serial.print("The current time is ");
//  Serial.println(t);
  Serial.println(CURRENTTIME);
}

void loop() {
  // put your main code here, to run repeatedly
  DURATION = CURRENTTIME - TIMEOFMARRIAGE;
  Serial.print("The current marriage duration is ");
  Serial.println(DURATION);
  Serial.print("CURRENTTIME is ");
  Serial.println(CURRENTTIME);
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
