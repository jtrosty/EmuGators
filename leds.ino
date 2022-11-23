#include <Adafruit_NeoPixel.h>
#define PIN 11
#define NUMPIXELS 30
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int currentPixel = 0;
void setup()
{
  pixels.begin();
  randomSeed(analogRead(0));
  Serial.begin(19200);

}

void loop()
{

  byte red, green, blue;
  
  if(Serial.available() > 2){
    digitalWrite(5, HIGH);
    red = Serial.read();
    green = Serial.read();
    blue = Serial.read();
    pixels.setPixelColor(currentPixel % 30, pixels.Color(red, green, blue));
    pixels.show();
    currentPixel++;
  }
  

}
