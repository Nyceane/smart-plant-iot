
#include "Arduino.h"
#include "Board.h"
#include "Helium.h"
#include "HeliumUtil.h"
#include <TH02_dev.h>
#include "Arduino.h"
#include "Wire.h"
#include <SeeedGrayOLED.h>
#include <avr/pgmspace.h>

#define CHANNEL_NAME "Azure IoT App"

Helium  helium(&atom_serial);
Channel channel(&helium);
int relay = 5;

void setDisplayToOriginalState()
{
  SeeedGrayOled.init(SSD1327);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  delay(150);
  /* Reset HP20x_dev */
  TH02.begin();
  delay(100);
  Serial.println("TH02_dev is available.\n");
  DBG_PRINTLN(F("Starting"));

  // Begin communication with the Helium Atom
  // The baud rate differs per supported board
  // and is configured in Board.h
  helium.begin(HELIUM_BAUD_RATE);

  // Connect the Atom to the Helium Network
  helium_connect(&helium);

  // Begin communicating with the channel. This should only need to
  // be done once. The HeliumUtil functions add simple retry logic
  // to re-create a channel if it disconnects.
  channel_create(&channel, CHANNEL_NAME);
  Wire.begin();
}

void loop() {

  //Sound Pollution
  int moisture = 0;
  for (int i = 0; i < 32; i++)
  {
    moisture += analogRead(A0);
  }

  int uvlight = 0;
  for (int i = 0; i < 32; i++)
  {
    uvlight += analogRead(A1);
  }

  float temper = TH02.ReadTemperature();
  float humidity = TH02.ReadHumidity();


  String dataString = "Moisture=" + String(moisture) + "&UVLight=" + String(uvlight) + "&Temperature=" + String(temper) + "&Humidity=" + String(humidity);
  char data[dataString.length()];
  dataString.toCharArray(data, dataString.length());
  channel_send(&channel, CHANNEL_NAME, data, strlen(data));
  Serial.println(data);

  setDisplayToOriginalState();
  SeeedGrayOled.clearDisplay();     //Clear Display.
  SeeedGrayOled.setNormalDisplay(); //Set Normal Display Mode
  SeeedGrayOled.setVerticalMode();  // Set to vertical mode for displaying text
  SeeedGrayOled.setTextXY(0, 0);          //Set the cursor to 0th line, 0th Column
  String moisturestring = "Moisture: " + String(moisture);
  char moibuffer[moisturestring.length()];
  moisturestring.toCharArray(moibuffer, moisturestring.length());
  SeeedGrayOled.putString(moibuffer);

  SeeedGrayOled.setTextXY(2, 0);
  String uvstring = "UVLight: " + String(uvlight);
  char uvbuffer[uvstring.length()];
  uvstring.toCharArray(uvbuffer, uvstring.length());
  SeeedGrayOled.putString(uvbuffer);

  SeeedGrayOled.setTextXY(4, 0);
  String temperaturestring = String(temper) + " C";
  char tempbuffer[temperaturestring.length()];
  temperaturestring.toCharArray(tempbuffer, temperaturestring.length());
  SeeedGrayOled.putString(tempbuffer);

  SeeedGrayOled.setTextXY(6, 0);
  String humidstring = "Humid: " + String(humidity);
  char humidbuffer[temperaturestring.length()];
  humidstring.toCharArray(humidbuffer, humidstring.length());
  SeeedGrayOled.putString(humidbuffer);

  if(moisture < 100)
  {
    digitalWrite(relay, HIGH);
    delay(5000);
    digitalWrite(relay, LOW);
  }
  delay(60000);
}
