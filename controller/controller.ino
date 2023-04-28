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

#define WRITE_THE_STUFF_TO_COMMA_DELIMITED_FILE 1
#define WRITE_THE_STUFF_TO_THE_SERIAL_PORT 1

#define ALTERNATE_WITH_PERIOD(n) ((millis() / n) % 2)
#define CYCLE_LEDS_EVERY_MS(n) set_indicator_led(ALTERNATE_WITH_PERIOD(n))
#define SERIAL_WRITE_VALUE(name, value) \
	Serial.print(name); Serial.print(value); Serial.print(",");
#define CSV_WRITE_VALUE(value) \
	logfile.write(value); logfile.write(",");


unsigned int loop_counter = 0;

File logfile;

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
	pinMode(DATA_LOGGER_CHIP_SELECT, OUTPUT);

	Serial.begin(115200);
	if (!SD.begin(DATA_LOGGER_CHIP_SELECT))
	{
		on_error_loop("SD CARD INIT FAILURE");
	}

	// create a new file
	char filename[] = "LOGGER00.CSV";
	for (uint8_t i = 0; i < 100; i++)
	{
		filename[6] = i / 10 + '0';
		filename[7] = i % 10 + '0';
		if (!SD.exists(filename))
		{
			logfile = SD.open(filename, FILE_WRITE);
			break;
		}
	}

	if (!logfile)
	{
		on_error_loop("LOGFILE INIT FAILURE");
	}

	Serial.print("Logging to: ");
	Serial.println(filename);

	Wire.begin();

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

	logfile.write("MY ASS");

	do_big_startup_light_show();
}

void loop()
{
	CYCLE_LEDS_EVERY_MS(1000);

	unsigned long time = millis();

	sensors_event_t baro_event;
	baro.getEvent(&baro_event);
	sensors_event_t accel_event;
	baro.getEvent(&accel_event);
	sensors_event_t gyro_event;
	baro.getEvent(&gyro_event);

	#if WRITE_THE_STUFF_TO_COMMA_DELIMITED_FILE
	CSV_WRITE_VALUE(loop_counter);
	CSV_WRITE_VALUE(time);
	CSV_WRITE_VALUE(baro_event.pressure);
	logfile.write("\n");
	logfile.flush();
	#endif // WRITE_THE_STUFF_TO_COMMA_DELIMITED_FILE

	#if WRITE_THE_STUFF_TO_THE_SERIAL_PORT
	SERIAL_WRITE_VALUE("t:", time);
	SERIAL_WRITE_VALUE("n:", loop_counter);
	SERIAL_WRITE_VALUE("dt:", time / loop_counter);
	SERIAL_WRITE_VALUE("p:", baro_event.pressure);
	SERIAL_WRITE_VALUE("ax:", accel_event.acceleration.x);
	SERIAL_WRITE_VALUE("ay:", accel_event.acceleration.y);
	SERIAL_WRITE_VALUE("az:", accel_event.acceleration.z);
	SERIAL_WRITE_VALUE("wx:", gyro_event.gyro.x);
	SERIAL_WRITE_VALUE("wy:", gyro_event.gyro.y);
	SERIAL_WRITE_VALUE("wz:", gyro_event.gyro.z);
	Serial.println();
	#endif // WRITE_THE_STUFF_TO_THE_SERIAL_PORT

	++loop_counter;
}
