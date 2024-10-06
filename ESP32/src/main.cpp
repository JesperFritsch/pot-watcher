#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// put function declarations here:
int myFunction(int, int);
int valueRead = 4;
const char *ssid = "Tele2_2e991d"; // Enter your WiFi name
const char *password = "rwyhhtmy";  // Enter WiFi password

const char *mqtt_broker = "broker.emqx.io";
const char *topic = "emqx/esp32";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

// Define client instance
PubSubClient client;

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  pinMode (valueRead, INPUT);
  Serial.begin(115200);

  // Connecting to a Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");
  // client.setServer(mqtt_broker, mqtt_port);
  // client.setCallback(callback);
  // while (!client.connected()) {
  //   String client_id = "esp32-client-";
  //   client_id += String(WiFi.macAddress());
  //   Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
  //   if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
  //     Serial.println("Public EMQX MQTT broker connected");
  //   } else {
  //     Serial.print("failed with state ");
  //     Serial.print(client.state());
  //     delay(2000);
  //   }
  // }

}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
  int mois = analogRead(valueRead);
  Serial.println(mois);
  delay(200);
    // put your main code here, to run repeatedly:
  // digitalWrite(valueRead, HIGH);  // turn the LED on (HIGH is the voltage level)
  // delay(1000);                      // wait for a second
  // digitalWrite(valueRead, LOW);   // turn the LED off by making the voltage LOW
  // delay(1000); 
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}