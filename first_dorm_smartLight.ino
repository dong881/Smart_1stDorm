const int trigPin = 10;                  //Trig Pin
const int echoPin = 9;                  //Echo Pin
const int relayPin = 8;                 //Relay Pin
int scanDelayTime = 250; //每次掃描間隔(ms)

void setup() {
  Serial.begin (115200);             // Serial Port begin
  pinMode(relayPin, OUTPUT);      //Define inputs and outputs 
  pinMode(trigPin, OUTPUT);         
  pinMode(echoPin, INPUT);
  RELAY_SWITCH(true);
}
String inStr = "";
void loop() {
  scan();
  if (Serial.available() > 0) {
    inStr = Serial.readString();
    Serial.println(inStr);
    if(inStr == "ON") RELAY_SWITCH(true);
    else if(inStr == "OFF") RELAY_SWITCH(false);
    else Serial.println("error command");
  }
}
void RELAY_SWITCH(boolean mode){
  if(mode){
    digitalWrite(relayPin, HIGH);
//    Serial.println("Relay switch ON !!");
  }else{
    digitalWrite(relayPin, LOW);
//    Serial.println("Relay switch OFF !!");
  }
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
//  Serial.print("Distance : ");  
  Serial.println(cm);
//  Serial.print("cm\n");
  delay(scanDelayTime);
  return cm;
}
