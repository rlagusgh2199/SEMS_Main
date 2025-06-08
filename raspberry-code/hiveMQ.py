# hiveMQ_refactored.py

import serial
import time
import json
import ssl
import paho.mqtt.client as mqtt

# ─────────────────────────────────────────────────────────────────────────
# 1) 시리얼(Arduino ↔ Raspberry Pi) 설정
# ─────────────────────────────────────────────────────────────────────────
SERIAL_PORT = '/dev/ttyACM0'   # Raspberry Pi에 연결된 Arduino 시리얼 포트
SERIAL_BAUD = 9600             # Arduino 스케치 측과 동일한 보드레이트
SERIAL_TIMEOUT = 1             # 1초 대기 후 리턴

try:
    ser = serial.Serial(SERIAL_PORT, SERIAL_BAUD, timeout=SERIAL_TIMEOUT)
    time.sleep(2)  # 시리얼 포트 안정화를 위해 잠시 대기
except Exception as e:
    print(f"[ERROR] 시리얼 포트 열기 실패: {e}")
    exit(1)

# ─────────────────────────────────────────────────────────────────────────
# 2) MQTT(HiveMQ) 설정
# ─────────────────────────────────────────────────────────────────────────
MQTT_BROKER   = "1f89c669ce6f40a8aa6952820a376a78.s1.eu.hivemq.cloud"
MQTT_PORT     = 8883
MQTT_USER     = "404project"
MQTT_PASSWORD = "Project1234"

# 토픽 구조 (room 번호는 필요에 따라 바꿀 수 있습니다)
PUBLISH_TOPIC    = "sensordata/room1"   # 센서 데이터 발행 토픽
CONTROL_ROOT     = "control/room1"      # 제어 명령 토픽 루트
CONTROL_TOPIC_ALL = CONTROL_ROOT + "/#" # 와일드카드로 power/mode/speed 세부 토픽을 모두 구독

# ─────────────────────────────────────────────────────────────────────────
# 3) MQTT 콜백 함수 정의
# ─────────────────────────────────────────────────────────────────────────
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("[MQTT] 연결 성공")
        # 제어 명령 토픽 전체를 구독
        client.subscribe(CONTROL_TOPIC_ALL)
        print(f"[MQTT] 구독 시작 → {CONTROL_TOPIC_ALL}")
    else:
        print(f"[MQTT] 연결 실패, 반환 코드: {rc}")

def on_message(client, userdata, msg):
    """
    제어 토픽으로 들어온 메시지를 수신해서 Arduino로 직렬 전송합니다.
    - control/room1/power   : payload "ON" 또는 "OFF"
    - control/room1/mode    : payload "AUTO" 또는 "MANUAL"
    - control/room1/speed   : payload "1","2","3" (정수 문자열)
    """
    topic = msg.topic
    payload = msg.payload.decode().strip()
    print(f"[MQTT] 메시지 도착 → 토픽: {topic}, 페이로드: {payload}")

    # 토픽이 control/room1/power 인지 확인
    if topic == f"{CONTROL_ROOT}/power":
        if payload.upper() == "ON":
            cmd = "POWER:ON"
        elif payload.upper() == "OFF":
            cmd = "POWER:OFF"
        else:
            print(f"[WARN] 잘못된 power 명령: {payload}")
            return

    # 토픽이 control/room1/mode 인지 확인
    elif topic == f"{CONTROL_ROOT}/mode":
        if payload.upper() == "AUTO":
            cmd = "MODE:AUTO"
        elif payload.upper() == "MANUAL":
            cmd = "MODE:MANUAL"
        else:
            print(f"[WARN] 잘못된 mode 명령: {payload}")
            return

    # 토픽이 control/room1/speed 인지 확인
    elif topic == f"{CONTROL_ROOT}/speed":
        # Arduino 쪽에서는 SPEED:1, SPEED:2, SPEED:3 형식으로 받습니다.
        if payload in ("1", "2", "3"):
            cmd = f"SPEED:{payload}"
        else:
            print(f"[WARN] 잘못된 speed 명령: {payload}")
            return

    else:
        # control/room1/xxx 중에 위 세 가지가 아니면 무시
        print(f"[WARN] 미처리 토픽: {topic}")
        return

    # Arduino로 명령 보내기 (끝에 개행 추가)
    try:
        ser.write((cmd + "\n").encode())
        print(f"[SERIAL] 명령 전송 → {cmd}")
    except Exception as e:
        print(f"[ERROR] Arduino로 명령 전송 실패: {e}")

# ─────────────────────────────────────────────────────────────────────────
# 4) MQTT 클라이언트 초기화 및 연결
# ─────────────────────────────────────────────────────────────────────────
client = mqtt.Client()
client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
client.tls_set()  # SSL/TLS 사용
client.on_connect = on_connect
client.on_message = on_message

try:
    client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
except Exception as e:
    print(f"[ERROR] MQTT 브로커 연결 실패: {e}")
    ser.close()
    exit(1)

# ─────────────────────────────────────────────────────────────────────────
# 5) 메인 루프: Arduino에서 들어오는 JSON 문자열을 HiveMQ로 퍼블리시
# ─────────────────────────────────────────────────────────────────────────
try:
    client.loop_start()
    while True:
        # Arduino에서 한 줄(‘\n’개행) 단위로 읽음
        if ser.in_waiting > 0:
            raw_line = ser.readline().decode('utf-8', errors='ignore').strip()
            # 양 끝에 중괄호가 있는 완전한 JSON이라면 파싱 시도
            if raw_line.startswith("{") and raw_line.endswith("}"):
                try:
                    data = json.loads(raw_line)
                    # data 예시: 
                    #   {"PM1.0":8,"PM2.5":14,"PM10":12,"CURRENT":-0.15,
                    #    "VOLT":5.00,"TEMP":27.82,"HUM":39.43,
                    #    "POWER":"OFF","MODE":"AUTO","SPEED":0}

                    # 추가로 room 번호가 고정이라면 여기서 필드로 넣을 수도 있습니다.
                    # 예시: data["ROOM"] = "room1"

                    # HiveMQ에 JSON 문자열 그대로 퍼블리시
                    client.publish(PUBLISH_TOPIC, json.dumps(data))
                    print(f"[PUBLISH] → {PUBLISH_TOPIC}: {json.dumps(data)}")
                except json.JSONDecodeError:
                    print(f"[ERROR] JSON 디코딩 실패: {raw_line}")
            else:
                # 완전한 JSON 라인이 아닌 경우 필요시 로깅
                # print(f"[DEBUG] 무시된 라인: {raw_line}")
                pass

        time.sleep(0.1)

except KeyboardInterrupt:
    print("\n[EXIT] 키보드 인터럽트, 종료 중...")
finally:
    client.loop_stop()
    client.disconnect()
    ser.close()
    print("[EXIT] Raspberry Pi 스크립트 종료 완료.")