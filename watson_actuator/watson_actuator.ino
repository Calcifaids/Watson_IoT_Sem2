#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Ethernet2.h>

// Needs changing for any different boards code is uploaded to.
byte mac[] = {0x90, 0xA2, 0xDA, 0x11, 0x3C, 0x9A};

// Set fallback network config in case of no DHCP server
IPAddress ip(192, 168, 5, 12);
IPAddress gateway(192, 168, 5, 1);
IPAddress subnet(255, 255, 255, 0);

// Create class instances for MQTT
EthernetClient ethernetClient;
PubSubClient client(ethernetClient);

const uint8_t heaterLed = 9;
const uint8_t acLed = 8;
const uint8_t lightLed = 7;
const uint8_t humidityLed = 6;
const uint8_t moistureLed = 5;


void setup() {
  // Init Serial
  Serial.begin(9600);
  Serial.println(F("Starting MQTT client on arduino ..."));

  // Set MQTT server
  client.setServer("192.168.1.112", 1883);
  client.setCallback(call_Back);

  /*!SET PINS HERE!*/ 
  // Set input and outputs
  pinMode(heaterLed, OUTPUT);
  pinMode(acLed, OUTPUT);
  pinMode(lightLed, OUTPUT);
  pinMode(humidityLed, OUTPUT);
  pinMode(moistureLed, OUTPUT);

  // Connect to network
  if (Ethernet.begin(mac) == 0){
  // Fallback if no DHCP
  Serial.println(F("Failed to configure ethernet using DHCP"));
  Ethernet.begin(mac, ip);
  }
  
  // Delay to ensure everything set
  delay(1500);

  Serial.print(F("MQTT client is at: "));
  Serial.println(Ethernet.localIP());
  
}

void loop() {
  // put your main code here, to run repeatedly:
  // Connect on first instance and then reconnect if any DC occurs
  if (!client.connected()){
    reconnect();
  }
  
  client.loop();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println(F("Attempting MQTT Connection..."));

    // Connect actuator to broker
    if (client.connect("WatsonActuator", "actuator", "passw0rd")) {
      Serial.println(F("... connected"));

      
      // Pre-Publish startup values done before subs to prevent improper activation of systems
      
      client.publish("/datafeed/message-log", "Actuator begun");


      // Subscribe to topics
      client.subscribe("/actuatorfeed/lighton");
      client.subscribe("/actuatorfeed/lightoff");
      client.subscribe("/actuatorfeed/heatingon");
      client.subscribe("/actuatorfeed/heatingoff");
      client.subscribe("/actuatorfeed/acon");
      client.subscribe("/actuatorfeed/acoff");
      client.subscribe("/actuatorfeed/humidityon");
      client.subscribe("/actuatorfeed/humidityoff");
      client.subscribe("/actuatorfeed/moistureon");
      client.subscribe("/actuatorfeed/moistureoff");
      

      delay(2000);
      
    }
    // Attempt Reconnect
    else {
      Serial.print(F("Failed, RC = "));
      Serial.print(client.state());
      Serial.println(F(" trying again in ~5 seconds"));

      /*!HARD DELAY USED HERE AS NO NEED PUB OCCURS ON THIS BOARD!*/
      delay(5000);
    }

  }
}

// Function to handle recieving messages from broker
void call_Back(char* topic, byte* payload, unsigned int messLength){
  // Format received data and topic and print to serial
  String t = String(topic);
  char data[messLength+1];
  for (int i = 0; i < messLength; i++){
    data[i] = payload[i]; 
  }
  data[messLength] = '\0';
  Serial.print(F("message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  Serial.println(data);

  //Sub sorting done to speed up
   
  if(t.indexOf("light") > 0){
    if(t.indexOf("lighton") > 0){
      digitalWrite(lightLed, HIGH);
    }
    else if(t.indexOf("lightoff") > 0){
      digitalWrite(lightLed, LOW);
    }
    else{
      Serial.println(F("Malformed light topic received"));
    }
  }
  else if(t.indexOf("heating") > 0){
    if(t.indexOf("heatingon") > 0){
      digitalWrite(heaterLed, HIGH);
    }
    else if(t.indexOf("heatingoff") > 0){
      digitalWrite(heaterLed, LOW);
    }
    else{
      Serial.println(F("Malformed heating topic received"));
    }
  }
  else if(t.indexOf("aco") > 0){
    if(t.indexOf("acon") > 0){
      digitalWrite(acLed, HIGH);
    }
    else if(t.indexOf("acoff") > 0){
      digitalWrite(acLed, LOW);
    }
    else{
      Serial.println(F("Malformed AC topic received"));
    }
  }
  else if(t.indexOf("humidity") > 0){
    if(t.indexOf("humidityon") > 0){
      digitalWrite(humidityLed, HIGH);
    }
    else if(t.indexOf("humidityoff") > 0){
      digitalWrite(humidityLed, LOW);
    }
    else{
      Serial.println(F("Malformed humidity topic received"));
    }
  }
  else if(t.indexOf("moisture") > 0){
    if(t.indexOf("moistureon") > 0){
      digitalWrite(moistureLed, HIGH);
    }
    else if(t.indexOf("moistureoff") > 0){
      digitalWrite(moistureLed, LOW);
    }
    else{
      Serial.println(F("Malformed moisture topic received"));
    }
  }
  
}
