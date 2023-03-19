#define ANALOG_PIN A1
#define VREF 5000 // ADC's reference voltage on your Arduino,typical value:5000mV
#define PRINT_INTERVAL 0
#define FULLLEVEL 1208   // 100 %
#define EMPTYLEVEL 460   // 0 %

#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

// WIFI setup
String percentageString = "";
char ssid[] = "Ramsay24"; 
char pass[] = "fochfoch";
int status = WL_IDLE_STATUS;

char serverAddress[] = "temperature.paullaney.com.au";
int port = 443;

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
String response;
int statusCode = 0;


// Read voltage setup
int16_t dataVoltage;
float dataCurrent, total, average; 
unsigned long timepoint_measure;
int counter;
int percentage;
char cstr[16];

// Initialize LED Pins
int ledGreen = 12;
int ledYellow = 10;

void setup()
{
  Serial.begin(9600);
  pinMode(ANALOG_PIN, INPUT);
  timepoint_measure = millis();

  // LED 
  pinMode(ledGreen, OUTPUT);     
  pinMode(ledYellow, OUTPUT); 
  digitalWrite(ledGreen, HIGH);  

  counter = 0;
  total = 0;

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Attempt to connect to WiFi network:
  connectToWifi();

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
  
}


void loop()
{
  // Turn the LED on
  digitalWrite(ledYellow, LOW);

  if (millis() - timepoint_measure > PRINT_INTERVAL) {
    counter++;
    timepoint_measure = millis();

    dataVoltage = analogRead(ANALOG_PIN)/ 1024.0 * VREF;
    dataCurrent = dataVoltage / 120.0; //Sense Resistor:120ohm
    
    total = total + dataVoltage;

  }

  //Serial.print("Counter:");
   // Serial.println(counter);
  ///  Serial.println("====================================");
  
  // Only send every 1000
  //if (counter == 10000) {
  if (counter == 100) {
    digitalWrite(ledYellow, HIGH);
    delay(200);
    digitalWrite(ledYellow, LOW);

    // Calculation
    average = total/counter;
    percentage = (average-EMPTYLEVEL)/(FULLLEVEL - EMPTYLEVEL) * 100;
    itoa(percentage, cstr, 10);

    // Print some debug data
    Serial.println("====================================");
    Serial.print("Percentage:");
    Serial.println(percentage);
    Serial.println("====================================");


    // Send the data
    // If we get a reading greater than 100
//    int percentInt = percentage.toInt();
    if (percentage > 100) {
      percentage = 100;
    }

    
    // Print some debug data
    Serial.println("====================================");
    Serial.print("Percentage ---- 2:");
    Serial.println(percentage);
    Serial.println("====================================");

    StaticJsonDocument<200> json;
    json["zone"] = "outside";
    json["water"] = percentage;

    // Send the data 
    String postData;
    serializeJson(json, postData);

        Serial.println("++++++++++++++++++++++++++++++++++++");
    Serial.print("Post Data:");
    Serial.println(postData);
    Serial.println("++++++++++++++++++++++++++++++++++++");;

    client.beginRequest();
    Serial.println("1");
    client.post("/v1/outside");
    Serial.println("2");
    client.sendHeader("Content-Type", "application/json");
    Serial.println("3");
    client.sendHeader("Content-Length", postData.length());
    Serial.println("4");
    client.sendHeader("x-api-key", "dWGajIpe8Lafbg2c3qm14acNbX1ZwKCvaLgc62H1");
    Serial.println("5");
    client.beginBody();
    Serial.println("6");
    client.print(postData);
    Serial.println("7");
    client.endRequest();
    Serial.println("8");


//        statusCode = client.responseStatusCode();
        Serial.println("++++++++++++++++++++++++++++++++++++");
    Serial.print("Status Code:");
    Serial.println(client.responseStatusCode();
    Serial.println("++++++++++++++++++++++++++++++++++++");;

            Serial.println("++++++++++++++++++++++++++++++++++++");
    Serial.print("Responce:");
    Serial.println(client.responseBody());
    Serial.println("++++++++++++++++++++++++++++++++++++");;
//

//    response = client.responseBody();

    

    // Reset the counter
    counter = 0;
    total = 0;
    delay(200);
  }
}

void connectToWifi() {
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}
void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
