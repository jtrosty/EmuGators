#include <Adafruit_NeoPixel.h>
#define PIN 11
#define NUMPIXELS 30
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int currentPixel = 0;
bool reading = false;
void setup()
{
  pixels.begin();
  randomSeed(analogRead(0));
  Serial.begin(19200);

}

void loop()
{

  byte red, green, blue;
  
  if(Serial.available() > 0){
    if(!reading && Serial.read() == 255){
      reading = true;
    }
    else if(currentPixel >= NUMPIXELS){
      pixels.show();
      reading = false;
      currentPixel = 0;
    }
    else if (reading && Serial.available() > 2){
      red = Serial.read();
      green = Serial.read();
      blue = Serial.read();
      pixels.setPixelColor(currentPixel, pixels.Color(red, green, blue));
      pixels.show();
      currentPixel++;
    }
  }
  

}
