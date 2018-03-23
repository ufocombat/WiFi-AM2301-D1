#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTPIN D7
#define LED_SCL D5
#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);

const String  vers = "2.42";
const char* host = "ufoiot.azurewebsites.net";
const int httpPort = 80;

/*const char* ssid[]     = {"Enceladus","Astra"};
const char* password[] = {"11111112","BrahmAstra"};*/

const char* ssid[]     = {"rnds"};
const char* password[] = {"20011983"};

int dell = 10000;
String _ssid="";

void WiFiConnect(){
  if (WiFi.status() != WL_CONNECTED)
  {
    for (int i=0;i<2;i++)
    {
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(ssid[i]);
      
      WiFi.begin(ssid[i], password[i]);
      _ssid = ssid[i];
    
      int tr = 0;
  
        
      while (WiFi.status() != WL_CONNECTED) 
      {
        delay(500);
        Serial.print(".");
        if ((i==0)&(tr++>20)) break;
      }
  
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("");
        Serial.println("WiFi connected");  
          
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());  
        break;
      }
      else
      {
        Serial.println("");
        Serial.println("failed connection!");  
      }    
    }
  }
}

void setup() {
  pinMode(LED_SCL, OUTPUT);   
  
  Serial.begin(9600);
  delay(100);
  
  Serial.println("DHT21 "+vers);  
  dht.begin();
  
}

int value = 0;

void loop() {
  delay(dell);
  ++value;
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");

///

  Serial.print("connecting to ");
  Serial.println(host);

  WiFiConnect();
  WiFiClient client;
  
  if (!client.connect(host, httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }

  digitalWrite(LED_SCL, HIGH);

// Settings request
//  String url = "http://ufoiot.azurewebsites.net/settings";
   String url = "https://ufoiot2.azurewebsites.net/api/settings";  
/*  url += streamId;
  url += "?private_key=";
  url += privateKey;
  url += "&value=";
  url += value;*/
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  client.println(String("GET ")+url+" HTTP/1.1");
  client.println("Host: ufoiot2.azurewebsites.net");
  client.println("Connection: close\r\n\r\n");
  
  unsigned long timeout = millis();
  
  while (client.available() == 0) {
    if (millis() - timeout > 10000) {
      Serial.println("<Client Timeout !>");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
//    Serial.print(line);

    int w1 = line.indexOf("dellay=");
    
    if (w1>0)
    {
      int w2 = line.indexOf("'}",w1+8);
      String w3 = line.substring(w1+8,w2);
      dell = w3.toInt();
      Serial.print("Setting dellay to ");
      Serial.println(dell);
    }
  }

  if (!client.connect(host, httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }
  
  url = "http://ufoiot.azurewebsites.net/weather";
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  String ss = "{";
  ss+="\"ID\":\"GEO-02\",";
  ss+="\"Temperature\":\""+String(t)+"\",";
  ss+="\"Humidity\":\""+String(h)+"\",";
  ss+="\"Term\":\"0\",";
  ss+="\"ssid\":\""+_ssid+"\",\"Version\":\""+vers+"\"";
  ss+="}";

  Serial.print("POST: ");
  Serial.println(ss);

  client.println("POST /weather HTTP/1.1");
  client.println("Host: ufoiot.azurewebsites.net");
  client.println("Cache-Control: no-cache");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(ss.length());
  client.println();
  client.println(ss);
               
  while (client.available() == 0) 
  {
    if (millis() - timeout > 10000) {
      Serial.println("Client Timeout !");
      client.stop();
      digitalWrite(LED_SCL, LOW);
      return;
    }
  }

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");    

  digitalWrite(LED_SCL, LOW);  
}

