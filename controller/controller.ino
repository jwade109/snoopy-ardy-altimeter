#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085_Unified       baro  = Adafruit_BMP085_Unified();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified();
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified();

const int DATA_LOGGER_CHIP_SELECT = 10;
const int BIG_RED_INDICATOR_LIGHT_PIN = 3;
const int WRITE_THE_STUFF_TO_CSV_FILE = 1;
const char *SD_LOG_FILENAME = "LOGMA.TXT";

#define ALTERNATE_WITH_PERIOD(n) ((millis() / n) % 2)
#define CYCLE_LEDS_EVERY_MS(n) set_indicator_led(ALTERNATE_WITH_PERIOD(n))

unsigned int loop_counter = 0;

void set_indicator_led(bool state, bool invert = false)
{
	digitalWrite(LED_BUILTIN, state);
	digitalWrite(BIG_RED_INDICATOR_LIGHT_PIN, state);
}

void on_error_loop(const char *error)
{
  	int time = 0;
	while (1)
	{
		if (millis() - time > 1000)
		{
			Serial.println(error);
			time = millis();
		}
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

File logfile;

void write_log(const char *str)
{
	if (WRITE_THE_STUFF_TO_CSV_FILE)
	{
		if (!logfile)
		{
			on_error_loop("FILE ISN'T OPEN");
		}
		logfile.write(str, strlen(str));
		logfile.flush();
	}
	else
	{
		Serial.print(str);
	}
}

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(BIG_RED_INDICATOR_LIGHT_PIN, OUTPUT);
	pinMode(DATA_LOGGER_CHIP_SELECT, OUTPUT);

	Serial.begin(115200);
	if (!SD.begin(DATA_LOGGER_CHIP_SELECT))
	{
		on_error_loop("SD CARD INIT FAILURE");
	}

	Serial.print("Logging to ");
	Serial.println(SD_LOG_FILENAME);

	logfile = SD.open(SD_LOG_FILENAME, FILE_WRITE);
	if (!logfile)
	{
		on_error_loop("FAILED TO OPEN FILE");
	}

	if(!baro.begin())
	{
		on_error_loop("ROMETER INIT FAILURE");
	}
	if(!accel.begin())
	{
		on_error_loop("L'ROMETER INIT FAILURE");
	}
	if(!gyro.begin())
	{
		on_error_loop("GYROMETER INIT FAILURE");
	}

	write_log("n,t,p\n"); // not network time protocol
}

void loop()
{
	CYCLE_LEDS_EVERY_MS(1000);

	unsigned long time = millis();

	sensors_event_t baro_event;
	baro.getEvent(&baro_event);

	// sensors_event_t accel_event;
	// baro.getEvent(&accel_event);
	// sensors_event_t gyro_event;
	// baro.getEvent(&gyro_event);

	char pressure_float[100];
	dtostrf(baro_event.pressure, 3, 2, pressure_float);
	char buffer[300];
	sprintf(buffer, "%d,%lu,%s\n", loop_counter, time, pressure_float);
	write_log(buffer);

	++loop_counter;
}
