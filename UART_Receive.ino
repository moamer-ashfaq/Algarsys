#define RXD2 16
#define TXD2 17

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <WiFiManager.h> 
 
 
#define FIREBASE_HOST "https://espdata-3a9ee-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "8ZzOg3zBPceVgWaJQipgFnaHmWeFzipxAAO4uHOd"



FirebaseData firebaseData;
FirebaseJson json;

String message = "";
String to_firebase="";
int sensor;


void setup() {
  // put your setup code here, to run once:
WiFiManager wm;
wm.setDebugOutput(false);

wm.resetSettings();

WiFiManagerParameter custom_text_box("my_text","MASTER MAC Address","80:7D:3A:6B:4A:A7",50);
wm.addParameter(&custom_text_box);

Serial.begin(115200);
Serial2.begin(9600,SERIAL_8N1, RXD2 , TXD2);
WiFi.mode(WIFI_STA);
bool res;
res = wm.autoConnect("TestAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi  
        Serial.println("Wifi Connected");  
        Serial.println("IP Address: ");
        Serial.println(WiFi.localIP());
    }


    Serial.println("Custom Text Box Entry: ");
    Serial.println(custom_text_box.getValue());
    pinMode(19,OUTPUT);
  /*Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();*/
 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
 
  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
 
  /*
  This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
  Firewall that allows only GET and POST requests.
  
  Firebase.enableClassicRequest(firebaseData, true);
  */
 
  //String path = "/data";
  
 
  Serial.println("------------------------------------");
  Serial.println("Connected to firebase...");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(Serial2.available())
  { Serial.println("recieved");
    while(Serial2.available()){
    message += char(Serial2.read());
    sensor = message.toInt();
    delay(5);}
    delay(10);
   Serial.println(sensor);
    //to_firebase = message;
   // to_firebase =int(to_firebase);
   // Serial.println(first);
    if(sensor == 0){
      
      digitalWrite(19,0);
      }
   else{
    digitalWrite(19,1);
    }
    delay(10);
    
   
    }
  
  json.set("data",sensor);
  Firebase.updateNode(firebaseData,"/Sensor",json);
  message = "";
  delay(50);
 

}
