# iot-smart-garden-esp32-blynk
Dự án Hệ thống Trồng Cây Thông Minh sử dụng ESP32 + Blynk IoT để theo dõi và điều khiển môi trường trồng cây.
🌱 Smart Garden ESP32 – Hệ thống Trồng Cây Thông Minh
📌 Giới thiệu

Dự án Hệ thống Trồng Cây Thông Minh sử dụng ESP32 + Blynk IoT để theo dõi và điều khiển môi trường trồng cây.
Hệ thống có khả năng:

Đo nhiệt độ, độ ẩm không khí (DHT11)

Đo độ ẩm đất

Đo ánh sáng môi trường (quang trở/LDR)

Tự động bật/tắt:

💧 Máy bơm nước

💡 Đèn chiếu sáng

🌬️ Quạt làm mát

🔥 LED cảnh báo nhiệt độ cao

Điều khiển thiết bị thủ công từ app Blynk

Chuyển đổi Auto / Manual mode dễ dàng

Gửi dữ liệu real-time lên Blynk Cloud

🛠️ Công nghệ sử dụng

ESP32

Blynk IoT

Arduino C/C++

DHT11 – Sensor đo nhiệt độ & độ ẩm

Soil Moisture Sensor – Cảm biến độ ẩm đất

LDR Light Sensor – Cảm biến ánh sáng

Relay Module – Điều khiển bơm, đèn, quạt

🔧 Tính năng chính
🔹 1. Đo và hiển thị dữ liệu

Nhiệt độ (°C)

Độ ẩm không khí (%)

Độ ẩm đất (%)

Cường độ ánh sáng (%)

🔹 2. Điều khiển tự động

Máy bơm tự bật khi đất < 30% và tắt khi đạt > 60%

Đèn tự bật khi trời tối (< 5%)

Quạt tự bật khi nhiệt độ > 27°C

LED nhấp nháy khi nhiệt độ cao để cảnh báo

🔹 3. Điều khiển thủ công qua Blynk

ON/OFF bơm

ON/OFF đèn

Bật LED cảnh báo

Bật quạt khi cần

Chặn điều khiển thủ công khi đang ở Auto Mode

🔹 4. Quản lý WiFi thông minh

WiFi auto reconnect

Kết nối lại Blynk khi mất mạng

Cấu hình WiFi qua web AP (nếu bạn dùng wifiConfig)

🔌 Sơ đồ chân (ESP32)
Thiết bị	Pin ESP32
DHT11	GPIO 14
Soil sensor	GPIO 34
LDR (Light sensor)	GPIO 32
Relay – Bơm nước	GPIO 25
Relay – Đèn	GPIO 26
Relay – Quạt	GPIO 27
LED cảnh báo	GPIO 13
🧩 Hình ảnh

Ảnh app Blynk

<img width="1410" height="708" alt="image" src="https://github.com/user-attachments/assets/f92adf06-ec9f-4625-8ed5-62878d1e4dc6" />


▶️ Cách sử dụng

Clone repo

Mở bằng Arduino IDE / PlatformIO

Thêm Blynk Library + DHT sensors

Chỉnh thông tin:

BLYNK_AUTH_TOKEN

Template ID

WiFi config

Upload lên ESP32
