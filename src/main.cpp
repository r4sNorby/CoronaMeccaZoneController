#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "M5Stack.h"
#include <M5_ENV.h>
#include <Adafruit_BMP280.h>

// Env Sensors
SHT3X sht30;
QMP6988 qmp6988;

// Clients
WiFiClient espClient;
PubSubClient client(espClient);

// Configure the name and password of the connected wifi and your MQTT Serve host.
const char *ssid = "CoronaMecca";
const char *password = "ckebab1234";
const char *mqtt_server = "192.168.1.114";
const char *mqtt_username = "client";
const char *mqtt_pass = "test";

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
bool running = false;

// FUNCTIONS
// Wifi
void setupWifi();
char* getEnvData();

// MQTT pubsubClient callback and client-connect
void callback(char *topic, byte *payload, unsigned int length);
void connect();

// ENV Sensor data
float tmp = 0.0;
float hum = 0.0;
float pressure = 0.0;

void setup()
{
    M5.begin();
    M5.Power.begin();
    setupWifi();
    client.setServer(mqtt_server, 1883); // Sets the server details.
    client.setCallback(callback);        // Sets the message callback function.
    M5.lcd.setTextSize(1);               // Set the text size to 2.

    Wire.begin(); // Wire init, adding the I2C bus.
    qmp6988.init();

    connect(); // Connect to client

    M5.Lcd.println("Please press 'Left' to send data");
}

void loop()
{
    // M5.Lcd.clear(); // Clear the screen.
    if (!client.connected())
    {
        connect();
    }
    client.loop(); // This function is called periodically to allow clients to
                   // process incoming messages and maintain connections to the
                   // server.

    unsigned long now = millis(); // Obtain the host startup duration.

    M5.update(); // Check the status of the key.
    if (M5.BtnA.wasPressed())
    {
        if (running == false)
        {
            running = true;
        }
        else
        {
            running = false;
        }
    }

    if (running == true)
    {
        if (now - lastMsg > 10000)
        {
            lastMsg = now;
            ++value;

            getEnvData();

            client.publish("test", msg); // Publishes a message to the specified topic.

            if (value % 6 == 0)
            {
                M5.Lcd.clear();
                M5.Lcd.setCursor(0, 0);
            }
        }
    }
}

void setupWifi()
{
    delay(10);
    M5.Lcd.printf("Connecting to %s", ssid);
    WiFi.mode(WIFI_STA);        // Set the mode to WiFi station mode.
    WiFi.begin(ssid, password); // Start Wifi connection.

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.printf("\nSuccess\n");
}

void callback(char *topic, byte *payload, unsigned int length)
{
    M5.Lcd.print("Message arrived [");
    M5.Lcd.print(topic);
    M5.Lcd.print("] ");
    for (int i = 0; i < length; i++)
    {
        M5.Lcd.print((char)payload[i]);
    }
    M5.Lcd.println();
}

void connect()
{
    while (!client.connected())
    {
        M5.Lcd.print("Attempting MQTT connection...");
        // Create a random client ID.
        String clientId = "M5Stack-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect.
        if (client.connect(clientId.c_str(), "client", "pass"))
        {
            M5.Lcd.printf("\nSuccess\n");
            // Once connected, publish an announcement to the topic.
            client.publish("test", "hello world");
            // ... and resubscribe.
            client.subscribe("test");
        }
        else
        {
            M5.Lcd.print("failed, rc=");
            M5.Lcd.print(client.state());
            M5.Lcd.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

char* getEnvData()
{
    pressure = qmp6988.calcPressure();
    if (sht30.get() == 0)
    {                      // Obtain the data of SHT30.
        tmp = sht30.cTemp; // Store the temperature obtained from SHT30.

        hum = sht30.humidity; // Store the humidity obtained from the SHT30.
    }
    else
    {
        tmp = 0, hum = 0;
    }
    M5.lcd.fillRect(0, 10, 100, 60, BLACK); // Fill the screen with black (to clear the screen).
    M5.lcd.setCursor(0, 10);

    snprintf(msg, MSG_BUFFER_SIZE, "{Temperature: %2.1f, Humidity: %2.0f%%}", tmp, hum); // Format to the specified string and store it in MSG.

    M5.Lcd.println(msg);

    return msg;
}