#include <Arduino.h>

// OTA Hub via GitHub
#define OTAGH_OWNER_NAME "Hard-Stuff"
#define OTAGH_REPO_NAME "test-ota"
#define OTAGH_BEARER "YOUR PRIVATE REPO TOKEN" // Follow the docs if using a private repo.
#include <OTA-Hub-diy.hpp>

#define SIM7600_APN "Three" // Your SIM's APN
#include <HardStuff-piolib-SIM7600.hpp>
SIM7600::ClientSecure secure_client(0);

void setup()
{
    // Initialise our board
    Serial.begin(115200);
    Serial.println("Started...");

    // Initialise OTA
    secure_client.setCACert(OTAGH_CA_CERT); // Set the api.github.cm SSL cert on the SIM7600 Client
    SIM7600::init();
    OTA::init(secure_client);

    // Check OTA for updates
    OTA::UpdateObject details = OTA::isUpdateAvailable();
    details.print();                             // Super useful for debugging!
    if (OTA::NEW_DIFFERENT == details.condition) // Only update if the update is both new and a different version name
    {
        // Perform OTA update
        OTA::InstallCondition result = OTA::performUpdate(&details);
        // GitHub hosts files on the objects.githubusercontent.com server, so we have to follow the redirect, unfortunately.
        if (result == OTA::REDIRECT_REQUIRED)
        {
            secure_client.setCACert(OTAGH_REDIRECT_CA_CERT); // Set the objects.githubusercontent.com SSL cert
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