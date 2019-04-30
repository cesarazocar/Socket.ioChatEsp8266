/*
    Cesar Azócar 04/30/2019

*/
#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
#include <ArduinoJson.h>

SocketIOClient client;
const char* ssid     = "SSID"; //Wifi SSID
const char* password = "******"; //Wifi Pass

char host[] = "192.168.x.x"; //nodejs host
int port = 3000; //nodejs port 3000

/*extern String RID;
  extern String Rname;
  extern String Rcontent;*/

StaticJsonDocument<200> doc;
String JSON;

unsigned long previousMillis = 0;
long interval = 5000;
unsigned long lastreply = 0;
unsigned long lastsend = 0;

void setup() {

  doc["message"] = "Hola";
  doc["username"] = "nodemcu";

  serializeJson(doc, JSON);
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  if (client.connected())
  {
    client.send("chat:newuser", "username", "nodemcu");
  }
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {
    previousMillis = currentMillis;
    //client.heartbeat(0);
    String salida = "mensaje numero: ";
    salida.concat(String(currentMillis));
    /*doc["message"] = salida;
      doc["username"] = "nodemcu";
      serializeJson(doc, JSON);
    */
    client.sendJSON("chat:message", JSON);
    lastsend = millis();
  }


  
    String json = client.on();
    
    if (json.length() > 0) {
    Serial.print("json: ");
    Serial.print(json);
    Serial.println(); Serial.println(); Serial.println();

    const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(2) + 50;
    DynamicJsonDocument doc2(capacity);
    deserializeJson(doc2, json);

    //const char* event = doc2[0]; // "chat:message"
    String event = doc2[0]; // "chat:message"

    Serial.print("event: ");
    Serial.println(event);

    if (event == "chat:message") {

      String message = doc2[1]["message"]; // "Holas"
      String username = doc2[1]["username"]; // "nodemcu"

      Serial.print(" user ");
      Serial.print(username);
      Serial.print(" says: ");
      Serial.println(message);
    }
    if (event == "chat:typing") {
      boolean typing = doc2[1]["typing"]; // true || false
      if (typing) {
        String username = doc2[1]["username"];
        Serial.print(" user ");
        Serial.print(username);
        Serial.print(" is typing...");
      } else {
        typing = false;
      }
    }
    if (event == "chat:closing") {

      String username = doc2[1]["username"];
      Serial.print(" user: ");
      Serial.print(username);
      Serial.print(" has left the room");
    }
  }
}
