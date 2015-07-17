
/*

* SD card attached to SPI bus as follows:
** MOSI - pin 11
** MISO - pin 12
** CLK - pin 13
** CS - pin 4

*/

#include <DS1307.h>
#include <SD.h>
#include <SPI.h>
#include <Streaming\Streaming.h>
#include "DHT\DHT.h"
#include "string.h"

#define DHTPIN			2				// what pin we're connected to
#define DHTTYPE			DHT22			// DHT 22  (AM2302)
#define RTC_SCL			A5				// Serial Clock for TWI protocol
#define RTC_SDA			A4				//Data line for TWI protocol



#define ERROR_TEMPERATURE		1
#define	ERROR_HUMIDITY			2

#define SAMPLE_DIVIDER			10


//Module RTC DS1307 connected to A4 and A5 ports
DS1307 rtc(RTC_SDA, RTC_SCL);
DHT dht(DHTPIN, DHTTYPE);


float humidity;
float temperature;

String	date;
String	time;
String  day, month, year;
String  sec, min, hour;

boolean validate = false;

long int counter = 0;
const int chipSelect = 4;


void setup()
{
	Serial.begin(9600);

	while (!Serial){ ; } // only for leonardo board

	pinMode(10, OUTPUT);
	Serial.print("Initializing SD card...");
	// see if the card is present and can be initialized:
	if (!SD.begin(chipSelect))
	{
		Serial.println("Card failed, or not present");
		return; // don't do anything more:
	}
	Serial.println("card initialized.");

	dht.begin(); //initialize dht sensor
	
	rtc.halt(false); //set the clock

	//rtc.setDOW(FRIDAY);      //Define the day of the week
	//rtc.setTime(16, 18, 40);     //Define the time
	//rtc.setDate(10,07, 2015);   //Define the date

	//Definicoes do pino SQW/Out
	rtc.setSQWRate(SQW_RATE_1);
	rtc.enableSQW(true);
	// Open serial communications and wait for port to open:
	
}

void loop()
{
	
	getDate();
	delay(10);
	getTime();
	delay(10);

	// The sample interval is obtained changing the next line code.
	// You must match sec.toInt(), min.toInt() or hour.toInt() functions with the constant SAMPLE_DIVIDER to get the interval wished.
	if (((sec.toInt()) % SAMPLE_DIVIDER) == 0) //This will be true several times due to the clock of uController.
	{
		validate = true;
		counter++;
	}
	else
	{
		validate = false;
		counter = 0;
	}
	

	if ((validate) && (counter == 1)) // this statement will be true once within the sample interval set.
	{                                 
		unsigned int codeReturn;

		codeReturn = getTemperature();
		delay(10);
		if (codeReturn)
			Serial << "Erro ao obter a temperatura" << endl;

		codeReturn = getHumidity();
		if (codeReturn)
			Serial << "Erro ao obter a humidade" << endl;

		File myData = SD.open(rtc.getMonthStr(FORMAT_SHORT), FILE_WRITE);
		

		if (myData)
		{		
			Serial << year << month << day << "," << hour << min << sec << "," << temperature << "," << humidity << endl;
			delay(10);
			myData << year << month << day << "," << hour << min << sec << "," << temperature << "," << humidity << endl;
			delay(10);
			myData.close();
			delay(10);
		}

		else
		{
			Serial << "Error writting on SD" << endl;
		}
	}
	
}


void getDate(void)
{
	date = rtc.getDateStr(FORMAT_SHORT);
	day = date.substring(0, 2);
	month = date.substring(3, 5);
	year = date.substring(6, 8);
}

void getTime(void)
{
	time = rtc.getTimeStr();
	hour = time.substring(0, 2);
	min = time.substring(3, 5);
	sec = time.substring(6, 8);
}

int getTemperature(void)
{
	temperature = dht.readTemperature();

	if (isnan(temperature))
		return ERROR_TEMPERATURE;
	return 0;
}

int getHumidity(void)
{
	humidity = dht.readHumidity();

	if (isnan(humidity))
		return ERROR_HUMIDITY;
	return 0;
}

