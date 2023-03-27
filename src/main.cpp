/*
  Titre      : Projet Final Examen
  Auteur     : Yvan Tankeu
  Date       : 27/03/2023
  Description: Patie arduino
    1- Création du serveur : il s'agit de mettre en place une infrastructure serveur 
            capable de stocker et de traiter les données météorologiques.
    2- Conception des objets intelligents : il faut concevoir des capteurs et autres
            dispositifs électroniques pour collecter les données météorologiques.
    3- Affichage des données via une interface web : une fois les données collectées et 
            stockées, elles doivent être présentées via une interface web conviviale et facile à utiliser.
  Version    : 0.0.1
*/

#include <Arduino.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#endif
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE DHT22
WiFiClient client;

const int DHTPIN = 5;
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 14, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

double temperature, humidite;

const int serverPort = 5500; // Port utilisé par le serveur
IPAddress server(192, 168, 160, 203); 

//const char ipMonOrdi[] = "192.168.2.50";

const char ssid[] = "Galaxy";
const char pass[] = "12345678";

void setup()
{
  Serial.begin(9600);
  
  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println("You're connected to the network");
  Serial.println();

  dht.begin();
  delay(10);
  lcd.begin(20, 4);
  lcd.setCursor(0,0);
  lcd.print("Station Météorologique");
  lcd.setCursor(0,1);
  lcd.print("Machine learning");
  lcd.setCursor(0,2);
  lcd.print("au CCNB Campus de Bathurst");
  lcd.setCursor(0,3);
  lcd.print("!!!...");
  delay(2000);
  lcd.clear();
  Serial.println("REBOOT");
}
void loop()
{
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;

  dht.temperature().getEvent(&event);
  temperature = event.temperature;
  dht.humidity().getEvent(&event);
  humidite = event.relative_humidity;

  if (isnan(temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }
  else
  {

    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(humidite))
  {
    Serial.println(F("Error reading humidity!"));
  }
  else
  {
    Serial.print(F("Humidity: "));
    Serial.print(humidite);
    Serial.println(F("%"));
    Serial.println();
  }

   // Calcul de la probabilité de pluie en pourcentage
  double k = 0.6;  // Valeur de la constante k
  double probabilityRain = 0.03743225 * humidite - 2.21389335 * temperature - 0.09773871 * k + 198.85713381876218;
  
  // Affichage des valeurs sur l'écran LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C");

  lcd.setCursor(9, 0);
  lcd.print("H:");
  lcd.print(humidite);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(k);
  lcd.print("%");

  if (client.connect(server, 5500)) {
    String postData = "t = " + String(temperature) + " h = " + String(humidite);
    client.print(postData);
  }
  else {
    Serial.println("Connection failed.");
  }

  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Rain Probability:");
  lcd.setCursor(0, 1);
  lcd.print(probabilityRain, 2);
  lcd.print("%");
  delay(3000);
}