// This code is written for controlling D0, D1, D2 pin of NodeMCU board, the callback function is written for recieving message format
// D0-1 -> for On D0 pin and D0-0 -> for Off D0 pin

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "Net_2.4G";           // Enter your WiFi name
const char *password = "Nopassword@163"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org"; // test.mosquitto.org
const char *topic = "xx342/control";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

uint8_t mqtt_led = D4;

void setup()
{
    pinMode(D0, OUTPUT);
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    

    digitalWrite(mqtt_led, HIGH);

    // Set software serial baud to 115200;
    Serial.begin(115200);

    // connecting to a WiFi network
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        digitalWrite(D0,HIGH);
        delay(300);
        digitalWrite(D0,LOW);
        delay(300);
    }

    Serial.println("Connected to the WiFi network");

    // connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);

    while (!client.connected())
    {
        String client_id = "esp8266-client-";
        client_id += String(ESP.getChipId());

        Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
        

        if (client.connect(client_id.c_str()))
        {
            Serial.println("Public mosquitto mqtt broker connected");
        }
        else
        {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(1400);
        }
    }

    // publish and subscribe
    client.publish(topic, "i_am_online");
    client.subscribe(topic);
}


void callback(char *topic, byte *payload, unsigned int length)
{   
    // Based on the message recieve as "D0-1"
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    String pin = "";
    for (int i = 1; i < 3; i++)
    {
        pin+=(char)payload[i];
    }
    String state = String((char)payload[4]);
    Serial.println(pin);
    Serial.print("state = ");
    Serial.println(state);
    pin_call(pin, state);



    Serial.println();
    Serial.println(" - - - - - - - - - - - -");
}

//function for controlling the pin
void pin_call(String pin, String state)
{   
    Serial.print(pin);
    Serial.print("-");
    Serial.println(state);
    int int_state = state.toInt();
    
    if(pin=="D0"){
        digitalWrite(D0,int_state);

    }
    else if(pin=="D1"){
        digitalWrite(D1,int_state);
    }
    else if(pin=="D2"){
        digitalWrite(D2,int_state);
    }
}


//function for reconnect to mqtt server
void mqtt_reconnect(){
    while (!client.connected())
    {
        String client_id = "esp8266-client-";
        client_id += String(WiFi.macAddress());

        //Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
        //digitalWrite(D0,HIGH);
        //delay(300);
        //digitalWrite(D0,LOW);
        //delay(300);

        if (client.connect(client_id.c_str()))
        {
            Serial.println("Reconnected to MQTT broker");
        }
        else
        {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(1400);
        }
    }

    // publish and subscribe
    client.publish(topic, "i_am_online");
    client.subscribe(topic);
}


void wifi_reconnect(){
    WiFi.begin(ssid, password);
    Serial.println("Reconnecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(D0,HIGH);
        delay(300);
        digitalWrite(D0,LOW);
        delay(300);
    }

    Serial.println("Connected to the WiFi network");

}

void loop()
{   
    if (WiFi.status() != WL_CONNECTED){
        ESP.restart();
    }

    if (!client.connected()){
        //analogWrite(mqtt_led,0); //it is set to HIGH but is 
        Serial.println("Mqtt disconnected..");
        //ESP.restart();
        mqtt_reconnect();
    }

    analogWrite(mqtt_led,100);
    client.loop();
    
} 
