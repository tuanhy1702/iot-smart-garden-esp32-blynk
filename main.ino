#include "wifiConfig.h"
#define BLYNK_TEMPLATE_ID "TMPL6q2SPoo8b"
#define BLYNK_TEMPLATE_NAME "Trồng Cây Thông minh"
#define BLYNK_AUTH_TOKEN "vSCJo3pryyBAuk-vsf-vWzZlgr29j3sw"

#include <BlynkSimpleEsp32.h>
#include "DHT.h"

#define DHTPIN 14
#define DHTTYPE DHT11
#define LED 13
#define SOIL_PIN 34
#define PUMP_PIN 25    // bơm
#define LIGHT_SENSOR_PIN 32   // 🌞 Quang trở đọc sáng/tối
#define LIGHT_RELAY_PIN 26    // 💡 Relay điều khiển đèn
#define FAN_RELAY_PIN 27      // 🌬️ Relay điều khiển quạt

DHT dht(DHTPIN, DHTTYPE);
bool blynkConnect = 0;
bool ledBlinking = false;
bool ledState = false;
bool pumpState = false;
bool autoModePump = false;
bool lightState = false;
bool autoModeLight = false;
bool fanState = false;  // 🌬️ Trạng thái quạt

BlynkTimer timer;
BlynkTimer blinkTimer;

// 🧠 Gửi dữ liệu cảm biến lên Blynk
void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int soilValue = analogRead(SOIL_PIN);
  int soilPercent = map(soilValue, 0, 4095, 100, 0);
  soilPercent = constrain(soilPercent, 0, 100);

  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  int lightPercent = map(lightValue, 0, 4095, 0, 100);
  lightPercent = constrain(lightPercent, 0, 100);
  Blynk.virtualWrite(V11, lightPercent);

  if (isnan(h) || isnan(t)) {
    Serial.println("❌ Không đọc được dữ liệu từ DHT11!");
    //return;
  }

  Serial.print("🌡 Nhiệt độ: "); Serial.print(t);
  Serial.print(" °C | 💧 Độ ẩm kk: "); Serial.print(h);
  Serial.print(" % | 🌿 Độ ẩm đất: "); Serial.print(soilPercent);
  Serial.print(" % | ☀️ Ánh sáng: "); Serial.print(lightPercent);
  Serial.println(" %");

  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V7, soilPercent);
  Blynk.virtualWrite(V8, pumpState);
  Blynk.virtualWrite(V9, autoModePump);
  Blynk.virtualWrite(V12, lightState);
  Blynk.virtualWrite(V3, autoModeLight);
  Blynk.virtualWrite(V10, ledBlinking);

  // 💧 Điều khiển bơm tự động
  if (autoModePump) {
    if (soilPercent < 30 && !pumpState) {
      pumpState = true;
      digitalWrite(PUMP_PIN, HIGH);
      Blynk.virtualWrite(V8, true);
      Serial.println("💧 Độ ẩm đất thấp! BẬT máy bơm (AUTO).");
    } 
    else if (soilPercent > 60 && pumpState) {
      pumpState = false;
      digitalWrite(PUMP_PIN, LOW);
      Blynk.virtualWrite(V8, false);
      Serial.println("✅ Độ ẩm đất đủ, TẮT máy bơm (AUTO).");
    }
  }

  // 💡 Điều khiển đèn tự động
  if (autoModeLight) {
    if (lightPercent < 5 && !lightState) {
      lightState = true;
      digitalWrite(LIGHT_RELAY_PIN, HIGH);
      Serial.println("🌙 Tối - BẬT đèn");
      Blynk.virtualWrite(V12, lightState);
    } 
    else if (lightPercent > 5 && lightState) {
      lightState = false;
      digitalWrite(LIGHT_RELAY_PIN, LOW);
      Serial.println("☀️ Sáng - TẮT đèn");
      Blynk.virtualWrite(V12, lightState);
    }
  }

  // 🌬️ Điều khiển quạt khi nhiệt độ cao
  if (t > 26 && !fanState) {
  fanState = true;
  digitalWrite(FAN_RELAY_PIN, HIGH);
  Serial.println("🌬️ Nhiệt độ cao! BẬT quạt làm mát!");
  } 
  else if (t <= 26 && fanState) {
    fanState = false;
    digitalWrite(FAN_RELAY_PIN, LOW);
    Serial.println("✅ Nhiệt độ ổn định. TẮT quạt.");
  }


  // 🔥 LED cảnh báo nhiệt độ cao
  if (t > 26) {
    if (!ledBlinking) {
      ledBlinking = true;
      Blynk.virtualWrite(V10, ledBlinking); 
      blinkTimer.setInterval(500L, []() {
        ledState = !ledState;
        digitalWrite(LED, ledState);
          
      });
      Serial.println("🔥 Nhiệt độ cao! LED nhấp nháy!");
    }
  } else {
    if (ledBlinking) {
      ledBlinking = false;
      blinkTimer.deleteTimer(0);     
      digitalWrite(LED, LOW);
      Blynk.virtualWrite(V10, ledBlinking);
      Serial.println("🌡 Nhiệt độ bình thường. LED tắt.");
    }
    
  }
}

// Blynk Connected
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V9);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(10);
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V12);
}

BLYNK_WRITE(V1) {
  int value = param.asInt();

  if (!ledBlinking) {
    // Điều khiển LED
    digitalWrite(LED, value);

    // Điều khiển quạt song song
    fanState = value;
    digitalWrite(FAN_RELAY_PIN, value);

    Serial.print("💡 LED: ");
    Serial.println(value ? "BẬT" : "TẮT");

    Serial.print("🌬️ Quạt: ");
    Serial.println(value ? "BẬT" : "TẮT");
  }
}


// 💧 Auto Mode Pump
BLYNK_WRITE(V9) {
  autoModePump = param.asInt();
  Serial.print("💧Bơm Auto: ");
  Serial.println(autoModePump ? "BẬT" : "TẮT");
  if (!autoModePump) {
    digitalWrite(PUMP_PIN, LOW);
    pumpState = false;
    Blynk.virtualWrite(V8, 0);
  }
}

// 💡 Auto Mode Light
BLYNK_WRITE(V3) {
  autoModeLight = param.asInt();
  Serial.print("💡Đèn Auto: ");
  Serial.println(autoModeLight ? "BẬT" : "TẮT");
  if (!autoModeLight) {
    digitalWrite(LIGHT_RELAY_PIN, LOW);
    lightState = false;
    Blynk.virtualWrite(V12, 0);
  }
}

// Bơm thủ công
BLYNK_WRITE(V2) {
  int manualPump = param.asInt();

  // Nếu Auto mode đang bật -> không cho bấm thủ công
  if (autoModePump) {
    Blynk.virtualWrite(V2, 0); // Reset nút trên app
    Serial.println("⚠️ Không thể điều khiển bơm khi Auto mode đang bật");
    return;
  }

  // Nếu Auto mode tắt -> cho phép điều khiển thủ công
  if (manualPump == 1 && !pumpState) {
    pumpState = true;
    digitalWrite(PUMP_PIN, HIGH);
    Blynk.virtualWrite(V8, 1);
    Serial.println("💧 BẬT bơm (THỦ CÔNG)");
  } 
  else if (manualPump == 0 && pumpState) {
    pumpState = false;
    digitalWrite(PUMP_PIN, LOW);
    Blynk.virtualWrite(V8, 0);
    Serial.println("✅ TẮT bơm (THỦ CÔNG)");
  }
}

// Đèn thủ công
BLYNK_WRITE(V4) {
  int manualLight = param.asInt();
  if (!autoModeLight) {
    if (manualLight == 1 && !lightState) {
      lightState = true;
      digitalWrite(LIGHT_RELAY_PIN, HIGH);
      Blynk.virtualWrite(V12, 1);
      Serial.println("💡 BẬT đèn (THỦ CÔNG)");
    } 
    else if (manualLight == 0 && lightState) {
      lightState = false;
      digitalWrite(LIGHT_RELAY_PIN, LOW);
      Blynk.virtualWrite(V12, 0);
      Serial.println("✅ TẮT đèn (THỦ CÔNG)");
    }
  } else {
    Blynk.virtualWrite(V4, 0);
    Serial.println("⚠️ Không điều khiển được khi Auto ON");
  }
}

void setup() {
  Serial.begin(115200);
  wifiConfig.begin();

  pinMode(LED, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT); // 🌬️ Quạt
  dht.begin();

  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(LIGHT_RELAY_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, LOW);

  Blynk.config(BLYNK_AUTH_TOKEN, "blynk.cloud", 80);
  timer.setInterval(2000L, sendSensor);
}

void loop() {
  wifiConfig.run();
  timer.run();
  blinkTimer.run();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ Mất WiFi...");
    WiFi.reconnect();
    delay(2000);
    return;
  }

  if (!Blynk.connected()) {
    Serial.println("⚠️ Mất Blynk...");
    if (Blynk.connect(5000)) {
      Serial.println("✅ Kết nối lại thành công!");
    } else {
      delay(5000);
      return;
    }
  }

  Blynk.run();
}
