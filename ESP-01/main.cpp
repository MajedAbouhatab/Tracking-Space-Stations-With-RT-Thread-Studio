#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

WiFiClient client;
WiFiClientSecure clientSecure;
HTTPClient http;

void setup()
{
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    WiFiManager wm;
    // reset settings - wipe stored credentials for testing    // these are stored by the esp library
    // wm.resetSettings();
    wm.autoConnect("ESP8266");
    clientSecure.setInsecure();
}

void loop()
{
    if (Serial.available())
    {
        String URL = Serial.readString();
        http.begin(URL[4] == 's' ? clientSecure : client, URL);
        http.GET();
        Serial.println(http.getString().c_str());
        http.end();
    }
    yield();
}
