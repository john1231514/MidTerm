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

Encoder myEnc(D8,D9);

Adafruit_BME280 bme ;
bool status;
float tempC;
float pressPA;
float humidity;
float phumidity;
float inHg;
float tempF;
int currentTime;
int last150millisSec;

int last1000millisSec;

const int hexAddress = 0x76;
int last5Seconds;

const int OLED_RESET=-1;
Adafruit_SSD1306 display(OLED_RESET);

const int LEDPIN = D4;

const int MYWEMO=3;
const int MYWEMO2=0;

const int BULB4=4;
const int BULB3=3;
int color;

const int NEOPIXEL_COUNT = 12;

Adafruit_NeoPixel pixel(NEOPIXEL_COUNT,SPI1,WS2812B);
int position;
int pp;
int neoLight;
void pixelFill(int color, int startp, int endp);


// Run the application and system concurrently in separate threads
//SYSTEM_THREAD(ENABLED);


void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);

 status = bme . begin (hexAddress);
  if(status == false) {
    Serial.printf("BME280 at address 0x%02X failed to start", hexAddress);
  }

  WiFi.on();
  WiFi.clearCredentials();
  WiFi.setCredentials("IoTNetwork");

  WiFi.connect();
  while(WiFi.connecting()) {
    currentTime = millis();
    if((currentTime-last5Seconds)>5000) {
        last5Seconds = millis();
        Serial.printf("connecting");
    }
  }

  if((currentTime-last5Seconds)>5000) {
        last5Seconds = millis();
  Serial.printf("\n\n");
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.display();
  delay(1000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("tempF %0.2f\n,humidity %0.2f\n neoLight%0.2f\n",tempF,humidity,neoLight);
  display.display();
  display.clearDisplay();

  pixel.begin();
  pixel.show();
  pixel.setBrightness(255);

  pinMode(LEDPIN, OUTPUT);



}


void loop() {

digitalWrite(LEDPIN, LOW);



  currentTime = millis ();
  if((currentTime-last150millisSec)>150) {
    last150millisSec = millis();
  tempC = bme.readTemperature();
    tempF = 1.8 * tempC + 32;
  pressPA = bme.readPressure();
    inHg= 0.000295 * pressPA + 4.77;
  humidity = bme.readHumidity();
    Serial.printf("---\ntempF %0.2f\n---\ninHg %0.2f\n---\nhumidity %0.2f\n",tempF ,inHg, humidity);
  }

if((currentTime-last1000millisSec)>1000) {
    last1000millisSec = millis();
  if(tempF > 80 ) {
    Serial.printf("Turning on Wemo# %i\n Turn Hue 4 Red %i\n",MYWEMO, BULB4);
      setHue(BULB4,true,HueRed,(255),255);
        wemoWrite(MYWEMO,HIGH);
    }
  else{
    Serial.printf("Turning off Wemo# %i\n Turn Hue 4 off %i\n",MYWEMO, BULB4);
      setHue(BULB4,false,HueRed,(255),255);
        wemoWrite(MYWEMO,LOW);
    }

    if(tempF < 80 ) {
    Serial.printf("Turning on Wemo2# %i\n Turn Hue 3 Blue %i\n",MYWEMO2, BULB3);
      setHue(BULB3,true,HueBlue,(255),255);
      wemoWrite(MYWEMO2,HIGH);
    }
  else{
    Serial.printf("Turning off Wemo2# %i\n Turn Hue 3 off %i\n",MYWEMO2, BULB3);
      setHue(BULB3,false,HueBlue,(255),255);
        wemoWrite(MYWEMO2,LOW);
    }

  }

humidity = bme.readHumidity();

    if(humidity != phumidity){

    Serial.printf("humidity=%0.2f\n neoLight%i\n",humidity, neoLight);

    neoLight = 0.12*humidity;


   phumidity=humidity;

    }

  pixelFill(blue,0,neoLight);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("----------------\ntempF:%0.2f\n----------------\nhumidity:%0.2f\n----------------\nneoLight:%i\n----------------",tempF,humidity,neoLight);
  display.display();
  display.clearDisplay();

  pixel.clear();

}

void pixelFill(int color, int startp, int endp) {
  int i;
  for(i = startp; i < endp; i++) {
    pixel.setPixelColor(i, color);
    pixel.show();
  }
}
