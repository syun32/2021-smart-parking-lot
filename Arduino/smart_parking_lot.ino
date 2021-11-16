#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <time.h>
#include <stdio.h>


// 0x3F I2C 주소를 가지고 있는 16x2 LCD객체를 생성합니다.(I2C 주소는 LCD에 맞게 수정해야 합니다.)
LiquidCrystal_I2C lcd(0x27, 16, 2); // 메가의 경우 ic2는 20 21 번 핀에 연결한다 

#define PIN1            7 // 아래 14개
#define PIN2            6 // 위 14개
#define PIN3            5 // 측면 위 8개 
#define PIN4            4 // 측면 아래 4개 

Adafruit_NeoPixel pixels1 = Adafruit_NeoPixel(14, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(14, PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels3 = Adafruit_NeoPixel(8, PIN3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels4 = Adafruit_NeoPixel(4, PIN4, NEO_GRB + NEO_KHZ800); // 네오픽셀 설정 

int RED_num=0;
int GREEN_num=120;
int BLUE_num=0;  // led 기본색 

int servoPin_in=50;
int servoPin_out=51;
Servo servo_in;
Servo servo_out;
int Level_in=180;
int Level_out=0;// 주차장 입구 서보핀 50 출구 서보핀 51 초기값 = 0


int trigPin_1=24;
int echoPin_1=25;
int trigPin_2=26;
int echoPin_2=27;
int trigPin_3=28;
int echoPin_3=29;
int trigPin_4=30;
int echoPin_4=31;
int trigPin_5=32;
int echoPin_5=33;
int trigPin_6=34;
int echoPin_6=35; // 1~6번 주차자리 

int trigPin_in=36;
int echoPin_in=37;
int trigPin_out=38;
int echoPin_out=39; // 들어오는 차량 감지 나가는 차량 감지 

#define BT_RXD 52
#define BT_TXD 53 
SoftwareSerial ESP_wifi(BT_RXD, BT_TXD);//AT 통신 설정 
#define DEBUG true
double  parking_distance[8]; // 배열의 형태로 주차 거리 저장  (1~6 + in + out)
bool parking_bool[8]={false,false,false,false,false,false,false,false}; // 배열의 형태로 차량 여부 저장  (0~7 + in + out) false로 초기화 
bool parking_bool_set[8]={false,false,false,false,false,false,false,false}; // 이전의 데이터를 저장해 주차 여부 비교를 위한 값 
double distance = 10; // 주차여부 판단 기준 거리 
unsigned long parking_time[6]={0,0,0,0,0,0};

bool parking_waiting = false; // 현재 주차중인 차량이 이동중인가?
int sum_car=0; // 현재 주차된 차량의 수 
int now_parking_waiting=6; // 현재 이동중인  주차차량 이동 장소 저장 초기화 값 6
int now_parking_start=6; // 최근에 주차차량 이동 장소 저장 초기화 값 6
int out_car_num=6; 
void setup() {
  Serial.begin(9600); // 시리얼 통신 설정

  lcd.init(); // I2C LCD를 초기화
  lcd.backlight(); // I2C LCD 백라이트 on

  pinMode(trigPin_1,OUTPUT);
  pinMode(echoPin_1,INPUT);
  pinMode(trigPin_2,OUTPUT);
  pinMode(echoPin_2,INPUT);
  pinMode(trigPin_3,OUTPUT);
  pinMode(echoPin_3,INPUT);
  pinMode(trigPin_4,OUTPUT);
  pinMode(echoPin_4,INPUT);
  pinMode(trigPin_5,OUTPUT);
  pinMode(echoPin_5,INPUT);
  pinMode(trigPin_6,OUTPUT);
  pinMode(echoPin_6,INPUT);  // 1~6 주차장 자리 초음파 센서 설정 
  pinMode(trigPin_in,OUTPUT);
  pinMode(echoPin_in,INPUT);
  pinMode(trigPin_out,OUTPUT);
  pinMode(echoPin_out,INPUT); //주차장 입차 출차 초음파 센서 설정 

  pixels1.begin(); 
  pixels2.begin();
  pixels3.begin();
  pixels4.begin(); // 네오픽셀 설정 

  ESP_wifi.begin(9600);
  ESP_wifi.setTimeout(5000); // wifi 설정 

  sendData("AT+RST\r\n", 2000, DEBUG); //모듈을 리셋
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG); //dual mode로 설정
  sendData("AT+CWJAP=\"AndroidHotspot1145\",\"qldtls12\"\r\n", 7000, DEBUG); //사용할 공유기 설정
  sendData("AT+CIPMUX=1\r\n", 7000, DEBUG); //mux=1
  sendData("AT+CIPSTART=1,\"TCP\",\"54.180.52.108\",9000\r\n", 5000, DEBUG);  // 1채널,접속 

}

void loop() {
  
  for (int i=0;i<8;i++){
    digitalWrite(i*2+24,HIGH);
    delayMicroseconds(10);
    digitalWrite(i*2+24,LOW);

    double duration=pulseIn(i*2+25,HIGH);
    parking_distance[i] = duration *340/10000/2;
    Serial.println(i+1);
    Serial.println(parking_distance[i]);
    Serial.println((int)parking_time[i]);
    delay(500);
   } // 1~6주차 자리 in, out 거리 저장 

   
  for (int i=0;i<8;i++){
    parking_bool_set[i]=parking_bool[i];
   } // 복사 

  
  for (int i=0;i<8;i++){ // 주차여부 판단 
    if(parking_distance[i]>distance){ // 기준거리보다 주차 거리가 길면 주차가 되어 있지 않음을 나타냄 
       parking_bool[i]=false;
    }
    else{ //기준거리보다 주차 거리가 짧으면 주차가 되어 있음을 나타냄 
      parking_bool[i]=true;
    }
   } // 0~7주차 자리 in, out 주차 여부 저장 

  char s[24]="1|0|2|0|3|0|4|0|5|0|6|0|";
  for (int i=0;i<6;i++){ 
     if(parking_bool[i]==true){
       s[4*i+2]='1';  
     }
     else{
       s[4*i+2]='0'; 
     }
   } 
   s[23]='\n'; 
   Serial.println(s);

   sendData("AT+CIPSEND=1,24\r\n", 1000, DEBUG);  // 24크기의 메세지 
   ESP_wifi.print(s); // 보냄 
   delay(1000); // 대기
   // 0~6 주차 여부 전달
   int sum_car=0;

   for(int i=0;i<6;i++){ // 주차 자리합산 
    if(parking_bool[i]){
      sum_car++;
      }
    }
    
   bool change_bool_set[8]={false,false,false,false,false,false,false,false};


   for (int i=0;i<8;i++){ // 변화 확인 모드 
    if(parking_bool_set[i]!=parking_bool[i]){ //이전값과 새로운 값의 변화가 생기면 
        change_bool_set[i]=true; // 이후 수행이 필요함 
      }
     else{
        change_bool_set[i]=false; // 이후 수행이 필요 없음 
     }
   } 
  for (int i=0;i<6;i++){
        if(change_bool_set[i]==true)
        out_car_num=i;
  } // 나간 차량 기억 
  
  for (int i=0;i<8;i++){ // 수행 모드
    if(change_bool_set[i] && parking_bool[i]==true&& i<6){ // 주차 자리에 차량이 들어옴 
       Serial.println(i+1);
       Serial.println("car in!");
       parking_time[i]= millis(); // 차량 입력 시간 저장 
       
       if(now_parking_waiting==i){
         Serial.println("and waiting");
         parking_waiting = false; // 대기중인 차량의 주차 완료
         now_parking_waiting=6; // 대기중인 차량 초기화  
         ALL_clear(); // 전체 등 제거 
         delay(500);
         if(now_parking_start!=6){
            num_set(now_parking_start+1); // 현재 주차중인 차량의 등을 다시 켜줌
         }
       }
       else{
        ALL_clear(); // 전체 등 제거 
        delay(500);
       }
       // 주차 완료 시 주차유도등 제거 후 주차 완료 데이터 송수신 
    }
    if(change_bool_set[i] && parking_bool[i]==false && i<6){ // 주차 자리에 차량이 나감 
       Serial.println(i+1);
       Serial.println("car out!");
      // 주차 차량이 나가면 주차 차량 나감 데이터 송수신
    }
    if(change_bool_set[i] && parking_bool[i]==true && i==6){ // 입차 자리에 차량이 새로 들어옴 
      if(sum_car==6){ // 주차장이 만차이면 아무일도 일어나지 않음 
        Serial.println("full!");
        }
      else{ // 만차가 아니라면 우선순위에 맞게 주차유도를 한 후 차단기를 열어준다 
         LED_ON();
         servo_in_open();
       }
    }
    if(change_bool_set[i] && parking_bool[i]==false && i==6){ // 입차 자리에 차량이 사라짐
      // 차단기를 닫음 
      servo_in_close();
    }
    
    if(change_bool_set[i] && parking_bool[i]==true && i==7){ // 출차 자리에 차량이 새로 들어옴 
      //데이터 송수신후 차단기 열어줌 
      lcd.setCursor(0,0);           // 0번째 줄 0번째 셀부터 입력하게 합니다.
      lcd.print("money:"); 
      lcd.setCursor(0,1);
      char s[15];
      int money=0;
      money=(int)millis()-parking_time[out_car_num];
      Serial.println(out_car_num+1);
      Serial.println(money);
 
      itoa(money,s,10);
      lcd.print(s);
      out_car_num=6;   
      servo_out_open();
    }
    if(change_bool_set[i] && parking_bool[i]==false && i==7){ // 출차 자리에 차량이 사라짐
      //차단기를 닫음 
      lcd.clear();
      servo_out_close();
    }
  }
}

void servo_in_open(){
  servo_in.attach(servoPin_in);
   for(Level_in=180 ;Level_in>90;Level_in--)
  {
    servo_in.write(Level_in);
    delay(20);
  }
  delay(100);
  servo_in.detach();
}  // 주차장 입차 차단기 열기 180->90
void servo_in_close(){
  servo_in.attach(servoPin_in);
  for(Level_in=90;Level_in<180;Level_in++)
  {
    servo_in.write(Level_in);
    delay(20);
  }
  delay(100);
  servo_in.detach();
} // 주차장 입차 차단기 닫기 90->180

void servo_out_open(){
  servo_out.attach(servoPin_out);
   for(Level_out=0;Level_out<90;Level_out++)
  {
    servo_out.write(Level_out);
    delay(20);
  }
  delay(100);
  servo_out.detach();
}  // 주차장 출차 차단기 열기 0->90
void servo_out_close(){
  servo_out.attach(servoPin_out);
  for(Level_out=90;Level_out>0;Level_out--)
  {
    servo_out.write(Level_out);
    delay(20);
  }
  delay(100);
  servo_out.detach();
} // 주차장 출차 차단기 닫기 90->0

void LED_ON(){
  int num_list[6]={2,5,1,4,0,3}; // 주차 우선 순위 거꾸로
  for(int i=0;i<6;i++)
  {
    if (parking_bool[num_list[i]]== false && now_parking_waiting!=num_list[i]){ // 해당 자리에 주차가 되어있지 않으면 now_parking_start로 옮김 + 해당자리로 주차중이지 않으면 
        now_parking_start=num_list[i];
      }
  }
  Serial.println(parking_waiting);
  if (parking_waiting==true){// 현재 주차차량 중첩 상태 
     num_set_double(now_parking_waiting+1,now_parking_start+1);
    }
  else{ // 차량 1대만 주차 유도 상태 
    num_set(now_parking_start+1); // now_parking_start, 우선순위가 가장 높은 자리로 주차 유도 
    parking_waiting = true; // 현재 주차차량이 이동중임을 알림 
    now_parking_waiting=now_parking_start; // 현재 주차차량 이동 장소 저장 
    now_parking_start=6;
  }
}


void num_set(int parking_num){
  if (parking_num<=3){
    for(int i=0;i<19-parking_num*5;i++){
    pixels2.setPixelColor(i, pixels2.Color(RED_num,GREEN_num,BLUE_num));  
    pixels2.show();
    }
    for(int i=0;i<8;i++){
    pixels3.setPixelColor(i, pixels3.Color(RED_num,GREEN_num,BLUE_num));  
    pixels3.show();
    }
  }
  else{
    for(int i=0;i<34-parking_num*5;i++){
    pixels1.setPixelColor(i, pixels1.Color(RED_num,GREEN_num,BLUE_num));  
    pixels1.show();
    }
  }
  for(int i=0;i<4;i++){
    pixels4.setPixelColor(i, pixels4.Color(RED_num,GREEN_num,BLUE_num));  
    pixels4.show();
  }
} //1~6까지 주차 유도 

void num_set_double(int parking_num1,int parking_num2){  // 선행 주차자리 , 후위 주차자리
  if (parking_num1<=3&&parking_num2<=3){
    for(int i=0;i<19-parking_num1*5;i++){
       pixels2.setPixelColor(i, pixels2.Color(RED_num,GREEN_num,BLUE_num));  
       pixels2.show();
    }
    
    int color_tmp=RED_num;
    RED_num=GREEN_num;
    GREEN_num=BLUE_num;
    BLUE_num=color_tmp; // LED 스트립 색상 sweep
    
    for(int i=0;i<19-parking_num2*5;i++){
       pixels2.setPixelColor(i, pixels2.Color(RED_num,GREEN_num,BLUE_num));  
       pixels2.show();
    }
    for(int i=0;i<8;i++){
        pixels3.setPixelColor(i, pixels3.Color(RED_num,GREEN_num,BLUE_num));  
        pixels3.show();
    }
    for(int i=0;i<4;i++){
        pixels4.setPixelColor(i, pixels4.Color(RED_num,GREEN_num,BLUE_num));  
        pixels4.show();
    }
  }
  if (parking_num1<=3&&parking_num2>3){
    for(int i=0;i<19-parking_num1*5;i++){
        pixels2.setPixelColor(i, pixels2.Color(RED_num,GREEN_num,BLUE_num));  
        pixels2.show();
    }
    for(int i=0;i<8;i++){
        pixels3.setPixelColor(i, pixels3.Color(RED_num,GREEN_num,BLUE_num));  
        pixels3.show();
    }
    
    int color_tmp=RED_num;
    RED_num=GREEN_num;
    GREEN_num=BLUE_num;
    BLUE_num=color_tmp; // LED 스트립 색상 sweep
    
    for(int i=0;i<34-parking_num2*5;i++){
        pixels1.setPixelColor(i, pixels1.Color(RED_num,GREEN_num,BLUE_num));  
        pixels1.show();
    }
    for(int i=0;i<4;i++){
        pixels4.setPixelColor(i, pixels4.Color(RED_num,GREEN_num,BLUE_num));  
        pixels4.show();
    }
  }
  if (parking_num1>3&&parking_num2<=3){
    for(int i=0;i<34-parking_num1*5;i++){
        pixels1.setPixelColor(i, pixels1.Color(RED_num,GREEN_num,BLUE_num));  
       pixels1.show();
    }
    int color_tmp=RED_num;
    RED_num=GREEN_num;
    GREEN_num=BLUE_num;
    BLUE_num=color_tmp; // LED 스트립 색상 sweep
    for(int i=0;i<19-parking_num2*5;i++){
       pixels2.setPixelColor(i, pixels2.Color(RED_num,GREEN_num,BLUE_num));  
       pixels2.show();
    }
    for(int i=0;i<8;i++){
       pixels3.setPixelColor(i, pixels3.Color(RED_num,GREEN_num,BLUE_num));  
       pixels3.show();
    }
    for(int i=0;i<4;i++){
       pixels4.setPixelColor(i, pixels4.Color(RED_num,GREEN_num,BLUE_num));  
       pixels4.show();
    }
  }
  else{
    for(int i=0;i<34-parking_num1*5;i++){
       pixels1.setPixelColor(i, pixels1.Color(RED_num,GREEN_num,BLUE_num));  
       pixels1.show();
    }
    int color_tmp=RED_num;
    RED_num=GREEN_num;
    GREEN_num=BLUE_num;
    BLUE_num=color_tmp; // LED 스트립 색상 sweep
    for(int i=0;i<34-parking_num2*5;i++){
    pixels1.setPixelColor(i, pixels2.Color(RED_num,GREEN_num,BLUE_num));  
    pixels1.show();
    }
    for(int i=0;i<4;i++){
    pixels4.setPixelColor(i, pixels4.Color(RED_num,GREEN_num,BLUE_num));  
    pixels4.show();
    }
  }
}

void ALL_clear(){
  pixels1.clear();
  pixels2.clear();
  pixels3.clear();
  pixels4.clear();
  pixels1.show();
  pixels2.show();
  pixels3.show();
  pixels4.show();
}

void send_string(char getStr[]){
  
  sendData("AT+CIPSTART=1,\"TCP\",\"54.180.52.108\",9000\r\n", 5000, DEBUG);  // 1채널,접속 
  sendData("AT+CIPSEND=1,24\r\n", 1000, DEBUG);  // 24크기의 메세지 
  ESP_wifi.print(getStr); // 보냄 
  sendData("AT+CIPCLOSE=1\r\n", 3000, DEBUG); // 접속종료 
  delay(3000); // 대기

}


String sendData(String command, const int timeout, boolean debug){
  String response = "";
  ESP_wifi.print(command); //command를 ESP8266에 보냄
  long int time=millis();
  
  while((time+timeout)>millis()){
    while(ESP_wifi.available()){
      //esp가 가진 데이터를 시리얼 모니터에 출력하기 위함
      char c=ESP_wifi.read(); //다음 문자를 읽어옴
      response+=c;
    }
  }
  if(debug){
    Serial.print(response);
  }
 
  return response;
}
