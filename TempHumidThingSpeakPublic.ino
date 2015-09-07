#include<stdlib.h>
#include "DHT.h"

#define SSID "<your_network_name>"//your network name
#define PASS "<your_network_password>"//your network password
#define IP "184.106.153.149" // thingspeak.com
#define DHTPIN 7     // what pin the DHT sensor is connected to
#define DHTTYPE DHT11   // DHT 11
#define Baud_Rate 115200 //original 9600
#define GREEN_LED 3
#define RED_LED 4
#define DELAY_TIME 60000

String GET = "GET /update?key=<your_thingspeak_channel_key>&field1=";
String FIELD2 = "&field2=";
String FIELD3 = "&field3=";
bool updated;

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(Baud_Rate);
  Serial.println("AT");
  
  delay(5000);
  
  if(Serial.find("OK")){
    //connect to your wifi netowork
    bool connected = connectWiFi();
    if(!connected){
      //failure, need to check your values and try again
      Error();
    }
  }else{
    Error();
  }
  
  //initalize DHT sensor
  dht.begin();
}

void loop(){
  float h = dht.readHumidity();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(f)) {
    LightRed();
    return;
  }
  
  //update ThingSpeak channel with new values
  updated = updateTemp(String(f), String(h));
  if(updated){
    LightGreen();
  }else{
    LightRed();
  }
  
  //wait for delay time before attempting to post again
  delay(DELAY_TIME);
}

bool updateTemp(String tenmpF, String humid){
  //initialize your AT command string
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  
  //add IP address and port
  cmd += IP;
  cmd += "\",80";
  
  //connect
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("Error")){
    return false;
  }
  
  //build 'Post' command, ThingSpeak takes Post or Get commands for updates, I use a Get
  cmd = GET;
  cmd += tenmpF;
  cmd += FIELD2;
  cmd += humid;
  cmd += "\r\n";
  
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.find(">")){
    //send through command to update values
    Serial.print(cmd);
  }else{
    Serial.println("AT+CIPCLOSE");
  }
  
  if(Serial.find("OK")){
    //success! Your most recent values should be online.
    return true;
  }else{
    return false;
  }
}
 
boolean connectWiFi(){
  Serial.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  delay(5000);
  if(Serial.find("OK")){
    return true;
  }else{
    return false;
  }
}

void LightGreen(){
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);  
}

void LightRed(){
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
}

void Error(){      
  while(true){      
    LightRed();      
    delay(2000);      
    LightGreen();
    delay(2000);
  }
}
