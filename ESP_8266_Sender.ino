#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#define GPIO_pin D1

bool wait_flag = 1;
bool try_again = 0;
class converter{
private:
char *ptr;
public:
int converted_nums[6];
void mac_string_to_int(char *passing){
ptr  = strtok(passing,":");
converted_nums[0] = (int)strtol(ptr, NULL, 16);
for(int i=1;i<6;i++){
 if(ptr != NULL){
                            
                ptr = strtok(NULL,":");                                 
                converted_nums[i] = (int)strtol(ptr, NULL, 16);
                }

}
}
};
 
int current=0;
int previous=0;
String inputMessage;
uint8_t macc_add[6];
//uint8_t broadcastAddress[] = {0x80, 0x7D, 0x3A, 0x6B, 0x4A, 0xA7};

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

struct_message myData;



void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.println("Last Packet Send Status: ");
  
 
  
  //sprintf(h,"%d",mac_addr);
  //Serial.println();
 
  if (sendStatus == 0){
    Serial.println("Delivery success");
    try_again = 0;
  }
  else{
    try_again = 1;
    Serial.println("Delivery fail");
  }
}
AsyncWebServer server(80);
const char* ssid     = "ESP8266-Access-Point";
const char* password = "123456789";

const char* PARAM_INPUT_1 = "input1";


// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    
    Enter your MAC Address: <input type="text" name="input1">
    <input type="submit" value="Submit">

</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  // put your setup code here, to run once:
// Init Serial Monitor
pinMode(GPIO_pin,INPUT_PULLUP);
  
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_AP);
  //WiFi.mode(WIFI_STA);
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      wait_flag = 0;
    }
    
      else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
    server.begin();
    while(wait_flag){delay(2);}
    server.end();
    delay(100);
    converter a;
  char x[20];
  inputMessage.toCharArray(x,20);
  char* ptr=x;
  a.mac_string_to_int(ptr);
  for(int i=0;i<6;i++){
  macc_add[i]=a.converted_nums[i];
  }
WiFi.mode(WIFI_STA);
WiFi.begin();
delay(100);
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  // Register peer
  esp_now_add_peer(macc_add, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  
  esp_now_register_send_cb(OnDataSent);
    strcpy(myData.a, "THIS IS A CHAR");
    myData.b = current;
    myData.c = 1.2;
    myData.d = "Hello";
    myData.e = false;
   esp_now_send(macc_add, (uint8_t *) &myData, sizeof(myData));

  
}

void loop() {
  // put your main code here, to run repeatedly:
    current = digitalRead(GPIO_pin);
    //if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    
  
    // Send message via ESP-NOW
    if(previous != current || try_again)
    {
    strcpy(myData.a, "THIS IS A CHAR");
    myData.b = current;
    myData.c = 1.2;
    myData.d = "Hello";
    myData.e = false;
    Serial.println("try again:" + String(try_again));
    if(try_again){
      Serial.println("if entered:"+String(try_again));
      long old_time = millis();
    while(try_again){ 
    esp_now_send(macc_add, (uint8_t *) &myData, sizeof(myData));
    delay(10);
    }
    long new_time = millis();
    long tim_diff = new_time - old_time;
    Serial.println("success try again in:"+String(tim_diff)+" ms");
    }
    else{
      esp_now_send(macc_add, (uint8_t *) &myData, sizeof(myData));
      }
    previous = current;
   }
    //lastTime = millis();
  //}
delay(30);
//Serial.println("try again:" + String(try_again));
}
