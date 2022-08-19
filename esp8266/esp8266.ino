#include "RSAAuth.h"                         // Khai báo sử dụng thư viện RSA (Rx = D1, Tx = D2)
#include <ArduinoJson.h>                     // Khai báo sử dụng thư viện đọc chuỗi Json
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Thông tin về wifi
#define ssid "Hai Anh"
#define password "123456789"
#define mqtt_server "116.118.48.230"  //ip broker mqtt
#define mqtt_user "doan2"         // Tài khoản đăng nhập broker
#define mqtt_pwd  "doan2"   // Mật khẩu đăng nhập broker
const uint16_t mqtt_port = 1883; //Port của CloudMQTT TCP

#define BAUD_RATE 115200                       // Tốc độ giao tiếp UART BAUD_RATE bit/s

WiFiClient espClient;
PubSubClient client(espClient);
StaticJsonDocument<200> jsonBuffer;
DynamicJsonDocument doc(2048); // Khởi tạo document Jon động với dữ liệu được lưu trong bộ nhớ heap
// trong đề tài này chúng em sử dụng 2kb <=> 2028 byte
// khuyến khích document có kích thước lớn hơn 1kb

/////______Các biến cho việc gửi và nhận______/////
char  cstr[255];
byte  m[255];
byte  buff[255];
int   BEGIN = 0;
int   END = 0;
int   DesLen = 0;

/////______Các biến cho việc gửi và nhận______/////
// Hàm kết nối wifi

void setup_wifi();
void handle(String payload);
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void receive_data_from_arduino_mega();

void setup()
{
  Serial.begin(BAUD_RATE);    // Tốc độ giao tiếp với máy tính
  swSer.begin(BAUD_RATE);     // Tốc độ giao tiếp UART giữa 2 esp BAUD_RATE bit/s
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())// Kiểm tra kết nối
    reconnect();
  client.loop();
  receive_data_from_arduino_mega();
}

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// Hàm call back để nhận dữ liệu
// Hàm call back để nhận dữ liệu
void callback(char* topic, byte* payload, unsigned int length)
{
  for (int i = 0; i < length; i++) {
    cstr[i] = 0;
    cstr[i] = (char)payload[i];
  }
  yield();                                        // Delay một khoản thời gian rất ngắn
  rsa.Send(cstr);                                 // Gửi chuỗi cstr qua device
}

// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect()
{
  while (!client.connected()) // Chờ tới khi kết nối
  {
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP8266Client", mqtt_user, mqtt_pwd)) //kết nối vào broker
    {
      Serial.println("Đã kết nối:");
      client.subscribe("doan2/getState");
      client.subscribe("doan2/onOff/led"); //đăng kí topic đễ nhận dữ liệu từ client
      client.subscribe("doan2/onOff/led/all");
      client.subscribe("doan2/onOff/door");
      client.subscribe("doan2/onOff/fan");
      client.subscribe("doan2/onOff/fan/all");
    }
    else
    {
      Serial.print("Lỗi:, rc=");
      Serial.print(client.state());
      Serial.println("Thu lai sau 5s");
      // Đợi 5s
      delay(5000);
    }
  }
}

void handle(String payload) {
  deserializeJson(doc, payload);         // Khởi tạo một document có kích thước động là 2kb để phân tích chuỗi Json truyền vào
  JsonObject obj = doc.as<JsonObject>(); // Tạo 1 obj có kiểu dữ liệu là JsonObject với document vừa tạo
  byte idtp = obj["idtp"];                // ID Topic
  byte idc = obj["idc"];                // ID connect
  byte idgw = obj["id"];                // ID getway
  if (idc == 1 && idgw == 1) {
    switch (idtp) {
      case 0:
        client.publish("doan2/status", (char*)m); // gửi dữ liệu lên topic doan2/status
        break;
         case 1:
        client.publish("doan2/onOff/feedback", (char*)m); // gửi dữ liệu lên topic doan2/status
        break;
      default: break;
    }
  }
}

void receive_data_from_arduino_mega() {
  DesLen = 0;
  if (rsa.Receive(buff, &BEGIN, &END, m, &DesLen)) // Nếu có dữ liệu từ device truyền qua
    // Nó sẽ kiểm tra dữ liệu nhận được đúng với dữ liệu mà device truyền qua
    // thì thực hiện pub lên broker
  {
    if (DesLen > 0)
    {
      handle((char*)m);
      Serial.println((char*)m);
    }
  }
  else Serial.println("Nhan error:");
}
