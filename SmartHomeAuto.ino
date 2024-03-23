#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#define ldr_pin A0 //pin where the ldr needs to be attached
#define rel_pin D3 //pin where the led pin is connected
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME  "YOUR_USERNAME_HERE"
#define AIO_KEY  "YOUR_ADAFRUIT_KEY_HERE"
WiFiClient client;
int ctr=-1;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

boolean MQTT_connect();
boolean MQTT_connect() //function to connect to the MQTT
{  
  int8_t ret;
  if (mqtt.connected()) 
  {    return true; 
  }  
  uint8_t retries = 3;  
  while ((ret = mqtt.connect()) != 0) 
  { 
    mqtt.disconnect(); 
    delay(2000);  
    retries--;
    if (retries == 0) 
    { 
      return false; 
    }
  } 
  return true;
}
Adafruit_MQTT_Publish light_Intensity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Name_of_feed"); 
Adafruit_MQTT_Subscribe light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Name_of_feed");
void setup()
{
  Serial.begin(115200);
  pinMode(ldr_pin,INPUT); //ldr is connected as Input devive
  pinMode(rel_pin,OUTPUT); //led gives the Output
  WiFi.disconnect();
  delay(3000);
  Serial.println("START");
  WiFi.begin("YOUR_SSID","YOUR_PASSWORD");
  while ((!(WiFi.status() == WL_CONNECTED)))
  {
    delay(300);
    Serial.print("..");
  }
  Serial.println("Connected");
  Serial.println("Your IP is");
  Serial.println((WiFi.localIP()));
}
void send_val()//function to send values to the server(Here Adafruit acts as a Server)
{
  if (MQTT_connect()) 
    {
      int intensity=analogRead(ldr_pin);
      Serial.print("Intensity : ");Serial.println(intensity);
      int value=0;
      if(intensity>500)//setting up a Threshold to compare the ldr value to
      {
        value=1;
      }
      else
      {
        value=0;
      }
      if (ctr!=value)//comparing so that the same value isn't sent to the server time and after
      {
        if(light_Intensity.publish(value))
          Serial.println("data sent");
        ctr=value;
      } 
      else 
      {
        Serial.println("data not sent");
      }

    } 
    else 
    {
      Serial.println("mqtt not connected");
    }
}
void recv_val()//function to receive value from the server
{
  if(MQTT_connect())
  {
    mqtt.subscribe(&light);
    Adafruit_MQTT_Subscribe *subscription;
    while (subscription = mqtt.readSubscription(2500)) {
      if(subscription==&light){
        Serial.println((char*)light.lastread);
        int value = atoi((char *)light.lastread);
        digitalWrite(rel_pin, value);//switching the Light(led) on and off according to the value received
      }
    }
  }
}
void loop()//loop function to perform the task of sending and receiving data to and from the server
{
    send_val();
    recv_val();
    delay(2500);
}
