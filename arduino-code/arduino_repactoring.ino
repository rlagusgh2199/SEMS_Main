#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>

// ============================
// === 핀 설정 및 상수 정의 ===
// ============================

// 모터 제어 핀 (DC 모터 드라이버 IN1/IN2, PWM)
const int motor1_1        = 2;   // 모터1 방향1
const int motor1_2        = 4;   // 모터1 방향2
const int motor1EnablePin = 9;   // 모터1 PWM 제어 핀

// 전류 센서(ACS712) 아날로그 입력
const int currentSensorPin = A0;

// 물리 스위치 (전원 토글용) → INPUT_PULLUP
const int powerSwitchPin   = 8;

// PMS7003(미세먼지) SoftwareSerial(RX=12, TX=13)
SoftwareSerial pmsSerial(12, 13);
unsigned char pmsData[32];
int dataIndex = 0;

// SHT31 온습도 센서
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// =================================================
// === 디바운스용 변수 (물리 스위치)                ===
// =================================================
bool switchState     = HIGH;       // 안정화된(확정된) 스위치 상태 (HIGH=안 눌림)
bool lastStableState = HIGH;       // 한 단계 전 안정화 상태
bool lastReading     = HIGH;       // 바로 전에 읽은(raw) 값
unsigned long lastDebounce = 0;    // 마지막으로 상태가 바뀐 시점 timestamp
const unsigned long debounceDelay = 50; // 50ms 디바운스

// =================================================
// === 모터 / 제어 관련 상태 변수                  ===
// =================================================
bool motorPower       = false;  // 모터 전원 상태 (OFF=false, ON=true)
int  motorMode        = 0;      // 0=자동(AUTO), 1=수동(MANUAL)
int  motorSpeedLevel  = 0;      // 속도 레벨 0~3 (0: 완전 정지)

// 마지막 수동 속도 레벨 저장용 (변경 시에만 출력하기 위해)
int lastManualSpeedLevel = -1;

// =================================================
// === 센서 저장용 변수                            ===
// =================================================
float pm01Value    = 0;
float pm25Value    = 0;
float pm10Value    = 0;
float currentValue = 0;
float temperature  = 0;
float humidity     = 0;

// 2초마다 센서값 전송 타이머
unsigned long lastSensorPrint      = 0;
const unsigned long sensorInterval = 2000; // 2000ms = 2초

// =================================================
// === 함수 프로토타입 선언                        ===
// =================================================
void processSerialCommand();
void checkPowerSwitch();
void readAllSensors();
void readPMSData();
void parsePMS();
void applyAutoMotorSpeed();
void applyManualMotorSpeed();
void runMotorAtLevel(int lvl);
void stopMotor();
void setMotorPWM(int pwmValue);
void printSensorData();

// =================================================
// === setup() =====================================
// =================================================
void setup() {
  // 시리얼 모니터 (디버깅 + JSON 출력)
  Serial.begin(9600);

  // 모터 제어 핀 출력 모드 설정
  pinMode(motor1_1, OUTPUT);
  pinMode(motor1_2, OUTPUT);
  pinMode(motor1EnablePin, OUTPUT);

  // 물리 스위치 입력 풀업
  pinMode(powerSwitchPin, INPUT_PULLUP);

  // 모터 초기 정지
  digitalWrite(motor1EnablePin, LOW);
  digitalWrite(motor1_1, LOW);
  digitalWrite(motor1_2, LOW);

  // PMS7003 시리얼 초기화
  pmsSerial.begin(9600);

  // SHT31 초기화
  if (!sht31.begin(0x44)) {
    Serial.println("SHT31 초기화 실패");
  }

  // 디바운스용 초기 상태 설정
  lastReading     = digitalRead(powerSwitchPin);
  switchState     = lastReading;
  lastStableState = lastReading;
  lastDebounce    = millis();

  // 초기 상태 출력
  Serial.print("초기 스위치 상태: ");
  Serial.println(lastReading == HIGH ? "HIGH(안 눌림)" : "LOW(눌림)");

  // 모터 제어 상태 초기화
  motorPower      = false;
  motorMode       = 0; // AUTO
  motorSpeedLevel = 0;
}

// =================================================
// === loop() ======================================
// =================================================
void loop() {
  // 1) 시리얼로부터 들어오는 제어 명령 처리 (POWER/MODE/SPEED)
  processSerialCommand();

  // 2) 물리 스위치(8번 핀) 디바운스 후 토글 처리
  checkPowerSwitch();

  // 3) 2초마다 센서 읽고 JSON 출력 + 모터 제어
  if (millis() - lastSensorPrint >= sensorInterval) {
    lastSensorPrint = millis();

    // 센서 모두 읽기 (PMS, SHT31, ACS712)
    readAllSensors();

    // JSON 형태로 센서값 & 상태 출력
    printSensorData();

    // 모터 제어 적용
    if (motorPower) {
      // 전원이 켜져 있으면 → 모드에 따라 속도 적용
      if (motorMode == 0) {
        applyAutoMotorSpeed();
      } else {
        applyManualMotorSpeed();
      }
    } else {
      // 전원이 꺼져 있으면 무조건 멈춤
      stopMotor();
    }
  }

  // 4) PMS7003(미세먼지) 데이터가 들어오면 계속 파싱
  readPMSData();
}

// =================================================
// === processSerialCommand() ======================
// === 시리얼로 CONTROL 명령(POWER/MODE/SPEED) 수신 ===
// =================================================
void processSerialCommand() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    // POWER:ON / POWER:OFF
    if (command.startsWith("POWER:")) {
      String val = command.substring(6);
      if (val == "ON") {
        motorPower = true;
        Serial.println("[SerialCmd] 모터 파워: ON");
      } else if (val == "OFF") {
        motorPower = false;
        Serial.println("[SerialCmd] 모터 파워: OFF");
      } else {
        Serial.println("[SerialCmd] 잘못된 POWER 명령 (ON/OFF 사용)");
      }
    }
    // MODE:AUTO / MODE:MANUAL
    else if (command.startsWith("MODE:")) {
      String modeValue = command.substring(5);
      if (modeValue == "AUTO") {
        motorMode = 0;
        Serial.println("[SerialCmd] 모터 모드: AUTOMATIC");
      } else if (modeValue == "MANUAL") {
        motorMode = 1;
        lastManualSpeedLevel = -1; // 수동 모드 진입 시 레벨 리셋
        Serial.println("[SerialCmd] 모터 모드: MANUAL");
      } else {
        Serial.println("[SerialCmd] 잘못된 MODE 명령 (AUTO/MANUAL 사용)");
      }
    }
    // SPEED:0~3
    else if (command.startsWith("SPEED:")) {
      int lvl = command.substring(6).toInt();
      if (lvl >= 0 && lvl <= 3) {
        motorSpeedLevel = lvl;
        Serial.print("[SerialCmd] 모터 속도 레벨 설정: ");
        Serial.println(lvl);
      } else {
        Serial.println("[SerialCmd] 잘못된 SPEED 명령 (0~3 사이)");
      }
    }
    // 그 외는 무시
  }
}

// =================================================
// === checkPowerSwitch() ==========================
// === 8번 핀 스위치 디바운스 후 “LOW가 확정되면” 토글 ===
// =================================================
void checkPowerSwitch() {
  bool reading = digitalRead(powerSwitchPin);

  // (1) raw reading이 지난 루프에 읽은 값과 다르면 debounce 타이머 재설정
  if (reading != lastReading) {
    lastDebounce = millis();
  }

  // (2) debounceDelay 이후에 안정된 값이 계속 유지되었다면 switchState 확정
  if (millis() - lastDebounce > debounceDelay) {
    if (reading != switchState) {
      switchState = reading;

      // (3) 새로운 안정화된 값(switchState)과 이전 안정화 값(lastStableState) 비교
      if (switchState != lastStableState) {
        // 눌림 순간(풀업 HIGH → LOW)만 토글
        if (switchState == LOW) {
          motorPower = !motorPower;
          Serial.print(">>> 물리 스위치 토글! motorPower = ");
          Serial.println(motorPower ? "ON" : "OFF");
        }
        // 안정화 상태 업데이트
        lastStableState = switchState;
      }
    }
  }

  // (4) 다음 루프를 위해 raw reading 저장
  lastReading = reading;

  // -- (선택) 디버깅용 롤 출력 --
  // Serial.print("[Debug] reading: ");
  // Serial.print(reading == HIGH ? "HIGH" : "LOW");
  // Serial.print(" | lastStableState: ");
  // Serial.print(lastStableState == HIGH ? "HIGH" : "LOW");
  // Serial.print(" | switchState: ");
  // Serial.println(switchState == HIGH ? "HIGH" : "LOW");
}

// =================================================
// === readAllSensors() ============================
// ===   PMS7003, SHT31, ACS712 모두 읽어서 전역 변수에 저장 ===
// =================================================
void readAllSensors() {
  // 1) SHT31 온습도 읽기
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  if (!isnan(t) && !isnan(h)) {
    temperature = t;
    humidity    = h;
  }

  // 2) ACS712 전류 센서 읽기
  {
    int raw = analogRead(currentSensorPin);
    // 아날로그(0~1023) → 전압(0~5V)
    float voltage = (raw / 1023.0) * 5.0;
    // 전류(A) 계산 (ACS712 5A 버전: 센터 전압 2.5V, 감도 약 0.066 V/A)
    float amps = (voltage - 2.5) / 0.066;
    currentValue = abs(amps);
  }
}

// =================================================
// === readPMSData() & parsePMS() ==================
// ===   PMS7003 바이너리 스트림 파싱              ===
// =================================================
void readPMSData() {
  while (pmsSerial.available()) {
    unsigned char c = pmsSerial.read();
    if (c == 0x42 && dataIndex == 0) {
      // 패킷 시작 바이트(0x42) 발견
      pmsData[dataIndex++] = c;
    }
    else if (dataIndex > 0) {
      // 중간에 이미 0x42 받은 후 들어오는 바이트일 때
      pmsData[dataIndex++] = c;
      if (dataIndex == 32) {
        // 32바이트 수신 완료 → 파싱
        parsePMS();
        dataIndex = 0;
      }
    }
  }
}

void parsePMS() {
  // PMS7003 파싱 예시 (바이트 오프셋이 센서 모델마다 살짝 다를 수 있으니 필요 시 조정)
  // Byte 10~11: PM1.0, 12~13: PM2.5, 14~15: PM10 (표준 모드 값)
  pm01Value = (pmsData[10] << 8) | pmsData[11];
  pm25Value = (pmsData[12] << 8) | pmsData[13];
  pm10Value = (pmsData[14] << 8) | pmsData[15];
}

// =================================================
// === applyAutoMotorSpeed() =======================
// ===   자동 모드일 때 PM2.5 농도 기준 속도 결정    ===
// =================================================
void applyAutoMotorSpeed() {
  int lvl;
  if (pm25Value < 30)  lvl = 1;
  else if (pm25Value < 50)  lvl = 2;
  else                      lvl = 3;

  // 속도가 변경되었을 때만 센서 로그 출력
  if (lvl != motorSpeedLevel) {
    motorSpeedLevel = lvl;
    Serial.print("[Auto] 속도 레벨: ");
    Serial.println(lvl);
  }
  runMotorAtLevel(lvl);
}

// =================================================
// === applyManualMotorSpeed() =====================
// ===   수동 모드일 때 시리얼로 받은 속도로 구동    ===
// =================================================
void applyManualMotorSpeed() {
  if (motorSpeedLevel != lastManualSpeedLevel) {
    lastManualSpeedLevel = motorSpeedLevel;
    Serial.print("[Manual] 속도 레벨: ");
    Serial.println(motorSpeedLevel);
  }
  runMotorAtLevel(motorSpeedLevel);
}

// =================================================
// === runMotorAtLevel(int lvl) ====================
// ===   속도 레벨(0~3)에 맞춰 모터 드라이브        ===
// =================================================
void runMotorAtLevel(int lvl) {
  if (!motorPower) {
    stopMotor();
    return;
  }
  switch (lvl) {
    case 0:
      stopMotor();
      break;
    case 1:
      setMotorPWM(130); // 속도 레벨1: PWM 100
      break;
    case 2:
      setMotorPWM(200); // 속도 레벨2: PWM 180
      break;
    case 3:
      setMotorPWM(255); // 속도 레벨3: 풀 속도
      break;
    default:
      stopMotor();
      break;
  }
}

// =================================================
// === stopMotor() =================================
// ===   모터 완전 정지                           ===
// =================================================
void stopMotor() {
  analogWrite(motor1EnablePin, 0);
  digitalWrite(motor1_1, LOW);
  digitalWrite(motor1_2, LOW);
}

// =================================================
// === setMotorPWM(int pwmValue) ==================
// ===   PWM 값에 따라 정방향 회전 (예시)          ===
// =================================================
void setMotorPWM(int pwmValue) {
  digitalWrite(motor1_1, HIGH); // 정방향
  digitalWrite(motor1_2, LOW);
  analogWrite(motor1EnablePin, pwmValue);
}

// =================================================
// === printSensorData() ===========================
// ===   JSON 형태로 sensordata/room1 토픽 출력    ===
// =================================================
void printSensorData() {
  Serial.print("{");
  //Serial.print("\"topic\":\"sensordata/room1\","); 
  Serial.print("\"PM1\":");    Serial.print(pm01Value);        Serial.print(",");
  Serial.print("\"PM2_5\":");  Serial.print(pm25Value);        Serial.print(",");
  Serial.print("\"PM10\":");   Serial.print(pm10Value);        Serial.print(",");
  Serial.print("\"CURRENT\":");Serial.print(currentValue, 2); Serial.print(",");
  Serial.print("\"VOLT\":");    Serial.print(5.00, 2);      Serial.print(",");   // <-- 반드시 쉼표
  Serial.print("\"TEMP\":");   Serial.print(temperature, 2);    Serial.print(",");
  Serial.print("\"HUM\":");    Serial.print(humidity, 2);       Serial.print(",");
  Serial.print("\"MODE\":\""); Serial.print(motorMode==0?"AUTO":"MANUAL");Serial.print("\",");
  Serial.print("\"SPEED\":");  Serial.print(motorSpeedLevel);   Serial.print(",");

  Serial.print("\"POWER\":\"");
  Serial.print(motorPower ? "ON" : "OFF");
  Serial.print("\"");
  
  Serial.println("}");
}
