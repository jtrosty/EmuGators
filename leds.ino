
byte weatherMode;
int cloudPos;
void setup()
{
  weatherMode = 'x';
  cloudPos = 0;
  
  for (int i = 2; i < 10; i++){
    pinMode(i, OUTPUT);
  }

  randomSeed(analogRead(0));
  Serial.begin(9600);

}

void loop()
{
  for (int i = 2; i < 10; i++){
    digitalWrite(i, LOW);
  }
  
  if(Serial.available() > 0){
    byte readIn = Serial.read();
    if(readIn == '0' ||readIn == '1' || readIn =='2'){
      weatherMode = readIn;
    }
  }
  if(weatherMode == '1'){
    digitalWrite(random(2,10), HIGH);
    delay(100);
  }
  else if(weatherMode == '2'){
    digitalWrite((cloudPos + 2), HIGH);
    digitalWrite((cloudPos + 3 ), HIGH);
    digitalWrite((cloudPos + 4), HIGH);
    cloudPos = (cloudPos + 1) % 8;
    delay(250);
  }
  

}
