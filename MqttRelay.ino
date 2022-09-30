/*
 MQTT Relay Control

 1) Connect to MQTT broker
 2) Subscribe to single topic
 3) Control relays as messages appear

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

// Define GPIO pins used for relays
#define RELAY1 8
#define RELAY2 9
#define RELAY3 10
#define RELAY4 11

#define MQTT_TOPIC "finchway/relays"


#include <YunClient.h>
#include <PubSubClient.h>

// MQTT broker address
IPAddress broker(192, 168, 0, 120);

void callback(char* topic, byte* payload, unsigned int length)
 {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    // Serial.print((char)payload[i]);
    Serial.print(payload[i]);
    Serial.print(" ");
  }
  Serial.println();

  // payload is two bytes
  // byte 0 is channel (1 - 4), byte 1 is state (1 = on, 0 = off)
  if (length == 2)
  {
    switch(payload[0])
    {
      case 1: digitalWrite(RELAY1, payload[1]);
              break;
      case 2: digitalWrite(RELAY2, payload[1]);
              break;
      case 3: digitalWrite(RELAY3, payload[1]);
              break;
      case 4: digitalWrite(RELAY4, payload[1]);
              break;
      default: break;
    }
  }
  
}

YunClient yunClient;
PubSubClient mqtt(yunClient);

void reconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect("arduinoClient"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic","hello world");
      // ... and resubscribe

      mqtt.subscribe(MQTT_TOPIC);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{

  // set pins as outputs
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  // start serial port for debugging
  Serial.begin(57600);

  // setup mqtt client parameters
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(callback);

  // start bridge for wifi
  Bridge.begin();

  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();
}
