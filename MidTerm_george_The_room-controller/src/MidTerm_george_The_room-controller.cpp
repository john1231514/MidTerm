/* 
 * Project MidTrem Room controller
 * Author: Johnathan
 * Date: 7/10/2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

//This is where you include libraries of code you have downloaded. Make sure that if a library has multiple other libraries in it that do not include the library again.
#include "Particle.h"
#include "IoTClassroom_CNM.h"
#include "neopixel.h"
#include "Encoder.h"
#include "Adafruit_BME280.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(MANUAL);

//This is the header, as we call it, where you define what things are when those things don't already exist within the coding language.
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

const int LEDPIN1 = D4;
const int LEDPIN2 = D3;

const int MYWEMO=3;
const int MYWEMO2=0;

const int BULB4=4;
const int BULB3=3;
int color;

const int NEOPIXEL_COUNT = 12;

Adafruit_NeoPixel pixel(NEOPIXEL_COUNT,SPI1,WS2812B);
int position;
int pp;
int pEncp;
int EncPosition;
int neoLight;
void pixelFill(int color, int startp, int endp);
int manualState;

//this is where you setup everything as stated in "void.setup".
void setup() {
  //this is for the Serial monitor.
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);

 //this is just telling the photon 2 to see if the BME properly started up.
 status = bme . begin (hexAddress);
  if(status == false) {
    Serial.printf("BME280 at address 0x%02X failed to start", hexAddress);
  }

  //this turns on the wifi than tells it to forget the other wifi than tells it what wifi to connect to. than prints to Serial monitor when connected.
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

  // this is setting up one of the timers.
  if((currentTime-last5Seconds)>5000) {
        last5Seconds = millis();
  Serial.printf("\n\n");
  }

  //this is just telling the display to turn on.
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // this tells it to start displaying and than wait than 1 second and clear display.
  display.display();
  delay(1000);
  display.clearDisplay();

  // this sets text size, color, where to set the cursor, than tells it to print something to the display.
  //the display clear acts as a way to refresh the display so new info can be displayed.
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("tempF %0.2f\n,humidity %0.2f\n neoLight%i\n",tempF,humidity,neoLight);
  display.display();
  display.clearDisplay();

  //this is just telling the neopixels to turn on and how bright to be.
  pixel.begin();
  pixel.show();
  pixel.setBrightness(255);

  // this is telling the photon 2 what pin the encoder LED is connected to and what it does.
  pinMode(LEDPIN1, OUTPUT);

  pinMode(LEDPIN2, OUTPUT);

}

//All of this is the main code for everything 
void loop() {

//this telling the LED in the encoder to turn on throw what pin and how to turn on low and high put out different colors.



  // this is a timer for code that reads the BME which gives back temp,humidity,atmospheric pressure and converts it to units that most understand.  
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


//this is anothor timer which makes it so the code is not sent to the hue at a rate that makes the hue stop responding.
//the hue is are the lights.
if((currentTime-last1000millisSec)>1000) {
    last1000millisSec = millis();
    //this if statement tells the photon 2 to turn on and outlet if the temp is over 80F.
  if(tempF > 80 ) {
    Serial.printf("Turning on Wemo# %i\n Turn Hue 4 Red %i\n",MYWEMO, BULB4);
      setHue(BULB4,true,HueRed,(255),255);
        wemoWrite(MYWEMO,HIGH);
    }
    //this else statement is telling the photon 2 to turn said outlet off if it does not meet this if statement.
  else{
    Serial.printf("Turning off Wemo# %i\n Turn Hue 4 off %i\n",MYWEMO, BULB4);
      setHue(BULB4,false,HueRed,(255),255);
        wemoWrite(MYWEMO,LOW);
    }
    //this is telling another outlet to turn on if under 80F.
    if(tempF < 80 ) {
    Serial.printf("Turning on Wemo2# %i\n Turn Hue 3 Blue %i\n",MYWEMO2, BULB3);
      setHue(BULB3,true,HueBlue,(255),255);
      wemoWrite(MYWEMO2,HIGH);
    }
    //this else statement is telling the outlet to turn off if the if statement is not met. 
  else{
    Serial.printf("Turning off Wemo2# %i\n Turn Hue 3 off %i\n",MYWEMO2, BULB3);
      setHue(BULB3,false,HueBlue,(255),255);
        wemoWrite(MYWEMO2,LOW);
    }

  }

//this is code that makes it so the neopixel ring lights up based on the humidity in the room.
humidity = bme.readHumidity();

    if(humidity != phumidity){

    Serial.printf("humidity=%0.2f\n neoLight%i\n",humidity, neoLight);

    neoLight = 0.12*humidity;


   phumidity=humidity;

    }

  //this is pixelfill which is the function we saw earlier, it's used to tell the pixels what color to be and which one to start at and where the end point is.
  pixelFill(blue,0,neoLight);

  //this is an if statement to say when the botton iss pressed go into manual mode
  if(EncoderButton.isPressed()){
    manualState = !manualState;
    Serial.printf("button pressed");
  }
  //this is anothor if statement that tells the photon 2 what manual mode is.
  if(manualState) {
    Serial.printf("manualmode enabled");
    digitalWrite(LEDPIN1, HIGH);
    digitalWrite(LEDPIN2, LOW);
   position = myEnc.read();
     if(position != pp) {
       display.setTextSize(1);
       display.setTextColor(WHITE);
       display.setCursor(0,0);
       display.printf("----------------\ntempF:%0.2f\n----------------\nhumidity:%0.2f\n----------------\nneoLight:%i\n----------------\nMME%i",tempF,humidity,neoLight,manualState);
       display.display();
       display.clearDisplay();

       Serial.printf("Position=%i\n", position);

       pp=position;
     }

     if (position > 95) {
     position=95;
     myEnc.write(95);
     }
   
     if (position < 0) {
         position=0;
         myEnc.write(0);
     }

  
  }
  else{
    digitalWrite(LEDPIN2, HIGH);
    digitalWrite(LEDPIN1, LOW);



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
  display.printf("----------------\ntempF:%0.2f\n----------------\nhumidity:%0.2f\n----------------\nneoLight:%i\n----------------\nMME%i",tempF,humidity,neoLight,manualState);
  display.display();
  display.clearDisplay();
  }

      position = myEnc.read();

    if(position != pp){

    Serial.printf("position=%i\n", position);

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.printf("----------------\ntempF:%0.2f\n----------------\nhumidity:%0.2f\n----------------\nneoLight:%i\n----------------\nMME%i",tempF,humidity,neoLight,manualState);
    display.display();
    display.clearDisplay();

    pp=position;

    }

    if (position > 95) {
      position=95;
      myEnc.write(95);
    }
    
    if (position < 0) {
        position=0;
        myEnc.write(0);
    }

  pixel.clear();

}

// makes a function for pixelfill.
void pixelFill(int color, int startp, int endp) {
  int i;
  for(i = startp; i < endp; i++) {
    pixel.setPixelColor(i, color);
    pixel.show();
  }
}
