#include "SD.h"
#include <Wire.h>

const int chipSelect = 10;
File logfile;

#define ALTERNATE_WITH_PERIOD(n) ((millis() / n) % 2)
#define CYCLE_LEDS_EVERY_MS(n) set_indicator_led(ALTERNATE_WITH_PERIOD(n))

bool led_state = false;

void set_indicator_led(bool state)
{
  digitalWrite(LED_BUILTIN, state);
}

void on_error_loop(const char *error)
{
  while (1)
  {
    Serial.println(error);
    CYCLE_LEDS_EVERY_MS(250);
    delay(20);
  }
}

void setup()
{
  Serial.begin(921600);

  pinMode(LED_BUILTIN, OUTPUT);

  // create a new file
  const char* filename = "log.csv";

  logfile = SD.open(filename, FILE_WRITE);
  if (!logfile)
  {
    on_error_loop("couldnt create file");
  }
}

void loop()
{
    CYCLE_LEDS_EVERY_MS(1000);
    Serial.println(millis());
}