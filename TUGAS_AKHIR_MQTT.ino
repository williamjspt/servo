#include <SPI.h>
#include <MFRC522.h>
#include <Servo_ESP32.h>
#include <PubSubClient.h>
#include <WiFi.h>
const char* ssid = "bangsad";
const char* password = "william12345";
const char* mqtt_server = "broker.mqttdashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
//char msg[MSG_BUFFER_SIZE];
#define SS_PIN    21
#define RST_PIN   22
#define ledPin  33  
#define ldrPin  34



#define SERVO_PIN 4
Servo_ESP32 myservo;
const char* msgRelayHidup = "Relay Mati";
const char* msgRelayMati = "Relay Hidup";
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  
    if ((char)payload[0] == '1') {
    digitalWrite(ledPin, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println(msgRelayHidup);
    client.publish("Relay", msgRelayHidup);
  } if((char)payload[0] == '2') {
    digitalWrite(ledPin, LOW);  // Turn the LED off
    Serial.println(msgRelayMati);
    client.publish("Relay", msgRelayMati);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    WiFi.mode(WIFI_STA);
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("topic", "hello world");
      // ... and resubscribe
      client.subscribe("tas");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
//   pinMode(RELAY, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  SPI.begin();          // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(ledPin, OUTPUT);  //initialize the LED pin as an output
  pinMode(ldrPin, INPUT);   //initialize the LDR pin as an input
  pinMode(ledPin,HIGH);
  myservo.attach(SERVO_PIN);
  myservo.write( 70 );
  delay(7500);
  myservo.write( 0 );
 
  Serial.println();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
   Serial.println("Put your card to the reader...");
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  int ldrStatus = analogRead(ldrPin);
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "EC E0 DE 2B") //change here the UID of the card
  {
    
    Serial.println("Authorized access");
    Serial.println();
   
   myservo.write( 70 );
 delay(7500);
  myservo.write( 0 );

  }else {
  delay(DENIED_DELAY);
  }

//  unsigned long now = millis();
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
//    //Serial.print("Publish message: ");
//    //Serial.println(msg);
//    client.publish("outTopic", msg);
//  }
}
