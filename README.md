# **OTA Hub (by Hard Stuff)** - OTA directly from GitHub

This is the Do-It-Yourself library for performing Over-the-Air updates for your ESP32 devices directly from GitHub releases. _Read the [the docs](https://github.com/Hard-Stuff/OTA-Hub-diy-example_project) for more a wider description of **OTA Hub**._

-   Once your GitHub CI/CD is set up on your public or private repos they will automatically create `firmware.bin` files for every newly created release.
-   Next, configure your ESP32 devices to automatically check for the latest releases on your GitHub repository, and download and install the updates as needed.

**OTA Hub** is designed to do one thing, and one thing only:

    Deliver Over-the-Air updates onto your ESP32/embedded devices directly from your code releases in an obvious, clean, light-weight way.

**OTA Hub DIY** is for the hobbyists and small teams, directly grabbing release files from GitHub, involving as minimal setup as possible. It's completely open-source, and of course, free!

**OTA Hub Pro** is for the experts and larger teams, providing you with a dashboard to have finer control over your release deployment, fleet management, greater flexibility, and even less setup! Find out more at [ota-hub.com/pro](ota-hub.com/pro).

### Benefits over alternatives

1. No-longer worry about drag-and-dropping `.bin` files into some fiddly UI.
2. Easily trace your code to your releases to your deployed firmware.
3. Not locked into any eco-systems you probably aren't using already (you're probably already using GitHub).
4. Client-agnostic! Implement OTA Hub on-top of secure or insecure\* connections, on 4G, NB-IoT, or WiFi modules.
5. The DIY version is open-source, **completely free**, and GitHub hosting is also (currently) completely free!

_\* Note that our default examples are for SSL-enabled connections, as GitHub requires a secure connection. As this is open-source, you can of course use your own storage buckets APIs for insecure connections etc._

## Usage

_You must first have CI/CD set up on your firmware repo of choice. Follow [the docs](https://github.com/Hard-Stuff/OTA-Hub-diy-example_project) for a simple copy-paste guide on how to do this._

The flow logic for this entire OTA library is super simple:

1. **Check for updates** - It first checks on your GitHub repo for the latest release of your firmware. GitHub reports back the `name` and `published_at` timestamp of the latest **release**.
2. **Perform the update** - If given the information in step 1 compared to the current installation you want to perform the update: automatically download and install the `firmware.bin` file onto the device.
3. **Follow the redirect** - Because GitHub hosts the release data on `api.github.com` but the `firmware.bin` asset on `objects.githubusercontent.com`, we may have to manually follow the redirect **after** updating the SSL CA Cert. _This suck, but it's how GitHub has built it 🤷‍♂️._

### Basic Example

```cpp
// OTA Hub via GitHub
#define OTAGH_OWNER_NAME "Hard-Stuff"
#define OTAGH_REPO_NAME "OTA-Hub-diy-example_project"
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

    // 1. Check for updates, by checking the latest release on GitHub
    // .. (n.b. get more control on OTA Hub Pro!)
    OTA::UpdateObject details = OTA::isUpdateAvailable();

    if (OTA::NO_UPDATE == details.condition)
    {
        Serial.println("No new update available. Continuing...");
    }
    else
    // 2. Perform the update (if there is one)
    {
        OTA::InstallCondition result = OTA::performUpdate(&details);

        if (result == OTA::REDIRECT_REQUIRED)
        {
            // Step 3: Follow GitHub's redirect to get the asset files
            // .. (n.b. this is faster and easier over on OTA Hub Pro!)
            wifi_client.setCACert(OTAGH_REDIRECT_CA_CERT);  // Set the objects.githubusercontent.com SSL cert
            OTA::continueRedirect(&details);                // Follow the redirect and performUpdate.
        }
    }
    // As normal... note: performUpdate will restart the board unless you specify otherwise.
}

void loop()
{
    // As normal...
}
```

### `#define`s to be aware of

```cpp
// Required defines
#define OTAGH_OWNER_NAME "Hard-Stuff" // change to the repo's owner
#define OTAGH_REPO_NAME "OTA-Hub-diy-example_project" // chante to the repo's name

// Optional defines
#define OTAGH_BEARER "YOUR PRIVATE REPO TOKEN" // Needed for private repositories.

// Optional, but rarely-changed, defines
#define OTAGH_SERVER char*  // default: api.github.com
#define OTAGH_PORT number   // default: 443 (HTTPS secured)
#define OTAGH_CHECK_PATH .. // only change if you're not using GitHub
#define OTAGH_BIN_PATH ..   // only change if you're not using GitHub
                            // you will also need to set custom certs if not using GitHub

// Built-in CA Certs
static const char OTAGH_CA_CERT[];          // CA Cert for GitHub's api.github.com server
static const char OTAGH_REDIRECT_CA_CERT[]; // CA Cert for GitHub's objects.githubusercontent.com server
```

### Dependencies

-   arduino-libraries/ArduinoHttpClient
-   paulstoffregen/Time
-   [HardStuff-piolib-Http](https://github.com/Hard-Stuff/HardStuff-piolib-Http)

## Compabibility and testing

This library has been tested on the ESP32S3 with both the internal WiFi functionality and a [SIMCOM SIM7600G](https://github.com/Hard-Stuff/TinyGSM).

We are looking for people to support us in testing more boards, other connectivity functionalities, and making **OTA Hub Pro** even more useful. Contribute either on our GitHub repos, or contact us at [ota-hub@hard-stuff.com](mailto:ota-hub@hard-stuff.com).

## Hard Stuff

Hard Stuff is a hardware prototyping agency and venture studio focussing on sustainability tech, based in London, UK.
Find out more at [hard-stuff.com](hard-stuff.com).

This library is written and provided open-source in the hope that you go on to build great things.
