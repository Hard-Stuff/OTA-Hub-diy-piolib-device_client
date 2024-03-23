// configs
#include <memory>

// configs
#include <configs/ota.h>

// libs
#include <HardStuff_pio_lib_Http.hpp>
#include <Update.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

#ifndef OTAGH_SERVER
#define OTAGH_SERVER "api.github.com"
#ifndef OTAGH_BEARER
#pragma message("You MUST have a Bearer token for your GitHub repo. Follow the documentation for this.") // TODO: fix this.
#endif

#ifndef OTAGH_PORT
#define OTAGH_PORT 443
#endif

#define OTAGH_CHECK_PATH "/repos/" + OTAGH_OWNER_NAME + "/" + OTAGH_REPO_NAME + "/releases?per_page=1" // TODO:: would this work?

#ifndef OTA_VERSION
#define OTA_VERSION "no_version"
#endif

String getMacAddress()
{
    uint8_t baseMac[6];
    // Get MAC address for WiFi station
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    char baseMacChr[18] = {0};
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    return String(baseMacChr);
}

time_t cvtDate()
{
    char s_month[5];
    int year;
    tmElements_t t;
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    sscanf(__DATE__, "%s %hhd %d", s_month, &t.Day, &year);
    sscanf(__TIME__, "%2hhd %*c %2hhd %*c %2hhd", &t.Hour, &t.Minute, &t.Second);

    // Find where is s_month in month_names. Deduce month value.
    t.Month = (strstr(month_names, s_month) - month_names) / 3 + 1;

    // year can be given as '2010' or '10'. It is converted to years since 1970
    if (year > 99)
        t.Year = year - 1970;
    else
        t.Year = year + 30;

    return makeTime(t);
}

namespace OTA
{
    String default_available_repo = ""; // This will be the default returned version when you
    String default_available_release = "";
    HardStuffHttpClient *http_ota;
    enum UpdateCondition
    {
        NO_UPDATE,
        OLD_DIFFERENT,
        NEW_DIFFERENT,
        NEW_SAME
    };
    void confirmConnected()
    {
        if (http_ota->connected())
        {
        }
    }

    void printFirmwareDetails(Stream *print_stream = &Serial)
    {
        print_stream->println("------------------------");
        print_stream->println("Device MAC: " + getMacAddress());
        print_stream->println("Firmware Version: " + (String)OTA_VERSION);
        print_stream->println("Firmware Compilation Date: " + (String)__DATE__ + ", " + (String)__TIME__);
        print_stream->println("------------------------");
    }

    void init(Client &underlying_client)
    {
        printFirmwareDetails();
        http_ota = new HardStuffHttpClient(underlying_client, OTA_GH_SERVER, OTA_PORT);
    }

    void deinit()
    {
        if (http_ota != nullptr)
        {
            http_ota->stop();
            delete http_ota;
            http_ota = nullptr;
        }
    }

    // TODO: Go to GitHub and get latest releases info, unpacking the asset
    UpdateCondition isUpdateAvailable()
    {
        Serial.println("Checking for updates on: " + String(OTAGH_SERVER) + String(OTA_CHECK_PATH));

        // Get the response from the server
        HardStuffHttpRequest request;
#ifdef OTA_BEARER
        request.addHeader("Authorization", "Bearer " + OTA_BEARER);
#endif

        HardStuffHardStuffHttpResponse response = http_ota->getFromHTTPServer(OTA_CHECK_PATH);

        if (response.success())
        {
            JsonDocument release_response;
            deserializeJson(release_response, response.body);


            // TODO: A lot to complete here! It's [0th] item, check "publised_at" time and "name", for return info.

            return ......;

            // Will need to compile the asset ID, too.

            if (release_response.containsKey("release_name") && release_response.containsKey("repo_name"))
            // We have to know the release name if we're going to update!
            {
                default_available_repo = release_response["repo_name"].as<String>();
                default_available_release = release_response["release_name"].as<String>();

                bool update_is_newer = release_response["release_published_at"].as<time_t>() > cvtDate();
                ;
                bool update_is_different = default_available_release.compareTo(OTA_VERSION) != 0;
                return update_is_different ? (update_is_newer ? NEW_DIFFERENT : OLD_DIFFERENT) : (update_is_newer ? NEW_SAME : NO_UPDATE);
            }
        }

        return NO_UPDATE;
    }

    void performUpdate()
    {
        Serial.println("Fetching update from: " + String(OTA_SERVER) + String(OTA_BIN_PATH) + "/" + asset_id);


        HardStuffHttpRequest request;
        // Headers
        request.addHeader("Connection", "keep-alive");
        request.addHeader("Accept", "application/octet-stream");  
#ifdef OTA_BEARER
        request.addHeader("Authorization", "Bearer " + OTA_BEARER);
#endif
        // Params

        // this will return a 302 with a "location" header:
        HardStuffHttpResponse response = http_ota->getFromHTTPServer(OTA_BIN_PATH, &request, true);

        if (response.status_code >= 200 response.status_code < 300 ) {
            // we can download as normal
        }
        if (response.status_code == 302) {
            // Do redirect logic

        }
        for (int i_header = 0; i_header < response.header_count; i_header++)
        {
            if (response.headers[i_header].key.compareTo("Content-Length") == 0)
            {
                contentLength = response.headers[i_header].value.toInt();
            }
            if (response.headers[i_header].key.compareTo("Content-Type") == 0)
            {
                String contentType = response.headers[i_header].value;
                isValidContentType =
                    contentType == "application/octet-stream" || contentType == "application/macbinary";
            }
        }

        if (contentLength && isValidContentType)
        {
            Serial.println("Begin OTA update. This may take a while...");
            if (Update.begin(contentLength))
            {
                Update.writeStream(*http_ota);
                if (Update.end())
                {
                    Serial.println("OTA done!");
                    if (Update.isFinished())
                    {
                        Serial.println("Reboot...");
                        ESP.restart();
                    }
                }

                Serial.println("------------------------------ERROR------------------------------");
                Serial.printf("    ERROR CODE: %d", Update.getError());
                Serial.println("-----------------------------------------------------------------");
            }
            else
            {
                Serial.println("Not enough space available.");
            }
        }
        else
        {
            Serial.println("There was no content in the response");
        }
        http_ota->stop();
    }
}