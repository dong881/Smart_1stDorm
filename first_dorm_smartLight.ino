#include <Adafruit_NeoPixel.h>
const int trigPin = 10;                  //Trig Pin
const int echoPin = 9;                  //Echo Pin
const int relayPin = 8;                 //Relay Pin
int scanDelayTime = 250; //每次掃描間隔(ms)
int old_ColorValue = 0; // 設定目前顏色
int FUN_MODE = 0; //情境模式
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN  5
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 60
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin (115200);             // Serial Port begin
  pinMode(relayPin, OUTPUT);      //Define inputs and outputs 
  pinMode(trigPin, OUTPUT);         
  pinMode(echoPin, INPUT);
  RELAY_SWITCH(true);
  /* WS2812 LED*/
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(150); // Set BRIGHTNESS to about 1/5 (max = 255)
}
String inStr = "";
const int HIGH_VAL = 105;
const int LOW_VAL = 55;
void loop() {
  int val = scan();
  switch(FUN_MODE){
    case 0:
      if(val < LOW_VAL) {
        RELAY_SWITCH(true);
        WS2812_LED_control(0);
      }
      else if(val >= LOW_VAL && val <=  HIGH_VAL){
        RELAY_SWITCH(true);
        WS2812_LED_control((val-LOW_VAL)*255/(HIGH_VAL - LOW_VAL));
      }
      else {
//        RELAY_SWITCH(false);
        theaterChase(strip.Color(255, 153, 18), 120); // orange, half brightness
      }
    break;
    case 1:
      rainbow(10);
      FUN_MODE = 0;
  }
  if (Serial.available() > 0) {
    inStr = Serial.readString();
//    Serial.println(inStr);
    if(inStr == "FUN") FUN_MODE = 1;
    else if(inStr == "ON") RELAY_SWITCH(true);
    else if(inStr == "OFF") RELAY_SWITCH(false);
    else Serial.println("error command");
  }
}
void RELAY_SWITCH(boolean mode){
  if(mode)digitalWrite(relayPin, HIGH);
  else    digitalWrite(relayPin, LOW);
}
int scan(void){
  long duration, cm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);     // 給 Trig 高電位，持續 10微秒
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);             // 讀取 echo 的電位
  duration = pulseIn(echoPin, HIGH);   // 收到高電位時的時間
  cm = (duration/2) / 29.1;         // 將時間換算成距離 cm
  Serial.println(cm);
  delay(scanDelayTime);
  return cm;
}

void WS2812_LED_control(int newValue){
  int stepVal = 12;
  if(newValue > old_ColorValue){
    for(int i=old_ColorValue; i<newValue; i+=stepVal)
      colorWipe(strip.Color(255-i,   i,   0), 0); // color , delay
  }else if(newValue < old_ColorValue){
    for(int i=old_ColorValue; i>newValue; i-=stepVal)
      colorWipe(strip.Color(255-i,   i,   0), 0); // color , delay
  }
  old_ColorValue = newValue;
}
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}
void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    strip.rainbow(firstPixelHue);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
