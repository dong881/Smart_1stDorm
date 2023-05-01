#include <Adafruit_NeoPixel.h>
/*
WORK_STATE
- common
  BRIGHTNESS 150
- SEITCH
  first part (0~19)
    - Fire_Mode()
  second part (20~LED_COUNT)
    - My distance with screen (RED to GREEN)
  Third part (Last 10 LEDs)
  input tomorrow wether state
    - RED / GREEN / BLUE

SLEEP_STETE
- commom
  BRIGHTNESS 10
  Fire_Mode()

NEED_LIGHT_STATE
- commom
  BRIGHTNESS 180
  Whhite_Mode()
  while(long time leave) => SLEEP MODE
*/
#define INIT_STATE 0
#define WORK_STATE 1
#define SLEEP_STATE 2
#define NEED_LIGHT_STATE 3

#define TURN_ON 1
#define TURN_OFF 0

const int scanDelayTime = 60; //每次掃描間隔(ms) 250

// I/O Pin
const int trigPin = 10;                 //Trig Pin
const int echoPin = 9;                  //Echo Pin
const int relayPin = 8;                 //Relay Pin

// 定義燈條接口和燈珠數量
#define LED_PIN 5
#define LED_COUNT 60

// 創建燈條對象
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// 設定 Timer 周期和計數器初值
const int TIMER_PERIOD_MS = 30;

void setup() {
  // 啟用 Timer1 中斷，並設定為 CTC 模式
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = TIMER_PERIOD_MS * 1000 / 64; // 設定比較值，使用 64 分頻
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11);
  TCCR1B |= (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  interrupts();

  // 初始化燈條
  strip.begin();
  strip.show(); // 關閉所有燈珠
  strip.setBrightness(150); // Set BRIGHTNESS to about 1/5 (max = 255)
  Serial.begin (115200);             // Serial Port begin

  // INITI/O Pin
  pinMode(relayPin, OUTPUT);      //Define inputs and outputs 
  pinMode(trigPin, OUTPUT);         
  pinMode(echoPin, INPUT);
  RELAY_SWITCH(TURN_ON);

}

int State_Machine = SLEEP_STATE;
int LastState = State_Machine;
int LED_RGB[3] = {0,0,0};

// 定義上下界
const int LOWER_THRESHOLD_DISTANCE = 80; // 下界設為 n 公分
const int UPPER_THRESHOLD_DISTANCE = 100; // 上界設為 n 公分
const int RED_DISTANCE = 55;
const int GREEN_DISTANCE = 85;

void loop() {
  int distanceVal = SCAN();
  CHANGE_STATE(distanceVal);
  if(LastState != State_Machine){ // run once time
    LastState = State_Machine;
    switch (State_Machine) {
    case WORK_STATE:
      RELAY_SWITCH(1);
      break;
    case SLEEP_STATE:
      RELAY_SWITCH(0);
      break;   
    }
  }
  switch (State_Machine) {
  case WORK_STATE:
    const int STEP = 8;
    int MAP_VAL = (distanceVal-RED_DISTANCE)*(255-STEP)/(GREEN_DISTANCE - RED_DISTANCE);
    MAP_VAL =  (MAP_VAL>(255-STEP)) ? (255-STEP) : (MAP_VAL < 0)? 0 : MAP_VAL;
    if(LED_RGB[1] != MAP_VAL){
      LED_RGB[1] = LED_RGB[1] + ((MAP_VAL > LED_RGB[1])?STEP:-1*STEP);
      LED_RGB[0] = 255 - LED_RGB[1];
    }
    break;
  }
}

// 定義 debounce 次數
const int DEBOUNCE_COUNT = 3;
// 宣告變數紀錄 debounce 次數
int debounceCount = 0;
// 宣告變數紀錄前一次判斷結果
bool lastResult = false;

void CHANGE_STATE(int distance){
  bool result = false;
  // 判斷是否符合上下界
       if (distance > UPPER_THRESHOLD_DISTANCE) result = true; // 符合上界
  else if (distance < LOWER_THRESHOLD_DISTANCE) result = false; // 符合下界
  
  // 當前判斷結果與前一次不同時，重置 debounceCount
  if (result != lastResult) debounceCount = 0;
  
  // 當前判斷結果與前一次相同時，且 debounceCount 還沒達到設定值時，增加 debounceCount
  if (result == lastResult && debounceCount < DEBOUNCE_COUNT) 
    debounceCount++;
  
  // 當 debounceCount 達到設定值時
  if (debounceCount == DEBOUNCE_COUNT) {
    if (result) {
      State_Machine = SLEEP_STATE;
    } else {
      State_Machine = WORK_STATE;
    }
    debounceCount = 0; // 重置 debounceCount
  }
  lastResult = result; // 更新前一次判斷結果
}

void RELAY_SWITCH(boolean mode){
  if(mode){
    digitalWrite(relayPin, HIGH);
    POWER_ON();
    Serial.println("ON");
    strip.setBrightness(160);
  }else{
    POWER_OFF();
    digitalWrite(relayPin, LOW);
    Serial.println("OFF");
    strip.setBrightness(10);
  }
}
void POWER_ON(){
  strip.clear();
  strip.show();
  for(int i=0;i<60;i++){
    strip.setPixelColor(i, strip.Color(255-3*i, 153-2*i, 18+i));         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(5);
  }
}
void POWER_OFF(){
  for(int i=59;i>=0;i--){
    strip.setPixelColor(i, strip.Color(0,0,0));         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(5);
  }
}
int SCAN(void){
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

// Timer1 比較匹配中斷服務程序
int SleepTime = 0;
ISR(TIMER1_COMPA_vect) {
  
  static uint32_t last_update_time_30ms = 0;
  static uint32_t last_update_time_SLOW = 0;
  uint32_t current_time = millis();
  
  if (current_time - last_update_time_30ms >= TIMER_PERIOD_MS) {
    last_update_time_30ms = current_time;
    switch (State_Machine) {
    case WORK_STATE:
      for (int i = 20; i < LED_COUNT; i++) 
        strip.setPixelColor(i,strip.Color(LED_RGB[0], LED_RGB[1], LED_RGB[2]));
      // 將顏色緩存發送到燈條
      strip.show();  
      break;
    }
  }
  /* Fire_Mode() */
  if (current_time - last_update_time_SLOW >= 900) {
    last_update_time_SLOW = current_time;
    int USE_RANGE = 0;
    switch (State_Machine) {
    case WORK_STATE:
      USE_RANGE = 20;
      SleepTime = 0;
      break;
    case SLEEP_STATE:
      USE_RANGE = LED_COUNT;
      SleepTime++;
      break;
    }
    if(SleepTime > 2000){ //After 30 min.
      SleepTime = 2000;
      strip.setPixelColor(random(USE_RANGE),strip.Color(0,0,0));
    }else{
      for(int i=0;i<USE_RANGE/4;i++){
        strip.setPixelColor(random(USE_RANGE),strip.Color(255, 5, 5));
        strip.setPixelColor(random(USE_RANGE),strip.Color(255, 69, 0));
      }
    }
    strip.show();
  }

}
