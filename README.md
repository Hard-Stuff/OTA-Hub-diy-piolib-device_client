# **Hard Stuff** libs: Http wrapper

This lib is designed to abstract complexity away from using the ArduinoHttpClient library, specifically for GET and POST requests. This means:

-   provide useful structs for bundling request and response details, `HardStuffHttpRequest` and `HardStuffHttpResponse`
-   calling high-level functions rather than low-level functions (e.g. `getFromHTTPServer` with bundled request details and returning bundled response details, instead of `beginRequest` then `get` then manually loading each header then manually compiling the params in the url then...)

## Usage

This is specifically designed for GET and POST requests. Use more-or-less as normal, but leverage the advantages of the bundled `HardStuffHttpRequest` and `HardStuffHttpRespones` structs.

### Basic Example

```cpp
#include <configs/wiremock.h> // Define your wiremock details in a config

#include <WiFiClientSecure.h>
WiFiClientSecure wifi_client;

#include <HardStuff_pio_lib_Http.hpp>
HardStuffHttpClient http_wiremock(wifi_client, WIREMOCK_SERVER, WIREMOCK_PORT); // Create an HTTP wrapper around the client

void setup() {
    SIM7600::init();
    wifi_client.setCACert(WIREMOCK_CERT_CA); // Set the SSL certificate for secure communication between client and server.
}

void loop() {
    HardStuffHttpRequest request;
    request.addParam("device_mac", getMacAddress()); // Include the device's mac address as a http parameter

    HardStuffHttpResponse response = http_wiremock.getFromHTTPServer("/hello_world", request); // Perform a get request on the /hello_world endpoint
    response.print(); // print the HTTP response
    delay(1000);
}
```

### Struct functions available

#### `HardStuffHttpRequest`

-   `addHeader(key, value)` allows you to add a header to the request (maximum of 10).
-   `addParam(key, value)` allows you to add a parameter to the request (maximum of 10).
-   `clear(ignore_headers)` allows you to clear the request (but selectably remove the headers).
-   `print(ignore_headers)` prints the HardStuffHttpRequest to Serial (useful for debugging)
-   the variable `content` is your content String. You can, for example, stringify a JSON into this variable.

#### `HardStuffHttpResponse`

-   `success()` quickly bool returns if the request was a success (result code is between 200 and 300).
-   `print()` prints the HardStuffHttpResponse to Serial
-   `clear()` wipes all variables in the HardStuffHttpResponse (you might use this instead of descoping/deleting if you're stack conscious).
-   the variable `body` is the response body String. You can, for example, JSONify this into a variable.
-   the variable `headers` are the headers returned.

### Functions available

-   `postToHTTPServer(String endpoint, HardStuffHttpRequest *request)` is for performing a POST request to a given endpoint. Use the HardStuffHttpRequest to define the headers, params, and content to post.
-   `getFromHTTPServer(String endpoint, HardStuffHttpRequest *request, bool skip_body)` is for performing a GET request to a given endpoint. You can include request params/headers (optional), and you can choose to skip_body if, for example, you're streaming the content into a file or memory during OTA updates.

### Dependencies

-   arduino-libraries/ArduinoHttpClient
-   paulstoffregen/Time

## Compabibility and testing

This library has been tested on the ESP32S3 with both the internal WiFi functionality and a SIMCOM SIM7600G for various purposes include OTA updates, and GET/POST requests to Airtable, AWS, and Wiremock.

## Hard Stuff

Hard Stuff is a hardware prototyping agency and venture studio focussing on sustainability tech, based in London, UK.
Find out more at [hard-stuff.com](hard-stuff.com).

This library is written and provided open-source in the hope that you go on to build great things.
