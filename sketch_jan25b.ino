#include <DHT.h>
#include <ESP8266WiFi.h>

String apiKey = "******";     //  Thingspeak API key'i

const char *ssid =  "***";     // Bağlanılan WiFi adı
const char *pass =  "***";                   // WiFi şifresi
const char* server = "api.thingspeak.com";

//Sıcaklık ve nem sensörü
  #define DHTPIN D2          //DHT sensörünün bağlandığı pin
  DHT dht(DHTPIN, DHT11);
//Sıcaklık ve nem sensörü

//ultrasonic sensör
  const int trigPin = D0;
  const int echoPin = D1;
  long duration;
  int distance;
  int fillLevel;
//ultrasonic sensör

//Gaz sensörü
  int deger;
//Gaz sensörü

WiFiClient client;

void setup() {

  Serial.begin(115200);
  delay(10);
  dht.begin();

//ultrasonic sensör
  pinMode(trigPin, OUTPUT); // trigPin'i veri çıkışı olarak tanımlıyor
  pinMode(echoPin, INPUT); // echoPin'i veri girişi olarak tanımlıyor
  Serial.begin(9600); // seri haberleşmeyi başlatıyor
//ultrasonic sensör

  Serial.println("Şu ağa bağlanıyor: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi bağlandı..");

}

void loop() {

//ultrasonic sensör
  // trigPin'i temizle
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // trigPin'i 10 microsaniyeliğine HIGH seviyesine çıkar.
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);


  // echoPin'den değerleri okur
  duration = pulseIn(echoPin, HIGH);
  
  // Mesafe hesaplanıyor
  distance = duration * 0.034 / 2;

  // Doluluk hesaplanıyor
  fillLevel = 0;
  if (distance < 100) {
    fillLevel = abs(100 - distance);
  }

  //Serial monitör'de doluluk oranını yazar.
  Serial.print("Yüzde: ");
  Serial.print(fillLevel);
  
  if(fillLevel > 90 ){
    Serial.println(" KUTU DOLDU TOPLANMALI!!!!");
  }
  else if(fillLevel > 75){
    Serial.println(" Yarıdan fazlası dolu.");
  }
  else if(fillLevel > 50){
    Serial.println(" Yarı dolu.");
  }
  else if(fillLevel > 25){
    Serial.println(" Çeyrek dolu");
  }
  else{
    Serial.println(" Boş.");
  }
//ultrasonik sensör


//gaz sensörü
  deger = analogRead(A0);

  Serial.print("Gaz degeri: ");
  Serial.print(deger);

  if ( deger < 400) {
    Serial.println(" Guvenli");
  }
  else {
    Serial.println(" UYARI!!!");
  }
//gaz sensörü

//Sıcaklık ve nem sensörü
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("DHT sensöründen veri okuma başarısız!");
    return;
  }
//Sıcaklık ve nem sensörü

  if (client.connect(server, 80)) {

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(fillLevel);
    postStr += "&field2=";
    postStr += String(t);
    postStr += "&field3=";
    postStr += String(h);
    postStr += "&field4=";
    postStr += String(deger);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Bağlantı: kapalı\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("İçerik Tipi: application/x-www-form-urlencoded\n");
    client.print("İçerik Uzunluğu: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Sıcaklık: ");
    Serial.print(t);
    Serial.print(" derece Celcius, Nem: ");
    Serial.println(h);
    Serial.println("%. Thingspeak'e gönder.");
    Serial.println();
  }
  client.stop();

  Serial.println("Bekleniyor...");

  // thingspeak verileri 15 saniyede bir günceller...

  delay(5000);
}
