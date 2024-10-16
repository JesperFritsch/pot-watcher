#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// put function declarations here:
// int myFunction(int, int);
// int valueRead = 4;
const char *ssid = "Tele2_2e991d"; // Enter your WiFi name
const char *password = "rwyhhtmy";  // Enter WiFi password

const char *mqtt_broker = "192.168.0.39";
const char *topic_out = "eps/test/out";
const char *topic_in = "eps/test/in";
const int mqtt_port = 1883;

// // Define client instance
// PubSubClient client;

// void setup() {
//   // put your setup code here, to run once:
//   int result = myFunction(2, 3);
//   pinMode (valueRead, INPUT);
//   Serial.begin(115200);

//   // Connecting to a Wi-Fi network
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.println("Connecting to WiFi..");
//   }
//   Serial.println("Connected to WiFi");
//   // client.setServer(mqtt_broker, mqtt_port);
//   // client.setCallback(callback);
//   // while (!client.connected()) {
//   //   String client_id = "esp32-client-";
//   //   client_id += String(WiFi.macAddress());
//   //   Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
//   //   if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
//   //     Serial.println("Public EMQX MQTT broker connected");
//   //   } else {
//   //     Serial.print("failed with state ");
//   //     Serial.print(client.state());
//   //     delay(2000);
//   //   }
//   // }

// }

// void callback(char *topic, byte *payload, unsigned int length) {
//     Serial.print("Message arrived in topic: ");
//     Serial.println(topic);
//     Serial.print("Message:");
//     for (int i = 0; i < length; i++) {
//         Serial.print((char) payload[i]);
//     }
//     Serial.println();
//     Serial.println("-----------------------");
// }

// void loop() {
//   int mois = analogRead(valueRead);
//   Serial.println(mois);
//   delay(200);
//     // put your main code here, to run repeatedly:
//   // digitalWrite(valueRead, HIGH);  // turn the LED on (HIGH is the voltage level)
//   // delay(1000);                      // wait for a second
//   // digitalWrite(valueRead, LOW);   // turn the LED off by making the voltage LOW
//   // delay(1000); 
// }

// // put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }




// const char* ssid = "........";
// const char* password = "........";
// const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsgTime = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
char *message_in = "no message";

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
    snprintf (msg, MSG_BUFFER_SIZE, "%s #%ld", message_in, value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    Serial.println(message_in);
    client.publish(topic_out, msg);
  }
}