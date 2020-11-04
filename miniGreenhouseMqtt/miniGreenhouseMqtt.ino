
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "settings.h"

//DHT11
#include "DHT.h"
#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
//PhotoResister
#define PhotoResisterPin 2
//HCSR04
#define echoPin 14
#define trigPin 12

StaticJsonDocument<512> doc;
StaticJsonDocument<512> doc2;


DHT dht(DHTPIN, DHTTYPE);
float plantHeight, sensorHeight, temperature, humidity, heatIndex, rawLight;
String lightConditions = "none";
char device_name[] = "PlantBoy1";


#define MSG_BUFFER_SIZE  (512)
char msg[MSG_BUFFER_SIZE];

//Setup Eclipse-Hono
//Hono hono(MQTT_BROKER, MQTT_PORT, MQTT_SERVER_FINGERPRINT);
WiFiClient espClient;
PubSubClient client(espClient);



//SETUP WIFI
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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
//END OF WIFI SETUP

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (MQTT_USECREDENTIALS){
      if (client.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("cedalo/scaleControl");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    }else{
          if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("cedalo/scaleControl");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
      
    }

  }
}

void mqttSetup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  setup_wifi();
  client.setBufferSize(512);
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
}


float ultrasonicRead(){
 digitalWrite(trigPin, LOW); 
 delayMicroseconds(2); 
 digitalWrite(trigPin, HIGH); 
 delayMicroseconds(10); 
 digitalWrite(trigPin, LOW);
 float duration = pulseIn(echoPin, HIGH); 
 float distance = (duration*0.0343)/2; 
 return distance;
}

void setup() {
  Serial.begin(115200);

  mqttSetup();

  //Ultrasonic sensor
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  sensorHeight = ultrasonicRead();

  //DHT
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Celsius (isFahreheit = false)
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);


//PhotoResister
    rawLight = analogRead(A0);

  // We'll have a few threshholds, qualitatively determined
  if (rawLight < 10) {
    lightConditions = "dark";
  } else if (rawLight < 200) {
    lightConditions = "dim";
  } else if (rawLight < 500) {
    lightConditions = "light";
  } else if (rawLight < 800) {
    lightConditions = "bright";
  } else {
    lightConditions = "v_bright";
  }

  //ultrasonic sensor
  float tempHeight = sensorHeight - ultrasonicRead();
  if (tempHeight >= 0){
    plantHeight = tempHeight;
  }

    doc["id"] = device_name;
    doc["height"] = plantHeight;
    doc["temp"] = temperature;
    doc["humid"] = humidity;
    doc["light"] = lightConditions;
    doc["rawlight"] = rawLight;
    
    doc2["metrics"]=doc;
    
    char tempString[512]; 

    serializeJson(doc2, tempString);

    snprintf (msg, MSG_BUFFER_SIZE, tempString);
    Serial.println("Publish message: ");
    Serial.println(msg);
    Serial.println();

    //hono.publish(msg);
    client.publish(MQTT_PUBTOPIC, msg);
    

    delay(100);
    ESP.deepSleep(20e6); // 20e6 is 20 microseconds
  
}
