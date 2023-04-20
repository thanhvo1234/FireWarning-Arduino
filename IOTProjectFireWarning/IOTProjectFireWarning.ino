//Include the library files
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
/*New blynk app project*/
/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID "TMPLOaOwC1Dq"
#define BLYNK_DEVICE_NAME "FIREWARNING"
#define BLYNK_AUTH_TOKEN "MaLlzCn8bYNM51ALBkF1kDfIvp6Z6MEZ"
// firebase
#define DATABASE_URL "https://firewarning-2f183-default-rtdb.firebaseio.com/" // Firebase host
#define API_KEY "AIzaSyAOdkLLSrgn9NVQLBWrgdBPOPomRbRLbX0" //Firebase Auth code

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Tang 2T";//Enter your WIFI name
char pass[] = "1234567890";//Enter your WIFI password

FirebaseData fbdo;
FirebaseAuth AUTH;
FirebaseConfig config;


#define Green D6
#define Red D7
bool signupOK = false;
int buzzer=5; //D1
int warningLevel=0;
BlynkTimer timer;
int timerID1,timerID2;
int flame_value =A0;
int sensorvalue;
int button=0; //D3
boolean buttonState=HIGH;
boolean runMode=1;//Turn on/off warning system
boolean warningState=0;
String notification="";
WidgetLED led(V0);



//Get the button value
void handleTimerID1(){
  if(Firebase.ready() && signupOK){
    sensorvalue = analogRead(flame_value);
    Firebase.RTDB.setFloat(&fbdo, "Flame Sensor/Value",sensorvalue);  
    Serial.println(1024-sensorvalue);
    Blynk.virtualWrite(V1,1024 -sensorvalue);
    if(led.getValue()) {
      led.off();
    } else {
      led.on();
    }
    if(runMode==1){
        if(1024-sensorvalue>warningLevel){
          digitalWrite(buzzer,HIGH);
          Blynk.virtualWrite(V3,HIGH);
          digitalWrite(Green, LOW);
          digitalWrite(Red, HIGH);
          notification = "WARNING!! THE FIRE!!!";
          Firebase.RTDB.setString(&fbdo, "FIRE/STATUS", notification);
          Serial.println("Warning!!! THE FIRE!");
          }else{
            digitalWrite(buzzer,LOW);
            Blynk.virtualWrite(V3,LOW);
            digitalWrite(Green, HIGH);
            digitalWrite(Red, LOW);
            notification = "THE FIRE NORMAL";
            Firebase.RTDB.setString(&fbdo, "FIRE/STATUS", notification);
            Serial.println("THE FIRE IN NORMAL STATUS!");
          }
        }else{
          digitalWrite(buzzer,LOW);
          Blynk.virtualWrite(V3,LOW);
          digitalWrite(Green, HIGH);
          digitalWrite(Red, LOW);
          notification = "THE FIRE NORMAL";
          Firebase.RTDB.setString(&fbdo, "FIRE/STATUS", notification);
          Serial.println("THE FIRE IN NORMAL STATUS!");
        }
  }
}
void handleTimerID2(){
  warningState=0;
}
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V2,V4);
}
BLYNK_WRITE(V2) {
  warningLevel = param.asInt();
}
BLYNK_WRITE(V4) {
  runMode = param.asInt();
}

void setup() {
  //Set the LED pin as an output pin
  Serial.begin(115200);
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);
  pinMode(flame_value, INPUT);
  pinMode(button,INPUT_PULLUP);
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,LOW); //Turn off buzzer
  //Initialize the Blynk library
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timerID1 = timer.setInterval(1000L,handleTimerID1);

  // for firebase
  WiFi.begin(ssid, pass);
   Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &AUTH, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  //Run the Blynk library
  Blynk.run();
  handleTimerID1();
  timer.run();
  if(digitalRead(button)==LOW){
    if(buttonState==HIGH){
      buttonState=LOW;
      runMode=!runMode;
      Serial.println("Run mode: " + String(runMode));
      Blynk.virtualWrite(V4,runMode);
      delay(200);
    }
  }else{
    buttonState=HIGH;
  }
}