#include <Arduino.h>

// OTA Hub via GitHub
#define OTAGH_OWNER_NAME "Hard-Stuff"
#define OTAGH_REPO_NAME "test-ota"
#include <OTA-Hub-diy.hpp>

// Networking
#include <configs/wifi.h>
#include <WiFiClientSecure.h>
WiFiClientSecure wifi_client;

void setup()
{
    // Initialise our board
    Serial.begin(115200);
    Serial.println("Started...");

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("WiFi failure");
        ESP.restart();
    }

    // Initialise OTA
    wifi_client.setCACert(OTAGH_CA_CERT); // Set the api.github.cm SSL cert on the WiFi Client
    OTA::init(wifi_client);

    // Check OTA for updates
    OTA::UpdateObject details = OTA::isUpdateAvailable();
    details.print(); // Super useful for debugging!
    if (OTA::NO_UPDATE != details.condition)
    {
        // Perform OTA update
        OTA::InstallCondition result = OTA::performUpdate(&details);
        // GitHub hosts files on different server, so we have to follow the redirect, unfortunately.
        if (result == OTA::REDIRECT_REQUIRED)
        {
            wifi_client.setCACert(OTAGH_REDIRECT_CA_CERT); // Set the objects.githubusercontent.com SSL cert
            OTA::continueRedirect(&details);
        }
    }
    else
    {
        Serial.println("No new update available. Continuing...");
    }
    Serial.print("Loop");
}

void loop()
{
    delay(5000);
    Serial.print("edy loop");
}