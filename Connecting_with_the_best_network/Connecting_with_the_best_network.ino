#include "EEPROM.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
String ssid1, ssid2, ssid3, password1, password2, password3;
const char* ssid = "Redmi 8292";
const char* password = "11111111";

WebServer server(80);

const int led = 2;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp32!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

String formHtml = "<!DOCTYPE html>\
<html>\
<body style=\"text-align: center;\">\
<form method=\"get\" action=\"http://192.168.137.104/STORE_SSID_PASSWORD\">\
            <select name=\"slot\" id=\"slot\">\
            <option value = \"Slot No 1\">Slot No 1</option>\
            <option value = \"Slot No 2\">Slot No 2</option>\
            <option value = \"Slot No 3\">Slot No 3</option>\
            </select>\
            <br>\
            <p> Enter the ssid here </p>\
            <input type=\"text\" name=\"ssid\" placeholder=\"Enter SSID\">\
            <p> Enter the password here </p>\
            <input type =\"text\" name=\"password\" placeholder=\"Enter password\">\
            <br>\
            <br>\
            <button type=\"submit\"> Submit </button>\
            </form>\
            <br>\
            <br>\
            <a href=\"set_wifi\"> Click Here for Connect with Wifi </a>\
            </body>\
</html>";


void setup(void) {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  EEPROM.begin(512);
  ssid1 = EEPROM.readString(0);
  password1 = EEPROM.readString(50);
  ssid2 = EEPROM.readString(100);
  password2 = EEPROM.readString(150);
  ssid3 = EEPROM.readString(200);
  password3 = EEPROM.readString(250);
  delay(2000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", []() {
    server.send(200, "text/html", formHtml);
  });
  server.on("/set_wifi", set_wifi);
  server.on("/STORE_SSID_PASSWORD", STORE_SSID_PASSWORD);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
}

void STORE_SSID_PASSWORD() {
  String _arg = server.arg(0);
  String s1="Slot No 1";
  String s2="Slot No 2";
  String s3="Slot No 3";
  if (s1 == _arg) {
    ssid1 = server.arg(1);
    password1 = server.arg(2);
    EEPROM.writeString(0, ssid1);
    EEPROM.writeString(50, password1);
    Serial.println(ssid1);
    Serial.println(password1);
    Serial.println();
  }
  else if (s2 == _arg) {
    ssid2 = server.arg(1);
    password2 = server.arg(2);
    EEPROM.writeString(100, ssid2);
    EEPROM.writeString(150, password2);
    Serial.println(ssid2);
    Serial.println(password2);
    Serial.println();
  }
  else if (s3 == _arg) {
    ssid3 = server.arg(1);
    password3 = server.arg(2);
    EEPROM.writeString(200, ssid3);
    EEPROM.writeString(250, password3);
    Serial.println(ssid3);
    Serial.println(password3);
    Serial.println();
  }
  EEPROM.commit();
  server.send(200, "text/html", formHtml);
}

void set_wifi(){
  int max_dbi = 0;
  int max_db = -300;
  Serial.println("Scanning available networks ...");
  int n = WiFi.scanNetworks();   // this function returns the no of available networks
  Serial.print(n); Serial.println("Networks Found");
  if (n != 0) {
    for (int i = 0; i < n; ++i) {
      Serial.print("network"); Serial.print(i + 1); Serial.print(":"); Serial.print(WiFi.SSID(i)); Serial.print(" ("); Serial.print(WiFi.RSSI(i)); Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "****");
      delay(50);
      if(WiFi.SSID(i)==ssid1 or WiFi.SSID(i)==ssid2 or WiFi.SSID(i)==ssid3 ){
        if(WiFi.RSSI(i)>max_db){
          max_dbi=i;
          max_db=WiFi.RSSI(i);
        }
      }
    }
    WiFi.disconnect(true);
    delay(200);
    // connecting with networks
    Serial.println("connecting to given network");
    if (WiFi.SSID(max_dbi) == ssid1) {
      WiFi.begin(ssid1.c_str(), password1.c_str());
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to ssid1");
      }
      Serial.println("Connected to wifi Network ");
       Serial.print("IP address: ");
       Serial.println(WiFi.localIP());
    }
    else if (WiFi.SSID(max_dbi) == ssid2) {
      WiFi.begin(ssid2.c_str(), password2.c_str());
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to ssid2");
      }
      Serial.println("Connected to wifi Network ");
       Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    }
    else if (WiFi.SSID(max_dbi) == ssid3) {
      WiFi.begin(ssid3.c_str(), password3.c_str());
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to ssid3");
      }
      Serial.println("Connected to wifi Network ");
       Serial.print("IP address: ");
       Serial.println(WiFi.localIP());
    }

  }
  else{
    Serial.println("no networks found");
  }
  server.send(200, "text/html", formHtml);
}
