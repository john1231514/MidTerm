/* 
 * Project MidTrem Room controller
 * Author: Johnathan
 * Date: 7/10/2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */
#include "Particle.h"
#include "IoTClassroom_CNM.h"
#include "neopixel.h"
#include "Encoder.h"
#include "Adafruit_BME280.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(MANUAL);

Button EncoderButton(D5);

Adafruit_BME280 bme ;
bool status;
float tempC;
float pressPA;
float humidRH;
float inHg;
float tempF;
int currentTime;
int last150millisSec;

const int hexAddress = 0x76;

const int OLED_RESET=-1;
Adafruit_SSD1306 display(OLED_RESET);

const int LEDPIN = 5;

// Run the application and system concurrently in separate threads
//SYSTEM_THREAD(ENABLED);


void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);

 status = bme . begin (hexAddress);
  if(status == false) {
    Serial.printf("BME280 at address 0x%02X failed to start", hexAddress);
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.display();
  delay(1000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("tempF %0.2f\n,humidRH %0.2f\n",tempF,humidRH);
  display.display();
  display.clearDisplay();

  pinMode(LEDPIN, INPUT_PULLDOWN);
}


void loop() {
  currentTime = millis ();
  if((currentTime-last150millisSec)>150) {
    last150millisSec = millis();
  tempC = bme.readTemperature();
    tempF = 1.8 * tempC + 32;
  pressPA = bme.readPressure();
    inHg= 0.000295 * pressPA + 4.77;
  humidRH = bme.readHumidity();
    Serial.printf("tempF %0.2f\n, inHg %0.2f\n,humidRH %0.2f\n",tempF ,inHg, humidRH);
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("tempF %0.2f\n,humidRH %0.2f\n",tempF,humidRH);
  display.display();
  display.clearDisplay();


}
