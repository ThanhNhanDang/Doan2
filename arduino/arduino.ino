#include "RSAAuth.h"                         // Khai báo sử dụng thư viện RSA (Rx = 2, Tx = 3)
#include <ArduinoJson.h>
#include "DHT.h"           // Khai báo sử dụng thư viện dht
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AsyncTimer.h>
#include <SPI.h>
#include <DS3231.h>
/* Include the RFID library */
#include <RFID.h>

#define DHTTYPE1   DHT11    // Cảm biến sử dụng
#define DHTPin1    A0       // Chân cảm biến 
#define DHTTYPE2   DHT22    // Cảm biến sử dụng
#define DHTPin2    A1       // Chân cảm biến 
#define BAUD_RATE  115200                       // Tốc độ giao tiếp UART BAUD_RATE bit/s
#define output 5
#define SERVO_PIN 3
#define SDA_DIO 8  // Define the DIO used for the SDA (SS) and RST (reset) pins.
#define RESET_DIO 9
#define SDA 20
#define SCL 21
#define button_door 48
#define button_led1 44
#define button_led2 45
#define button_led3 46
#define button_led4 47
#define button_led_all 49

byte button_door_state;             // the current reading from the input pin
byte last_button_door_state = LOW;   // the previous reading from the input pin

byte button_led_states [] = {0, 0, 0, 0, 0};    // {led 1, led 2, led 3, led 4, led all}
byte last_button_led_states [] = {0, 0, 0, 0, 0};  //{led 1, led 2, led 3, led 4, led all}

const byte ButtonPinsLed[] = {button_led1, button_led2, button_led3, button_led4, button_led_all};

unsigned long last_button_debounce_time = 0;  // the last time the output pin was toggled
unsigned long debounce_delay = 40;    // the debounce time; increase if the output flickers

String UID = "SEQD3SEQAASEQ62SEQA3SEQB8";
byte is_open_door = 0;
byte is_wrong_card = 0;

const String fullname = "Dang Thanh Nhan";
const String mssv = "3119510032";
const byte LedPins [5] = {4, 5, 6, 7, SERVO_PIN}; // Chân led door, led1, led2, led3, led4
byte check_value_lcd_G_S = 0;
byte check_value_lcd_ds3231 = 0;
unsigned long current_display_lcd_G_S_time = 0;
unsigned long current_display_lcd_ds3231_time = 0;
unsigned long display_lcd_lock_door_time = 5000;
unsigned long current_lock_door_time = 0;

/////______Các biến cho việc gửi và nhận______/////
String JsonData;
float  temp = 0;
float  humid = 0;
float  temp1 = 0;
float  humid1 = 0;
byte  state[4];  // tạo một mảng có 4 thiết bị để lưu trạng thái thiết bị
char  cstr[255];
byte  m[255];
byte  buff[255];
byte  on_off_all_led = 0;
int   BEGIN = 0;
int   END = 0;
int   DesLen = 0;
int   sending_time = 500; // Thời gian gửi qua esp8266 (500 ms)
int   saving_time  = 10 * 1000; // Thời gian gửi qua esp8266 để lưu vào cơ sở dữ liệu (10s)

byte Degree[8] = {
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte Skull[8] = {
  B00000,
  B01110,
  B10101,
  B11011,
  B01110,
  B01110,
  B00000,
  B00000
};

byte Lock[8] = {
  B01110,
  B10001,
  B10001,
  B11111,
  B11011,
  B11011,
  B11111,
  B00000
};

byte Check[8] = {
  B00000,
  B00001,
  B00011,
  B10110,
  B11100,
  B01000,
  B00000,
  B00000
};

/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO);
DS3231  ds3231(SDA, SCL);
AsyncTimer timer;

LiquidCrystal_I2C lcd20x4(0x27, 20, 4); // Set the LCD I2C address (display 1)
LiquidCrystal_I2C lcd16x2(0x26, 16, 2);// display 2

//LiquidCrystal_I2C lcd16x2(0x27, 16,2);  // Set the LCD I2C address
DHT dht11(DHTPin1, DHTTYPE1);                // Cấu hình cảm biến DHT11 với chân data và loại cảm biến
DHT dht22(DHTPin2, DHTTYPE2);                // Cấu hình cảm biến DHT22 với chân data và loại cảm biến
DynamicJsonDocument doc(2048);
// Tạo 200 byte vùng nhớ tĩnh đễ chứa rootJson
StaticJsonDocument<200> jsonBuffer;

// trong đề tài này chúng em sử dụng 2kb <=> 2028 byte
// khuyến khích document có kích thước lớn hơn 1kb

/////______Các biến cho việc gửi và nhận______/////

//Prototype
void Start();
void handle(char* payload);
void read_sensor();
void update_and_send_data (byte idtp);
void update_state();
void send_data_to_esp_8266 ();
void receive_data_from_esp_8266();
void setup_lcd();
void display_lcd_title();
void display_lcd_value();
void display_lcd_state_door();
void door_locker();
void is_turn_all();
void writeLcd16x2(byte idWrite);
void read_button();

void setup()
{
  for (int i = 0; i < output; i++) {
    pinMode(LedPins[i], OUTPUT);
    digitalWrite(LedPins[i], 0);
  }
  pinMode(button_door, INPUT);
  Serial.begin(BAUD_RATE);    // Tốc độ giao tiếp với máy tính
  Serial1.begin(BAUD_RATE); // Tốc độ giao tiếp UART giữa 2 thiết bị 115200 bit/s
  timer.setInterval(send_data_to_esp_8266, sending_time);
  timer.setInterval(Start, 0);
  ds3231.begin();
  dht22.begin();
  dht11.begin();
  SPI.begin();
  /* Initialise the RFID reader */
  RC522.init();
  setup_lcd();
}

void loop()
{
  timer.handle();
  door_locker();
}

void read_button() {
  // read the state of the switch into a local variable:

  int reading1 = digitalRead(button_door);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading1 != last_button_door_state) {
    // reset the debouncing timer
    last_button_debounce_time = millis();
  }

  if ((millis() - last_button_debounce_time) > debounce_delay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading1 != button_door_state) {
      button_door_state = reading1;
      if (button_door_state == HIGH) {
        is_open_door = 0;
        is_wrong_card = 0;
        is_turn_all();
        update_and_send_data(1); // doan2/onOff/feedback (pub)
      }
    }
  }
  last_button_door_state = reading1;

  for (int i = 0; i < output; i++) {
    int reading = digitalRead(ButtonPinsLed[i]);
    if (reading != last_button_led_states[i])
      last_button_debounce_time = millis();
    if ((millis() - last_button_debounce_time) > debounce_delay) {
      if (reading != button_led_states[i]) {
        button_led_states[i] = reading;
        if (button_led_states[i] == HIGH) {
          if (ButtonPinsLed[i] != button_led_all)
            digitalWrite(LedPins[i], !digitalRead(LedPins[i]));
          else {
            for (int i = 0; i < 4; i++) {
              digitalWrite(LedPins[i], !on_off_all_led);
            }
            on_off_all_led = !on_off_all_led;
          }
          is_turn_all();
          update_and_send_data(1); // doan2/onOff/feedback (pub)
        }
      }
    }
    last_button_led_states[i] = reading;
  }
}

void Start() {
  receive_data_from_esp_8266();
  read_sensor();
  read_button();
  is_turn_all();
  update_state(1, 1, 0); //id, idc, idtp (doan2/status (pub))
  display_lcd_value();
  display_lcd_state_door();
  digitalWrite(SERVO_PIN, is_open_door);
}


void setup_lcd() {
  lcd20x4.init();
  lcd20x4.backlight();
  lcd20x4.createChar(1, Degree);

  lcd16x2.init();
  lcd16x2.backlight();
  lcd16x2.createChar(1, Skull);
  lcd16x2.createChar(2, Lock);
  lcd16x2.createChar(3, Check);
  //  display_lcd_title();
}

void display_lcd_title() {
  lcd20x4.clear();
  lcd20x4.setCursor(6, 0);
  lcd20x4.print("DO AN 2");
  lcd20x4.setCursor(6, 1);
  lcd20x4.print("Wellcome");
  lcd20x4.setCursor(2, 2);
  for (int i = 0; i < fullname.length(); i++) {
    lcd20x4.print(fullname[i]);
    delay(250);
  }
  lcd20x4.cursor();
  delay(500);
  lcd20x4.noCursor();
  delay(500);
  lcd20x4.cursor();
  delay(500);
  lcd20x4.noCursor();
  lcd20x4.setCursor(5, 3);
  for (int i = 0; i < mssv.length(); i++) {
    lcd20x4.print(mssv[i]);
    delay(250);
  }

  for (int i = 0; i < 6; i++) {
    lcd20x4.cursor();
    delay(500);
    lcd20x4.noCursor();
    delay(500);
  }

  lcd20x4.clear();
}

void display_lcd_value() {
  lcd20x4.setCursor(0, 0);
  if (check_value_lcd_ds3231) {
    lcd20x4.print("Time:  ");
    lcd20x4.print(ds3231.getTimeStr());
    lcd20x4.print ("   ");
  }
  else {
    lcd20x4.print("Date:  ");
    lcd20x4.print(ds3231.getDateStr());
  }
  if (millis() - current_display_lcd_ds3231_time > 10000) {
    check_value_lcd_ds3231 = !check_value_lcd_ds3231;
    current_display_lcd_ds3231_time = millis();
  }

  lcd20x4.setCursor(0, 1);
  lcd20x4.print("NB:");
  lcd20x4.print(temp1);
  lcd20x4.write(1);
  lcd20x4.print("C, ");
  lcd20x4.print(humid1);
  lcd20x4.print("%");

  lcd20x4.setCursor(0, 2);
  lcd20x4.print("Pk:");
  lcd20x4.print(temp);
  lcd20x4.write(1);
  lcd20x4.print("C, ");
  lcd20x4.print(humid);
  lcd20x4.print("%");

  lcd20x4.setCursor(0, 3);
  lcd20x4.print("L:");
  for (int i = 0; i < 4; i++) {
    lcd20x4.print(digitalRead(LedPins[i]));
  }

  lcd20x4.print(", ");
  if (check_value_lcd_G_S) {
    lcd20x4.print("G:");
    lcd20x4.print(temp);
  }
  else {
    lcd20x4.print("S:");
    lcd20x4.print(humid);
  }
  if (millis() - current_display_lcd_G_S_time > 2000) {
    check_value_lcd_G_S = !check_value_lcd_G_S;
    current_display_lcd_G_S_time = millis();
  }
}


void writeLcd16x2(byte idWrite) {
  for (int i = 0; i < 16; i++) {
    if ((i % 3) == 0) {
      lcd16x2.setCursor(i, 1);
      lcd16x2.write(idWrite);
    }
  }
}

void display_lcd_state_door() {
  lcd16x2.setCursor(0, 0);
  if (is_open_door == 0 && is_wrong_card == 1)
  {
    if (millis() - current_lock_door_time > display_lcd_lock_door_time) {
      is_wrong_card = 0;
      current_lock_door_time = millis();
      lcd16x2.clear();
      return;
    }
    lcd16x2.write(1);
    lcd16x2.print("  WRONG CARD  ");
    lcd16x2.write(1);
    lcd16x2.setCursor(0, 1);
    lcd16x2.write(2);
    lcd16x2.write(2);
    lcd16x2.print(" TRY AGAIN! ");
    lcd16x2.write(2);
    lcd16x2.write(2);

    return;
  }
  if (is_open_door == 1) {
    lcd16x2.write(3);
    lcd16x2.print(" Door is open ");
    lcd16x2.write(3);
    writeLcd16x2(3);
    return;
  }
  if (is_open_door == 0) {
    lcd16x2.print(" Door is locked ");
    writeLcd16x2(2);
    return;
  }
}

void update_and_send_data (byte idtp) {
  update_state(1, 1, idtp); //id, iddv, idtp
  send_data_to_esp_8266 ();
}

void is_turn_all() {
  for (int i = 0; i < 4; i++)
    if (!digitalRead(LedPins[i])) {
      on_off_all_led = 0;
      return;
    }
  on_off_all_led = 1;
}

void handle(char* payload) {
  /*
    idtp = 0: doan2/status  (pub)
    idtp = 1: doan2/onOff/feedback  (pub)
    idtp = 2: doan2/onOff/led (sub)
    idtp = 3: doan2/onOff/door (sub)
    idtp = 4: doan2/onOff/led/all (sub)
  */

  deserializeJson(doc, payload);         // Khởi tạo một document có kích thước động là 2kb để phân tích chuỗi Json truyền vào
  JsonObject obj = doc.as<JsonObject>(); // Tạo 1 obj có kiểu dữ liệu là JsonObject với document vừa tạo

  byte onOff = obj["onOff"];                // ID thiết bị
  byte iddv = obj["iddv"];
  byte idtp = obj["idtp"];              // ID topic
  switch (idtp) {
    case 2: // doan2/onOff/led (sub)
      digitalWrite(LedPins[iddv - 1], onOff);
      break;
    case 3: // doan2/onOff/door (sub)
      is_open_door = onOff;
      digitalWrite(LedPins[iddv - 1], onOff);
      current_lock_door_time = millis ();
      break;
    case 4: // doan2/onOff/led/all (sub)
      for (int i = 0; i < 4; i++) {
        digitalWrite(LedPins[i], onOff);
      }
      on_off_all_led = onOff;
      break;
    default: break;
  }
  is_turn_all();
  update_and_send_data(1); // doan2/onOff/feedback (pub)
}

void update_state(byte id, byte idc, byte idtp) {
  char  tmpBuf[20]; // Bộ đệm để lưu trữ giá trị
  jsonBuffer["id"] = id;
  jsonBuffer["idc"] = idc;  // id connect vd: arduino mega2560
  jsonBuffer["idtp"] = idtp;
  jsonBuffer["t"] = dtostrf(temp, 0, 1, tmpBuf);
  jsonBuffer["h"] = dtostrf(humid, 0, 1, tmpBuf);
  jsonBuffer["t1"] = dtostrf(temp1, 0, 1, tmpBuf);
  jsonBuffer["h1"] = dtostrf(humid1, 0, 1, tmpBuf);
  jsonBuffer["l1"] = digitalRead(LedPins[0]);
  jsonBuffer["l2"] = digitalRead(LedPins[1]);
  jsonBuffer["l3"] = digitalRead(LedPins[2]);
  jsonBuffer["l4"] = digitalRead(LedPins[3]);
  jsonBuffer["al"] = on_off_all_led;
  jsonBuffer["d"] = digitalRead(SERVO_PIN);
  JsonObject rootJson = jsonBuffer.as<JsonObject>();
  serializeJson(rootJson, cstr);
}

void read_sensor()
{

  float h = dht11.readHumidity();                            // Đọc độ ẩm
  float t = dht11.readTemperature();                          // Đọc nhiệt độ
  float h1 = dht22.readHumidity();                            // Đọc độ ẩm
  float t1 = dht22.readTemperature();                          // Đọc nhiệt độ
  if (isnan(h) || isnan(t))
    return;
  humid = h;
  temp = t;
  if (isnan(h1) || isnan(t1))
    return;
  humid1 = h1;
  temp1 = t1;


}

void  send_data_to_esp_8266 () {
  yield();                                        // Delay một khoản thời gian rất ngắn
  rsa.Send(cstr);                                 // Gửi chuỗi cstr qua device
}

void receive_data_from_esp_8266() {
  DesLen = 0;
  if (rsa.Receive(buff, &BEGIN, &END, m, &DesLen)) // Nếu có dữ liệu từ device truyền qua
    // Nó sẽ kiểm tra dữ liệu nhận được đúng với dữ liệu mà device truyền qua
    // thì thực hiện pub lên broker
  {
    if (DesLen > 0)
    {
      Serial.print("Nhan: ");
      Serial.println((char*)m);
      handle((char*)m);
    }
  }
  else Serial.println("Nhan error:");
}

void door_locker() {
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();

    String ID = "";
    for (int i = 0; i < 5; i++)
    {
      ID.concat(String(RC522.serNum[i] < 0x10 ? " 0" : "SEQ")); //to print card detail in Hexa Decimal format
      ID.concat(String(RC522.serNum[i], HEX));
    }
    ID.toUpperCase();
    if (ID == UID && is_open_door == 0 ) {
      is_open_door = 1;
      update_and_send_data(1); // doan2/onOff/feedback (pub)
      lcd16x2.clear();
    }
    else if (ID != UID) {
      is_wrong_card = 1;
      current_lock_door_time = millis ();
      lcd16x2.clear();
    }
    digitalWrite(SERVO_PIN, is_open_door);
  }
}
