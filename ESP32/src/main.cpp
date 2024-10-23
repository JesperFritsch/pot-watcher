#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <vector>

// put function declarations here:
// int myFunction(int, int);
// int valueRead = 4;
const char *ssid = "Tele2_2e991d"; // Enter your WiFi name
const char *password = "rwyhhtmy";  // Enter WiFi password

const char *mqtt_broker = "192.168.0.39";
const char *topic_out = "eps/test/out";
const char *topic_in = "eps/test/in";
const int mqtt_port = 1883;



class MoistureSensor{
  public:
  
    MoistureSensor(unsigned int read_pin_in, unsigned int scaling) : 
      scaling(scaling),
      read_pin(read_pin_in){
        pinMode(this->read_pin, INPUT);
      }
      
    float read(){
      return 1-((float)(analogRead(this->read_pin)) / this->scaling);
    }
      
    unsigned int get_pin(){
      // if we need to know what pin this instance is using 
      return this->read_pin;
    }
    
  private:
    unsigned int read_pin;
    unsigned int scaling;
};


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsgTime = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
char *message_in = "no message";


// MoistureSensor moist = MoistureSensor(4, 0xFFF);

std::vector<MoistureSensor> sensors;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
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
  
  
  if(strcmp("config", topic) == 0){
    sensors.clear();
    sensors.push_back(MoistureSensor(payload[0], 0xFFF));
    Serial.println(payload[0]);
  }
  else{
    Serial.println("no config");
  }
  
  value = 0;
  char message_in[length + 1];
  memcpy(message_in, payload, length);
  message_in[length] = '\0';
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
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic_out, "hello world");
      // ... and resubscribe
      client.subscribe(topic_in);
      client.subscribe("config");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsgTime > 1000) {
    lastMsgTime = now;
    ++value;
    
    for(unsigned int i=0;i<sensors.size();i++){
      MoistureSensor moist = sensors[i];
      float moist_val = moist.read();
      char txt_buffer[60];
      snprintf(txt_buffer, 60, "moist_value [%d]: %.2f", i + 1, moist_val);
      Serial.println(txt_buffer);
    }
    
    
    // snprintf (msg, MSG_BUFFER_SIZE, "%s #%ld", message_in, value);
    // Serial.print("Publish message: ");
    // Serial.println(msg);
    // Serial.println(message_in);
    // client.publish(topic_out, msg);
  }
}

