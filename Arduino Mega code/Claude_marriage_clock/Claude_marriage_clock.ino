#include <Adafruit_GPS.h>
#include <Wire.h>
#include <SparkFun_Alphanumeric_Display.h>
#include "RTClib.h"

// GPS WIRING
// VIN gets 5V
// TX on the GPS unit goes to RX1 (PIN 19) on the MEGA
// RX on the GPS unit goes to TX1 (PIN 18) on the MEGA

// DISPLAY WIRING
// Black wire goes to GND
// Red wire goes to 5V
// Blue wire goes to SDA (PIN 20)
// Yellow wire goes to SCL (PIN 21)

// CLOCK (DS3231) WIRING
// VIN gets 5V
// SDA goes to pin 20
// SCL goes to pin 21

// ---------------------------------------------------------------------------
// OBJECTS
// ---------------------------------------------------------------------------
RTC_DS3231 rtc;
Adafruit_GPS GPS(&Serial1);
HT16K33 display;

// Set GPSECHO to true to echo raw GPS sentences to the Serial console.
#define GPSECHO false

// Set to 0 for the final build.
bool DEBUGGER_FLAG = 1;

bool usingInterrupt = false;
void useInterrupt(boolean);

// ---------------------------------------------------------------------------
// DISPLAY / CONTROLS
// ---------------------------------------------------------------------------
String padding = "                ";  // 16 spaces
const int brightnessPin = A0;        // brightness potentiometer
const int speedPin = A1;             // scroll speed potentiometer

// ---------------------------------------------------------------------------
// TIME CONSTANTS (all integer seconds; no floating point needed)
// ---------------------------------------------------------------------------

// Amy and Greg
// October 10, 1986 at 4:00 pm EDT = October 10, 1986 at 20:00 UTC = 529358400 unix time.
const unsigned long TIME_OF_MARRIAGE = 529358400UL;

// Calendar date of the wedding (UTC). Time of day is taken from TIME_OF_MARRIAGE.
// These must agree with TIME_OF_MARRIAGE above.
const int MARRIAGE_YEAR = 1986;
const int MARRIAGE_MONTH = 10;
const int MARRIAGE_DAY = 10;

// Any RTC reading earlier than this is treated as invalid (Jan 1, 2026 00:00 UTC).
const unsigned long EARLIEST_VALID_TIME = 1767225600UL;

const unsigned long MINUTE_SECONDS = 60UL;
const unsigned long HOUR_SECONDS = 3600UL;
const unsigned long DAY_SECONDS = 86400UL;
const unsigned long WEEK_SECONDS = 604800UL;
const unsigned long FORTNIGHT_SECONDS = 1209600UL;

uint32_t statsTimer = 0;

// ---------------------------------------------------------------------------
// HELPERS
// ---------------------------------------------------------------------------

// Two-digit zero-padded number, e.g. 7 -> "07"
String two(int n) {
  if (n < 10) return "0" + String(n);
  return String(n);
}

// Scrolls (scroll=1) or statically prints (scroll=0) a message on the 16-character display.
void displayMessage(String theMessage, int scroll) {
  if (DEBUGGER_FLAG) {
    Serial.println(theMessage);
  }

  if (!scroll) {
    display.print(theMessage);
    return;
  }

  int len = theMessage.length();
  for (int i = 0; i < len - 15; i++) {
    int knob = analogRead(brightnessPin);
    int speed = map(analogRead(speedPin), 0, 1023, 400, 70);
    if (knob > 20) {
      int brightness = map(knob, 0, 1023, 0, 15);
      display.setBrightness(brightness);
      display.print(theMessage.substring(i, i + 16));
    } else {
      display.print(padding);  // brightness knob all the way down = blank
    }
    delay(speed);
  }
}

// Formats num/den with the given number of decimals using integer math only,
// writing "-POINT-" for the decimal point. Avoids the 32-bit float precision
// limit on the Mega.
String formatFixed(uint64_t num, uint64_t den, uint8_t decimals) {
  String s = String((unsigned long)(num / den));
  if (decimals == 0) return s;
  s += "-POINT-";
  uint64_t rem = num % den;
  for (uint8_t i = 0; i < decimals; i++) {
    rem *= 10;
    s += String((unsigned int)(rem / den));
    rem %= den;
  }
  return s;
}

// Days since 1970-01-01 for a given civil date (proleptic Gregorian).
long daysFromCivil(long y, int m, int d) {
  y -= (m <= 2);
  long era = y / 400;
  long yoe = y - era * 400;
  long doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
  long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097L + doe - 719468L;
}

// Unix time (UTC) of the wedding anniversary in the given year.
unsigned long anniversaryUnix(int year) {
  return (unsigned long)daysFromCivil(year, MARRIAGE_MONTH, MARRIAGE_DAY) * DAY_SECONDS
         + (TIME_OF_MARRIAGE % DAY_SECONDS);
}

// Reads the clock fresh and scrolls "YOU HAVE BEEN MARRIED FOR <n> <unit>"
// for fixed-length units (seconds through fortnights).
void showDuration(const char *unitName, unsigned long divisor, uint8_t decimals) {
  DateTime now = rtc.now();
  unsigned long t = (unsigned long)now.unixtime();
  if (t <= TIME_OF_MARRIAGE) return;

  unsigned long elapsed = t - TIME_OF_MARRIAGE;

  if (DEBUGGER_FLAG) {
    Serial.print("elapsed (");
    Serial.print(unitName);
    Serial.print(") seconds: ");
    Serial.println(elapsed);
  }

  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + formatFixed(elapsed, divisor, decimals) + " " + unitName + padding, 1);
}

// Calendar-accurate years (or decades/centuries via yearsPerUnit = 1, 10, 100).
// Whole years are counted by real anniversaries; the fraction is the progress
// through the current anniversary year, whose length is 365 or 366 days.
void showCalendarYears(const char *unitName, unsigned long yearsPerUnit, uint8_t decimals) {
  DateTime now = rtc.now();
  unsigned long t = (unsigned long)now.unixtime();
  if (t <= TIME_OF_MARRIAGE) return;

  int whole = now.year() - MARRIAGE_YEAR;
  if (anniversaryUnix(MARRIAGE_YEAR + whole) > t) whole--;  // anniversary not reached yet this year

  unsigned long last = anniversaryUnix(MARRIAGE_YEAR + whole);
  unsigned long next = anniversaryUnix(MARRIAGE_YEAR + whole + 1);
  uint64_t yearLen = next - last;  // 365 or 366 days, in seconds

  uint64_t num = (uint64_t)whole * yearLen + (t - last);  // total years, scaled by yearLen
  uint64_t den = yearLen * yearsPerUnit;

  if (DEBUGGER_FLAG) {
    Serial.print("whole years: ");
    Serial.print(whole);
    Serial.print("  seconds into year: ");
    Serial.print(t - last);
    Serial.print(" of ");
    Serial.println((unsigned long)yearLen);
  }

  displayMessage(padding + "YOU HAVE BEEN MARRIED FOR " + formatFixed(num, den, decimals) + " " + unitName + padding, 1);
}

// True if the GPS currently has a fix and a plausible date/time.
bool gpsTimeIsUsable() {
  if (!GPS.fix) return false;
  if (GPS.month < 1 || GPS.month > 12 || GPS.day < 1 || GPS.day > 31) return false;
  DateTime g(2000 + GPS.year, GPS.month, GPS.day, GPS.hour, GPS.minute, GPS.seconds);
  return (unsigned long)g.unixtime() >= EARLIEST_VALID_TIME;
}

// Sets the DS3231 from the GPS (UTC) and briefly shows the date and time.
void setClockFromGPS() {
  rtc.adjust(DateTime(2000 + GPS.year, GPS.month, GPS.day, GPS.hour, GPS.minute, GPS.seconds));
  DateTime now = rtc.now();

  displayMessage(padding + "UPDATED CLOCK SIGNAL ACQUIRED" + padding, 1);

  displayMessage("  CURRENT DATE:", 0);
  delay(2000);
  displayMessage("  " + two(now.month()) + "-" + two(now.day()) + "-" + String(now.year()) + " UTC", 0);
  delay(4000);

  now = rtc.now();
  displayMessage("  CURRENT TIME:", 0);
  delay(2000);
  now = rtc.now();
  displayMessage("    " + two(now.hour()) + ":" + two(now.minute()) + " UTC", 0);
  delay(4000);
}

// ---------------------------------------------------------------------------
// SETUP
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's
  GPS.begin(9600);

  Serial.print("DEBUGGER IS ");
  Serial.println(DEBUGGER_FLAG);
  Serial.print("Time of marriage is ");
  Serial.println(TIME_OF_MARRIAGE);

  // RMC (recommended minimum, includes date) and GGA (fix data)
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);

  // Timer0 interrupt reads GPS data once per millisecond
  useInterrupt(true);

  delay(2000);

  // SET UP DISPLAY CONNECTIONS
  Wire.begin();

  if (display.begin(0x73, 0x72, 0x71, 0x70) == false) {
    Serial.println("Display did not acknowledge! Freezing.");
    while (1)
      ;
  }
  Serial.println("Displays acknowledged.");
  display.setBrightness(map(analogRead(brightnessPin), 0, 1023, 0, 10));

  // SET UP REAL TIME CLOCK
  if (!rtc.begin()) {
    Serial.println("Couldn't find the DS3231 clock! Freezing.");
    display.print("  CLOCK ERROR");
    while (1)
      ;
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power (battery dead or missing). Time will be set from GPS.");
  }

  DateTime t = rtc.now();
  Serial.println("Initial Real Time Clock reading (UTC)...");
  Serial.print(t.month());
  Serial.print("/");
  Serial.print(t.day());
  Serial.print("/");
  Serial.print(t.year());
  Serial.print(" ");
  Serial.print(t.hour());
  Serial.print(":");
  Serial.print(two(t.minute()));
  Serial.print(":");
  Serial.println(two(t.second()));

  delay(1000);

  // INITIAL WELCOME DISPLAY
  displayMessage(padding + "MARRIAGE CLOCK" + padding + "DESIGNED AND BUILT IN CALIFORNIA BY CHRIS SPURGEON" + padding, 1);

  for (int i = 0; i < 5; i++) {
    display.print("  INITIALIZING:");
    delay(500);
    display.print("  INITIALIZING");
    delay(500);
  }

  statsTimer = millis();
}

// ---------------------------------------------------------------------------
// GPS INTERRUPT
// ---------------------------------------------------------------------------

// Called once a millisecond; reads any new GPS data and stores it.
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

// ---------------------------------------------------------------------------
// MAIN LOOP
// ---------------------------------------------------------------------------
void loop() {

  // GPS: parse any newly received sentence. freshParse is true only when this
  // pass of loop() parsed a brand-new sentence, so the GPS values are current.
  bool freshParse = false;
  if (!usingInterrupt) {
    GPS.read();
  }
  if (GPS.newNMEAreceived()) {
    if (GPS.parse(GPS.lastNMEA())) {
      freshParse = true;
    }
  }

  // Check the clock. It's valid if it hasn't lost power and reads a plausible time.
  DateTime now = rtc.now();
  unsigned long currentTime = (unsigned long)now.unixtime();
  bool rtcValid = !rtc.lostPower() && currentTime >= EARLIEST_VALID_TIME;

  if (DEBUGGER_FLAG) {
    Serial.print("loop(): rtcValid=");
    Serial.print(rtcValid);
    Serial.print(" gpsFix=");
    Serial.print((int)GPS.fix);
    Serial.print(" currentTime=");
    Serial.println(currentTime);
  }

  // --- Clock needs setting ---
  if (!rtcValid) {
    if (freshParse && gpsTimeIsUsable()) {
      setClockFromGPS();
    } else if (GPS.fix) {
      // We have a fix; wait for the next fresh sentence so the time we set is current.
    } else {
      displayMessage(padding + "WAITING FOR CLOCK SIGNAL" + padding, 1);
      displayMessage(padding + "THIS MAY TAKE AN HOUR OR EVEN MORE" + padding, 1);
      displayMessage(padding + "HAVE PATIENCE" + padding, 1);
      displayMessage(padding + "IF THIS MESSAGE CONTINUES FOR MORE THAN AN HOUR MOVE THE DEVICE CLOSER TO A WINDOW AND TRY AGAIN" + padding, 1);
      delay(2100);
    }
    return;
  }

  // --- Clock is good: MAIN DISPLAY ---
  if (currentTime > TIME_OF_MARRIAGE) {
    showDuration("SECONDS", 1UL, 0);
    showDuration("MINUTES", MINUTE_SECONDS, 0);
    showDuration("HOURS", HOUR_SECONDS, 2);
    showDuration("DAYS", DAY_SECONDS, 4);
    showDuration("WEEKS", WEEK_SECONDS, 4);
    showDuration("FORTNIGHTS", FORTNIGHT_SECONDS, 4);
    showCalendarYears("YEARS", 1, 4);
    showCalendarYears("DECADES", 10, 4);
    showCalendarYears("CENTURIES", 100, 4);
  } else {
    displayMessage(padding + "NOT MARRIED YET! COME BACK AFTER THE WEDDING!" + padding, 1);
  }

  // Periodic debug stats
  if (DEBUGGER_FLAG && millis() - statsTimer > 2000) {
    statsTimer = millis();
    Serial.print("GPS time: ");
    Serial.print(GPS.hour);
    Serial.print(":");
    Serial.print(two(GPS.minute));
    Serial.print(":");
    Serial.print(two(GPS.seconds));
    Serial.print("  date 20");
    Serial.print(GPS.year);
    Serial.print("-");
    Serial.print(GPS.month);
    Serial.print("-");
    Serial.print(GPS.day);
    Serial.print("  fix ");
    Serial.print((int)GPS.fix);
    Serial.print(" quality ");
    Serial.print((int)GPS.fixquality);
    Serial.print(" sats ");
    Serial.println((int)GPS.satellites);
  }
}
