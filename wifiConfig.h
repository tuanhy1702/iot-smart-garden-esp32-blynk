#include <EEPROM.h> 
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
WebServer webServer(80);
#include <Ticker.h>
Ticker blinker;

String ssid;
String password;
#define ledPin 2
#define btnPin 0
unsigned long lastTimePress = millis();
#define PUSHTIME 5000
int wifiMode; // 0: cấu hình, 1: kết nối, 2: mất wifi
unsigned long blinkTime = millis();

const char html[] PROGMEM = R"html(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>SETTING WIFI INFORMATION</title>
  <style type="text/css">
    body{display: flex;justify-content: center;align-items: center;}
    button{width: 135px;height: 40px;margin-top: 10px;border-radius: 5px}
    label, span{font-size: 25px;}
    input{margin-bottom: 10px;width:275px;height: 30px;font-size: 17px;}
    select{margin-bottom: 10px;width: 280px;height: 30px;font-size: 17px;}
  </style>
</head>
<body>
  <div>
    <h3 style="text-align: center;">SETTING WIFI INFORMATION</h3>
    <p id="info" style="text-align: center;">Scanning wifi network...!</p>
    <label>Wifi name:</label><br>
    <select id="ssid">
      <option>Choose wifi name!</option>
    </select><br>
    <label>Password:</label><br>
    <input id="password" type="text"><br>

    <button onclick="saveWifi()" style="background-color: cyan;margin-right: 10px">SAVE</button>
    <button onclick="reStart()" style="background-color: pink;">RESTART</button>
  </div>
  <script type="text/javascript">
    window.onload=function(){ scanWifi(); }
    var xhttp = new XMLHttpRequest();
    function scanWifi(){
      xhttp.onreadystatechange = function(){
        if(xhttp.readyState==4&&xhttp.status==200){
          data = xhttp.responseText;
          document.getElementById("info").innerHTML = "WiFi scan completed!";
          var obj = JSON.parse(data);
          var select = document.getElementById("ssid");
          for(var i=0; i<obj.length;++i){
            select[select.length] = new Option(obj[i],obj[i]);
          }
        }
      }
      xhttp.open("GET","/scanWifi",true);
      xhttp.send();
    }
    function saveWifi(){
      ssid = document.getElementById("ssid").value;
      pass = document.getElementById("password").value;
      xhttp.onreadystatechange = function(){
        if(xhttp.readyState==4&&xhttp.status==200){
          data = xhttp.responseText;
          alert(data);
        }
      }
      xhttp.open("GET","/saveWifi?ssid="+ssid+"&pass="+pass,true);
      xhttp.send();
    }
    function reStart(){
      xhttp.onreadystatechange = function(){
        if(xhttp.readyState==4&&xhttp.status==200){
          data = xhttp.responseText;
          alert(data);
        }
      }
      xhttp.open("GET","/reStart",true);
      xhttp.send();
    }
  </script>
</body>
</html>
)html";

void blinkLed(uint32_t t) {
  if (millis() - blinkTime > t) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    blinkTime = millis();
  }
}

void ledControl() {
  if (digitalRead(btnPin) == LOW) {
    if (millis() - lastTimePress < PUSHTIME) {
      blinkLed(1000);
    } else {
      blinkLed(50);
    }
  } else {
    if (wifiMode == 0) blinkLed(50);
    else if (wifiMode == 1) blinkLed(3000);
    else if (wifiMode == 2) blinkLed(300);
  }
}

// ===== Sửa phần này =====
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:  // Tên mới của SYSTEM_EVENT_STA_GOT_IP
      Serial.println("Connected to WiFi");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      wifiMode = 1;
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: // Tên mới của SYSTEM_EVENT_STA_DISCONNECTED
      Serial.println("Disconnected from WiFi");
      wifiMode = 2;
      WiFi.begin(ssid.c_str(), password.c_str());
      break;

    default:
      break;
  }
}

void setupWifi() {
  if (ssid != "") {
    Serial.println("Connecting to WiFi...!");
    WiFi.mode(WIFI_STA);
    WiFi.onEvent(WiFiEvent);
    WiFi.begin(ssid.c_str(), password.c_str());
  } else {
    Serial.println("ESP32 WiFi network created!");
    WiFi.mode(WIFI_AP);
    uint8_t macAddr[6];
    WiFi.softAPmacAddress(macAddr);
    String ssid_ap = "ESP32-" + String(macAddr[4], HEX) + String(macAddr[5], HEX);
    ssid_ap.toUpperCase();
    WiFi.softAP(ssid_ap.c_str());
    Serial.println("Access point name: " + ssid_ap);
    Serial.println("Web server access address: " + WiFi.softAPIP().toString());
    wifiMode = 0;
  }
}

void setupWebServer() {
  webServer.on("/", [] {
    webServer.send(200, "text/html", html);
  });

  webServer.on("/scanWifi", [] {
    Serial.println("Scanning WiFi network...!");
    int wifi_nets = WiFi.scanNetworks(true, true);
    const unsigned long t = millis();
    while (wifi_nets < 0 && millis() - t < 10000) {
      delay(20);
      wifi_nets = WiFi.scanComplete();
    }
    DynamicJsonDocument doc(400);
    for (int i = 0; i < wifi_nets; ++i) {
      doc.add(WiFi.SSID(i));
    }
    String wifiList;
    serializeJson(doc, wifiList);
    webServer.send(200, "application/json", wifiList);
  });

  webServer.on("/saveWifi", [] {
    String ssid_temp = webServer.arg("ssid");
    String password_temp = webServer.arg("pass");
    Serial.println("SSID: " + ssid_temp);
    Serial.println("PASS: " + password_temp);
    EEPROM.writeString(0, ssid_temp);
    EEPROM.writeString(32, password_temp);
    EEPROM.commit();
    webServer.send(200, "text/plain", "WiFi has been saved!");
  });

  webServer.on("/reStart", [] {
    webServer.send(200, "text/plain", "ESP32 is restarting!");
    delay(3000);
    ESP.restart();
  });

  webServer.begin();
}

void checkButton() {
  if (digitalRead(btnPin) == LOW) {
    Serial.println("Press and hold for 5 seconds to reset to default!");
    if (millis() - lastTimePress > PUSHTIME) {
      for (int i = 0; i < 100; i++) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
      Serial.println("EEPROM memory erased!");
      delay(2000);
      ESP.restart();
    }
    delay(1000);
  } else {
    lastTimePress = millis();
  }
}

class Config {
public:
  void begin() {
    pinMode(ledPin, OUTPUT);
    pinMode(btnPin, INPUT_PULLUP);
    blinker.attach_ms(50, ledControl);
    EEPROM.begin(100);

    ssid = EEPROM.readString(0);
    password = EEPROM.readString(32);

    if (ssid != "") {
      Serial.println("WiFi name: " + ssid);
      Serial.println("Password: " + password);
    }

    setupWifi();
    if (wifiMode == 0) setupWebServer();
  }

  void run() {
    checkButton();
    if (wifiMode == 0) webServer.handleClient();
  }
} wifiConfig;
