import paho.mqtt.client as mqtt
import argparse
import sys



# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("pot_watch/#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(f"topic: {msg.topic} payload: {str(msg.payload)}")


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--sender", action="store_true", default=False)
    args = ap.parse_args(sys.argv[1:])
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message
    mqttc.connect("192.168.0.39", 1883, 60)
    
    if args.sender:
        mqttc.loop_start()
        while True:
            val = input("Send: ")
            if val == "q":
                break
            mqttc.publish("pot_watch/text", val)
        mqttc.loop_stop()


    else:
        mqttc.loop_forever()
