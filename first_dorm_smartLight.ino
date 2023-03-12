#include <Adafruit_NeoPixel.h>
const int trigPin = 10;                  //Trig Pin
const int echoPin = 9;                  //Echo Pin
const int relayPin = 8;                 //Relay Pin
int scanDelayTime = 250; //每次掃描間隔(ms)
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN  5
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 60
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
String inStr = "";
void setup() {
  inStr.reserve(500);
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

const int HIGH_VAL = 95;
const int LOW_VAL = 55;
int leaveTimes = 0;
int sitDownTimes = 0;
int noOneTimes = 0;
int old_ColorValue = 0; // 設定目前顏色
boolean nowStateIsLight = true;
boolean ActiveWork = true;
int T = 0;
int val = 60;
int timer1 = 1;
int rangeLive = 200;
int timer2 = 1;
int rangeLive2 = 1000;
long lazyLED = 12345000;
int stateMachine = 1;

void loop() {
  if(timer1>rangeLive){ //一段時間更新一次
    int temp = scan();
    if(temp < 150) val = temp;
    timer1=0;
  }timer1++;
  if(timer2>rangeLive2){ //一段時間更新一次
    timer2=0;
  }timer2++;
  
  if(ActiveWork){ //判斷模式(Active: 我在位置工作 [準備偵測離開時關燈])
    lazyLED = 12345000;
     if(timer2 < (rangeLive2/2)){
      for(int x=0;x<20;x+=2) strip.setPixelColor(x, strip.Color(255, 153, 18));
      for(int x=1;x<20;x+=2) strip.setPixelColor(x, 0);
//      if(val < 20 ){
//        switch(stateMachine){
//          
//        }
//      }
    }else{
      for(int x=1;x<20;x+=2) strip.setPixelColor(x, strip.Color(255, 153, 18));
      for(int x=0;x<20;x+=2) strip.setPixelColor(x, 0);
    }
    strip.show();
    WS2812_LED_control((val-LOW_VAL)*255/(HIGH_VAL - LOW_VAL));
    
    if (val > 110){
        timer1 = rangeLive+1; //強迫更新資料
        sitDownTimes = 0;
        leaveTimes += 1;
        if (leaveTimes > 2){ 
            leaveTimes = 0;
            stateMachine = 1; //reset
            RELAY_SWITCH(false); 
        }
    }else leaveTimes = 0; // (0~120) 重置變數INIT value
        
  }else{ // not ActiveWork
      if(lazyLED > 0) lazyLED -= 1000;
      else lazyLED = 0;
      theaterChase(lazyLED, 240); // orange, half brightness
      if (val < 85)sitDownTimes += 1;
      else         sitDownTimes =0;
      if (sitDownTimes == 0){ // 完全沒有人坐下
          noOneTimes += 1;
          T = 5;
          if (noOneTimes > T+1) noOneTimes = T+1;
          // if (noOneTimes > T) 可執行剛離開後一下下是事件
      }else if (sitDownTimes > 2){ // 偵測到坐下 (確定觸發坐下指令)   
          sitDownTimes = 0;
          RELAY_SWITCH(true);   
      }else // sitDownTimes沒有持續等於0 就重置
          noOneTimes = 0;
          timer1 = rangeLive+1; //強迫更新資料
  }
//  if (Serial.available() > 0) {
//    inStr = Serial.readString();
////    Serial.println(inStr.substring(0,4));
//    if(inStr == "FUN") rainbow(1);
//    else if(inStr == "PON") POWER_ON();
//    else if(inStr == "POFF") POWER_OFF();
//    else if(inStr == "ON") RELAY_SWITCH(true);
//    else if(inStr == "OFF") RELAY_SWITCH(false);
//    else if(inStr.substring(0,4) == "data"){ // Set and Show LED data
////      Serial.println(inStr);
//      inStr = inStr.substring(4);
//      for(int i=0; i<60; i++) {
////        if(inStr == "") break;
//        strip.setPixelColor(i, inStr.substring(0,8).toInt());
////        Serial.println(inStr.substring(0,8));
//        inStr = inStr.substring(8);
//      }
//      strip.show(); // Update strip with new contents
//    }
//    else Serial.println("error command");
//  }
}

void RELAY_SWITCH(boolean mode){
  ActiveWork = mode;
  if(mode){
    digitalWrite(relayPin, HIGH);
    POWER_ON();
    nowStateIsLight = true;
    Serial.println("ON");
    delay(200);
    strip.setBrightness(160); // Set BRIGHTNESS to about 1/5 (max = 255)
  }else{
    POWER_OFF();
    digitalWrite(relayPin, LOW);
    nowStateIsLight = false;
    Serial.println("OFF");
    delay(200);
    strip.setBrightness(10); // Set BRIGHTNESS to about 1/5 (max = 255)
  }
}

void WS2812_LED_control(int newValue){
  if(newValue < 0) newValue = 0;
  if(newValue > 255) newValue = 255;
  int stepVal = 13;
  if(newValue > old_ColorValue){
    for(int i=old_ColorValue; i<newValue; i+=stepVal)
      colorWipe(strip.Color(255-i,   i,   0), 0); // color , delay
  }else if(newValue < old_ColorValue){
    for(int i=old_ColorValue; i>newValue; i-=stepVal)
      colorWipe(strip.Color(255-i,   i,   0), 0); // color , delay
  }
  old_ColorValue = newValue;
}
void POWER_ON(){
  strip.clear();
  strip.show();
  for(int i=0;i<60;i++){
    strip.setPixelColor(i, strip.Color(255-3*i, 153-2*i, 18+i));         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(5);
  }
//  delay(500);
}
void POWER_OFF(){
  for(int i=59;i>=0;i--){
    strip.setPixelColor(i, strip.Color(0,0,0));         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(5);
  }
//  delay(500);
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
//  delay(scanDelayTime);
  return cm;
}

void colorWipe(uint32_t color, int wait) {
  for(int i=20; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
void theaterChase(uint32_t color, int wait) {
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
void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 256) {
    strip.rainbow(firstPixelHue);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
