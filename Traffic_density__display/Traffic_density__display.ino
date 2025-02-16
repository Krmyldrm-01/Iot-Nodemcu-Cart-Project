#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>//http isteği ve api 
#include <ArduinoJson.h> 
#include <ThingSpeak.h> // kullanıcağımız site için kütüphane  
const char *wifiName = "Kerem İphone";   // baglanilan wifi aginin adi
const char *password = "keremkerem010203";//wifi şifre 


const char *apiPoint = "************************m";//bağlanılacak api noktası
const char *apiKey ="*********************";//google maps api key 
const uint16_t port = 443;// baglanilacak sunucu portu
// İlçe Adları ve Koordinatları
String district[] = {"Merkez", "Tavşanlı", "Simav", "Gediz", "Domaniç"};
String coordinate[] = {
  "39.4236,29.9831", // Merkez
  "39.5458,29.4937", // Tavşanlı
  "39.0881,28.9778", // Simav
  "39.0431,29.4028", // Gediz
  "39.5245,29.4749", // Domaniç
  
};
//kullanıcı seçimi yapılacağından dolayıbeliirli ilçeler dizeler halinde göterilmiştir 


  //thingspeak için gerekli olan veriler 
//#define channneld 2793068;//thingspeak sitesinde ki bize verilen channel id
//#define writeApiKey "DSHFH4YKR1WFJ69V"; //yazma için verilen api key

const int districtNumber = sizeof(district) / sizeof(district[0]);
int selectedDistrict = -1; // İlçe seçimi için indeks
String satir="";//http isteği sonucu gelen veri için 
WiFiClientSecure  wifiClientTraffic; //http isteği için kullanılan istemci
WiFiClient w;//thingspeak wificlientsecure ile hata verdiğinden dlayı bu istemci ile yaapılmıştır

void setup()
{

  Serial.begin(9600);               // seri portu başlat 
  delay(200);                                   // 200 ms bekle
  Serial.println("Wifi agina baglaniyoruz");  

  WiFi.mode(WIFI_STA);                 // istasyon modunda calis
  WiFi.begin(wifiName, password);          // wifi agina baglan

  //------- Wifi ağına bağlanıncaya kadar beklenilen kısım ------------
  while (WiFi.status() != WL_CONNECTED) 
  { 
    delay(500);                                 // Bu arada her 500 ms de bir seri ekrana yan yana noktalar yazdırılarak
    Serial.print(".");                          //görsellik sağlanıyor.
  }
  //-------------------------------------------------------------------
  
  Serial.println("");                           //Bir alt satıra geçiliyor.
  Serial.println("Bağlantı sağlandı...");       //seri ekrana bağlantının kurulduğu bilgisi gönderiliyor.

  Serial.print("Alınan IP addresi: ");          // kablosuz ağdan alınan IP adresi
  Serial.println(WiFi.localIP());               // kablosuz ağdan alınan IP adresi

  ThingSpeak.begin(w);  
 //begin metodu ile thingspeak e bağlanma işlemi gerçekleşmekte  
   
   // ilçe seçimi
  Serial.println("===========================================================");
  Serial.println("Lütfen aşağıdaki ilçelerden birini seçin:");
  Serial.println("===========================================================");
  for (int i = 0; i < districtNumber; i++) 
  {
    Serial.printf("%d: %s\n", i, district[i].c_str());//ilçelerin düzgün bir şekilde gösterimi için c_str() ifadesi kullanılmıştır 
  }
  Serial.printf("=>");

}




void loop() {
    handleDistrictSelection();  // İlçe seçimi işlemini dışarıya aldık
    delay(1000);  // loop hızını yavaşlatmak için
   }

// İlçe seçimini ve işlemleri yöneten metod
void handleDistrictSelection() {
    // Eğer kullanıcı ilçe seçmediyse ve seri port üzerinden veri varsa:
    if (selectedDistrict == -1 && Serial.available() > 0) {
        String input = Serial.readStringUntil('\n'); //kullanıcı veri girişi için
        input.trim();//boşlukları silmek için 
        int index = input.toInt();//girilen değeri int çevrimi
        
        if (index >= 0 && index < districtNumber) {
            selectedDistrict = index;
            Serial.printf("Seçilen ilçe: %s\n", district[selectedDistrict].c_str());
            futureTrafficData(coordinate[selectedDistrict]);

            // İlçe seçildikten sonra kullanıcıya başka işlem yapmak isteyip istemediğini sor
            Serial.println("Başka bir işlem yapmak ister misiniz? (Evet/Hayır)");

            while (Serial.available() == 0) {
                // Bu döngüde herhangi bir giriş bekliyoruz
            }

            String userChoice = Serial.readStringUntil('\n');
            userChoice.trim();
            userChoice.toLowerCase();  // Küçük harfe dönüştür

            if (userChoice == "evet") {
                // Evet derse ilçe seçimine geri dön
                selectedDistrict = -1;
                Serial.println("Yeni bir ilçe seçin:");
                for (int i = 0; i < districtNumber; i++) {
                    Serial.printf("%d: %s\n", i, district[i].c_str());
                }
            } 
            else if (userChoice == "hayır") {
                // Hayır derse program sonlansın
                Serial.println("Uygulama sonlandırılıyor...");
                Serial.end();  // Seri portu kapat
                while (true);  // Program sonlanıyor
            } 
            else {
                Serial.println("Geçersiz seçim! 'Evet' ya da 'Hayır' yazın.");
            }
        } 
        else {
            Serial.println("Hatalı seçim! Geçerli bir ilçe numarası giriniz.");
        }
    }
}

void futureTrafficData(String data) 
{
  if (wifiClientTraffic.connect(apiPoint, port)) {
  Serial.println("Sunucuya baglanildi...");

   if (wifiClientTraffic.connect(apiPoint, port)) { // Sunucuya bağlan
     String kdpuCoordinate = "39.4569,29.9805";  // Kütahya Dumlupınar Üniversitesi'nin koordinatları
String http_req = "GET /maps/api/distancematrix/json?origins=" + kdpuCoordinate + "&destinations=" + data + 
                  "&key=" + apiKey + "&departure_time=now&traffic_model=best_guess HTTP/1.1\r\n";
http_req += "Host: " + String(apiPoint) + "\r\n";  // Host bilgisi ekleyin
http_req += "Connection: close\r\n\r\n";          // Bağlantı sonlandırma

 Serial.println("\nAtilan istek");
     Serial.println(http_req);  // İstek bilgilerini ekrana yazdır
    wifiClientTraffic.print(http_req);  
   // 5 saniye içinde cevap almayı bekle
      unsigned long onceki_zaman = millis();
      while (wifiClientTraffic.available() == 0) {
        if (millis() - onceki_zaman > 5000) {   
          Serial.println("Baglanti hatasi");
          wifiClientTraffic.stop();
          delay(2000);
          return;
        }
      }

     while (wifiClientTraffic.available()) 
     {
      satir=wifiClientTraffic.readStringUntil('\r');
       Serial.println(satir);
     }
   

    // JSON verisini işleyelim
      DynamicJsonDocument doc(2048);// JSON verilerini saklamak için 2048 baytlık bir bellek ayırarak dinamik bir JSON dokümanı oluşturur.
      deserializeJson(doc, satir); // 'satir' değişkenindeki JSON verilerini çözümleyerek (parse ederek) 'doc' isimli JSON dokümanına yükler.

      // Trafik yoğunluğu bilgisini al
      const char* status = doc["rows"][0]["elements"][0]["status"];
      if (strcmp(status, "OK") == 0) {
        // Mesafe bilgilerini kontrol et
      if (doc["rows"][0]["elements"][0]["distance"]["text"].isNull()) {
        Serial.println("Mesafe bilgisi bulunamadı.");
      } else {
        String distanceText = doc["rows"][0]["elements"][0]["distance"]["text"];
        Serial.printf("Mesafe: %s\n", distanceText.c_str());
        ThingSpeak.setField(4, distanceText);  // Field 1'e mesafe bilgisini gönder
      }

        // Trafik süresi ve normal süreyi al
        int durationMinutes = doc["rows"][0]["elements"][0]["duration"]["value"];  // normal süre
        int durationInTrafficMinutes = doc["rows"][0]["elements"][0]["duration_in_traffic"]["value"]; // trafik içindeki süre
        
        int trafficDensity = 0;
if (durationMinutes > 0) {
    trafficDensity = map(durationInTrafficMinutes, durationMinutes, durationMinutes * 2, 0, 100);
    if (trafficDensity < 0) trafficDensity = 0;
    if (trafficDensity > 100) trafficDensity = 100;
}

        int tSminute = durationInTrafficMinutes/60;
        // Saat ve dakika olarak dönüştür
        int hours = durationInTrafficMinutes / 3600;
        int minutes = durationInTrafficMinutes % 60;
        String timeInTraffic = String(hours) + " saat " + String(minutes) + " dakika";
// Sonuçları ThingSpeak'e gönder
ThingSpeak.setField(5,minutes);  // Field 2'ye trafik süresi bilgisi
ThingSpeak.setField(6,hours);  // Field 2'ye trafik süresi bilgisi
        // Sonuçları yazdır
        // Serial.printf("Mesafe: %f km\n", distanceKilometers);
        Serial.printf("Trafik Süresi (%s): %d saat %d dakika\n", district[selectedDistrict].c_str(), hours, minutes);
        Serial.printf("Trafik Yoğunluğu: %d%%\n", trafficDensity);
          int x = ThingSpeak.writeFields(2793068, "PYO7ACHQKMU6N2GQ");//channel-id write-key
        if(x == 200){
         Serial.println("YES");
         }
        else{
      Serial.println("NO" + String(x));
        }  
      } else {
        Serial.println("Trafik verisi alınamadı.");
      }
  } else {
  Serial.println("Sunucuya baglanilamadi!"); 
}
}
}
