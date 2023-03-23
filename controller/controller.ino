#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20.h>
#include <Wire.h>

Adafruit_BMP085_Unified baro = Adafruit_BMP085_Unified();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified();
Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified();

const int BIG_RED_INDICATOR_LIGHT_PIN = 10;

#define ALTERNATE_WITH_PERIOD(n) ((millis() / n) % 2)
#define CYCLE_LEDS_EVERY_MS(n) set_indicator_led(ALTERNATE_WITH_PERIOD(n))

bool led_state = false;

void set_indicator_led(bool state)
{
  digitalWrite(LED_BUILTIN, state);
  digitalWrite(BIG_RED_INDICATOR_LIGHT_PIN, state);
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

void do_big_startup_light_show()
{
  for (int i = 0; i < 100; ++i)
  {
    set_indicator_led(i % 2);
    delay(50);
  }
  set_indicator_led(true);
  delay(2000);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BIG_RED_INDICATOR_LIGHT_PIN, OUTPUT);

  Serial.begin(921600);
  Wire.begin();
  if(!baro.begin())
  {
    on_error_loop("BAROMETER INIT FAILURE");
  }
  if(!accel.begin())
  {
    on_error_loop("ACCELEROMETER INIT FAILURE");
  }
  if(!gyro.begin())
  {
    on_error_loop("GYROSCOPE INIT FAILURE");
  }

  do_big_startup_light_show();
}

#define SERIAL_WRITE_VALUE(name, value) Serial.print(name); Serial.print(value); Serial.print(",");

unsigned int loop_counter = 0;

void loop()
{
  CYCLE_LEDS_EVERY_MS(1000);

  unsigned long time = millis();

  SERIAL_WRITE_VALUE("t:", time);
  SERIAL_WRITE_VALUE("n:", loop_counter);
  SERIAL_WRITE_VALUE("dt:", time / loop_counter);

  sensors_event_t event;

  baro.getEvent(&event);
  SERIAL_WRITE_VALUE("p:", event.pressure);

  accel.getEvent(&event);
  SERIAL_WRITE_VALUE("ax:", event.acceleration.x);
  SERIAL_WRITE_VALUE("ay:", event.acceleration.y);
  SERIAL_WRITE_VALUE("az:", event.acceleration.z);

  gyro.getEvent(&event);
  SERIAL_WRITE_VALUE("wx:", event.gyro.x);
  SERIAL_WRITE_VALUE("wy:", event.gyro.y);
  SERIAL_WRITE_VALUE("wz:", event.gyro.z);

  Serial.println();
  
  ++loop_counter;
}
