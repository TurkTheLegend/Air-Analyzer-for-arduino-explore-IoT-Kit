#include <MQUnifiedsensor.h>
#include <SPI.h>
#include <WiFiNINA.h>

//const char* SSID
//const char* PASSWORD
//const char* SCRIPT_ID
const char* HOST = "script.google.com";

const int Pin3 = A0;
const int Pin4 = A1;
const int Pin135 = A2;
const int Pin7 = A3;
const int Pin8 = A4;
const int Pin9 = A5;

const float RatioMQ3CleanAir = 60;
const float RatioMQ4CleanAir = 4.4;
const float RatioMQ135CleanAir = 3.6;
const float RatioMQ7CleanAir = 27.5;
const float RatioMQ8CleanAir = 70;
const float RatioMQ9CleanAir = 9.6;
const int ADC_Bit_Resolution = 10;
const float Voltage_Resolution = 5;

const String Board = "MKR1010";

MQUnifiedsensor MQ3(Board.c_str(), Voltage_Resolution, ADC_Bit_Resolution, Pin3, Board.c_str());
MQUnifiedsensor MQ4(Board.c_str(), Voltage_Resolution, ADC_Bit_Resolution, Pin4, Board.c_str());
MQUnifiedsensor MQ135(Board.c_str(), Voltage_Resolution, ADC_Bit_Resolution, Pin135, Board.c_str());
MQUnifiedsensor MQ7(Board.c_str(), Voltage_Resolution, ADC_Bit_Resolution, Pin7, Board.c_str());
MQUnifiedsensor MQ8(Board.c_str(), Voltage_Resolution, ADC_Bit_Resolution, Pin8, Board.c_str());
MQUnifiedsensor MQ9(Board.c_str(), Voltage_Resolution, ADC_Bit_Resolution, Pin9, Board.c_str());

float Alcohol = 0.0;
float Benzene = 0.0;
float Hexane = 0.0;
float Methane = 0.0;
float Smoke = 0.0;
float Carbondioxide = 0.0;
float Toluene = 0.0;
float Ammonia = 0.0;
float Acetone = 0.0;
float Carbonmonoxide = 0.0;
float Hydrogen = 0.0;
float Flaming_gas = 0.0;

unsigned long time_ms = 0;
unsigned long time_1000_ms_buf = 0;
unsigned long time_sheet_update_buf = 0;

void setup() {
  Serial.begin(9600);
  connectToWiFi();
  MQ3.init();
  MQ3.setRegressionMethod(1);
  MQ3.setR0(0.45);
  MQ4.init();
  MQ4.setRegressionMethod(1);
  MQ4.setR0(14.23);
  MQ135.init();
  MQ135.setRegressionMethod(1);
  MQ135.setR0(9.03);
  MQ7.init();
  MQ7.setRegressionMethod(1);
  MQ7.setR0(5.90);
  MQ8.init();
  MQ8.setRegressionMethod(1);
  MQ8.setR0(0.91);
  MQ9.init();
  MQ9.setRegressionMethod(1);
  MQ9.setR0(13.93);

  Serial.print("Calibrating, please wait.");
  float MQ3calcR0 = 0.0;
  float MQ4calcR0 = 0.0;
  float MQ135calcR0 = 0.0;
  float MQ7calcR0 = 0.0;
  float MQ8calcR0 = 0.0;
  float MQ9calcR0 = 0.0;
  
  for (int i = 1; i <= 10; i++) {
    MQ3.update();
    MQ4.update();
    MQ135.update();
    MQ7.update();
    MQ8.update();
    MQ9.update();

    MQ3calcR0 += MQ3.calibrate(RatioMQ3CleanAir);
    MQ4calcR0 += MQ4.calibrate(RatioMQ4CleanAir);
    MQ135calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    MQ7calcR0 += MQ7.calibrate(RatioMQ7CleanAir);
    MQ8calcR0 += MQ8.calibrate(RatioMQ8CleanAir);
    MQ9calcR0 += MQ9.calibrate(RatioMQ9CleanAir);

    Serial.print(".");
  }

  MQ3.setR0(MQ3calcR0 / 10);
  MQ4.setR0(MQ4calcR0 / 10);
  MQ135.setR0(MQ135calcR0 / 10);
  MQ7.setR0(MQ7calcR0 / 10);
  MQ8.setR0(MQ8calcR0 / 10);
  MQ9.setR0(MQ9calcR0 / 10);
  Serial.println("  done!.");

  Serial.print("(MQ3 - MQ9):");
  Serial.print(MQ3calcR0 / 10);
  Serial.print(" | ");
  Serial.print(MQ4calcR0 / 10);
  Serial.print(" | ");
  Serial.print(MQ135calcR0 / 10);
  Serial.print(" | ");
  Serial.print(MQ7calcR0 / 10);
  Serial.print(" | ");
  Serial.print(MQ8calcR0 / 10);
  Serial.print(" | ");
  Serial.print(MQ9calcR0 / 10);
  Serial.println(" |");
}

void loop() {
  time_ms = millis();

  if (time_ms - time_1000_ms_buf >= 1000) {
    time_1000_ms_buf = time_ms;
    readGasSensors();
  }
  if (time_ms - time_sheet_update_buf >= 60000) {
    time_sheet_update_buf = time_ms;
    updateGoogleSheet();
  }
}

void connectToWiFi() {
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to " + String(SSID));

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SSID, PASSWORD);
    delay(5000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void readGasSensors() {
  MQ3.update();
  MQ4.update();
  MQ135.update();
  MQ7.update();
  MQ8.update();
  MQ9.update();

  MQ3.setA(0.3934);
  MQ3.setB(-1.504);
  Alcohol = MQ3.readSensor();

  MQ3.setA(4.8387);
  MQ3.setB(-2.68);
  Benzene = MQ3.readSensor();

  MQ3.setA(7585.3);
  MQ3.setB(-2.849);
  Hexane = MQ3.readSensor();

  MQ4.setA(1012.7);
  MQ4.setB(-2.786);
  Methane = MQ4.readSensor() / 10;

  MQ4.setA(30000000);
  MQ4.setB(-8.308);
  Smoke = MQ4.readSensor() / 10000;

  MQ135.setA(110.47);
  MQ135.setB(-2.862);
  Carbondioxide = MQ135.readSensor() * 150;

  MQ135.setA(44.947);
  MQ135.setB(-3.445);
  Toluene = MQ135.readSensor();

  MQ135.setA(102.2);
  MQ135.setB(-2.473);
  Ammonia = MQ135.readSensor() / 1000;

  MQ135.setA(34.668);
  MQ135.setB(-3.369);
  Acetone = MQ135.readSensor();

  MQ7.setA(99.042);
  MQ7.setB(-1.518);
  Carbonmonoxide = MQ7.readSensor();

  MQ8.setA(976.97);
  MQ8.setB(-0.688);
  Hydrogen = MQ8.readSensor() / 60;

  MQ9.setA(1000.5);
  MQ9.setB(-2.186);
  Flaming_gas = MQ9.readSensor() / 6;
}

String buildQueryString() {
  String queryString = "Alcohol=" + String(Alcohol) +
                       "&Benzene=" + String(Benzene) +
                       "&Hexane=" + String(Hexane) +
                       "&Methane=" + String(Methane) +
                       "&Smoke=" + String(Smoke) +
                       "&Carbondioxide=" + String(Carbondioxide) +
                       "&Toluene=" + String(Toluene) +
                       "&Ammonia=" + String(Ammonia) +
                       "&Acetone=" + String(Acetone) +
                       "&Carbonmonoxide=" + String(Carbonmonoxide) +
                       "&Hydrogen=" + String(Hydrogen) +
                       "&Flaming_gas=" + String(Flaming_gas);
  return queryString;
}

void updateGoogleSheet() {
  connectToWiFi();
  Serial.print("Connecting to ");
  Serial.println(HOST);
  WiFiSSLClient sheetClient;
  const int sheetHttpsPort = 443;
  if (!sheetClient.connect(HOST, sheetHttpsPort)) {
    Serial.println("Connection failed");
    return updateGoogleSheet();
  }

  String path = String("https://script.google.com") + "/macros/s/" + String(SCRIPT_ID) + "/dev";
  String queryString = buildQueryString();
  String url = path + "?" + queryString;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  sheetClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                    "Host: " + HOST + "\r\n" +
                    "Connection: close\r\n\r\n");

  Serial.println();
  Serial.println("Closing connection");
}