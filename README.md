# 🌿 Smart Environmental Management System

<p align="center">
  <img width="850" src="./assets/SEMS_logo.jpg" alt="SEMS Logo">
</p>

<br>

# 📌 프로젝트 개요

### 🔍 개발 배경 및 필요성

<img src="https://github.com/user-attachments/assets/d1b5a6b0-91d6-4198-a2f6-a9770555fdb6" alt="개요 이미지" style="width:100%; max-width:1000px; display:block; margin: 0 auto;">
  
  
국가에서는 **학교보건법 시행규칙**을 통해 교실의 실내 공기질을 **정기적으로 측정하고, 기준치를 초과하면 즉시 조치할 것**을 명시하고 있습니다.  
특히 초·중·고등학교는 아래 항목을 지속적으로 관리해야 합니다:

- 미세먼지 (PM10, PM2.5)
- 이산화탄소
- 온도 및 습도

> 교육부·환경부의 공기질 관리 매뉴얼에 따르면,
> 실내 PM2.5 농도는 **연평균 35㎍/㎥ 이하**, CO₂ 농도는 **1,000ppm 이하**, 온도는 **18~28도**, 습도는 **30~80%**를 권장합니다.

그러나 현실적으로 많은 학교에서는 실시간 모니터링 및 자동화된 환기 제어 시스템이 부재하며, 기존의 상용 공기청정기는 단순 자동 정화에 그쳐 사용자 개입이 어렵고 에너지 분석도 불가능한 경우가 많습니다.

👉 따라서 **국가 기준을 충족하면서도 실질적인 대응이 가능한, 실시간 데이터 기반의 스마트 환경관리 시스템이 절실히 필요한 상황**입니다.

---

### 🎯 개발 목표

- 실내 환경(PM1.0, PM2.5, PM10, 온도, 습도, 전류) 실시간 수집
- 환기 팬 자동/수동 제어
- 에너지 소비량 측정 및 절약량 환산
- 관리자와 사용자간의 실시간 문의하기 기능
- 관리자/학생 권한 분리 + 실시간 웹 기반 제어 및 모니터링
- 여러 환경(교실)을 웹 브라우저를 통해 손쉽게 관리

---

### 🌱 기대 효과

본 시스템은 단순한 공기 정화 수준을 넘어서, 사용자 참여와 교육적 활용을 동시에 고려한 스마트 환경관리 솔루션입니다.

#### 👩‍🏫 학생 측면
- 실시간으로 온도·습도·미세먼지 데이터를 확인함으로써, **환경 변화에 대한 관심과 감수성을 높일 수 있습니다.**
- 데이터에 따른 **직접적인 행동(예: 창문 열기, 환기 모드 전환 등)**으로 연결되어, **환경 인식 교육 효과**를 기대할 수 있습니다.

#### 🧑‍💻 관리자 측면
- 여러 교실의 환경 데이터를 **웹 UI를 통해 통합 모니터링**하고, 팬 작동 여부나 제어 명령을 **한 번에 원격 조정**할 수 있습니다.
- 센서 기반의 자동 제어 기능과 결합되어, **교실 환경 관리의 효율성과 신뢰도를 동시에 향상**시킬 수 있습니다.

---

<br>

## 🔧 주요 기능

### 🌡️ 센서 데이터 실시간 수집 및 시각화
- PM1.0, PM2.5, PM10, 온도, 습도, 전류 데이터를 2초 간격으로 측정하여 웹에 시각화

### 🌬️ 자동/수동 모드 기반 환기 제어
- 자동 모드: 미세먼지 농도에 따라 팬 속도 자동 조절
- 수동 모드: 관리자가 속도 1~3단 조절 가능

### 🔋 에너지 소비 측정 및 절감 비용 계산
- 전류 센서(ACS712)로 환기 팬의 실시간 소비 전력 측정
- 누적 전력을 기반으로 **원화(KRW)로 절감 비용 환산**
- 사용자에게 **직관적인 비용 절감 효과**를 제공

### 🧑‍🏫 관리자/사용자 권한 분리
- **관리자**는 여러 교실의 상태를 통합적으로 모니터링 및 제어 가능
- **(학생)사용자**는 자신이 속한 공간의 실내 환경을 확인

### 💬 실시간 문의 채팅 기능
- 사용자 ↔ 관리자 간 문의 및 피드백을 위한 채팅 시스템 내장

### 🖥️ 웹 기반 통합 제어 UI (React)
- 로그인, 대시보드, 데이터 분석, 제어, 문의 내역 기능 탑재

<br>

## 🖧 시스템 구성도

<p align="center">
  <img src="./assets/시스템구성도.drawio.png" alt="ERD 다이어그램" style="width:100%; max-width:1000px; display:block; margin: 0 auto;">
</p>

<br>

## 🗂️ 데이터베이스 ERD

<p align="center">
  <img src="./assets/ERD.png" alt="ERD 다이어그램" style="width:100%; max-width:1000px; display:block; margin: 0 auto;">
</p>

<br>

## 🛠️ 개발 환경

- **Front-end**: ![React](https://img.shields.io/badge/React-20232A?style=flat&logo=react&logoColor=61DAFB)  
- **Back-end**: ![Spring Boot](https://img.shields.io/badge/Spring_Boot-6DB33F?style=flat&logo=spring-boot&logoColor=white) ![Flask](https://img.shields.io/badge/Flask-000000?style=flat&logo=flask&logoColor=white)  
- **Database**: ![MySQL](https://img.shields.io/badge/MySQL-4479A1?style=flat&logo=mysql&logoColor=white)  
- **MQTT 통신**: ![MQTT](https://img.shields.io/badge/MQTT-paho--mqtt-blue?style=flat&logo=python&logoColor=white)  
- **MQTT 브로커**: ![HiveMQ](https://img.shields.io/badge/HiveMQ-FFDD00?style=flat&logo=protocols.io&logoColor=black)  
- **IoT 디바이스**: ![Arduino](https://img.shields.io/badge/Arduino-00979D?style=flat&logo=arduino&logoColor=white) ![Raspberry Pi](https://img.shields.io/badge/Raspberry%20Pi-C51A4A?style=flat&logo=raspberry-pi&logoColor=white)   
- **데이터 포맷**: ![JSON](https://img.shields.io/badge/JSON-000000?style=flat&logo=json&logoColor=white)  
- **배포(예정)**: ![AWS EC2](https://img.shields.io/badge/AWS_EC2-FF9900?style=flat&logo=amazon-aws&logoColor=white)  
- **협업 도구**: ![GitHub](https://img.shields.io/badge/GitHub-181717?style=flat&logo=github&logoColor=white) ![Notion](https://img.shields.io/badge/Notion-000000?style=flat&logo=notion&logoColor=white)  
- **디자인**: ![Figma](https://img.shields.io/badge/Figma-F24E1E?style=flat&logo=figma&logoColor=white)

<br>

## 👥 팀원 소개

| 김현호 (팀장) | 임종원 | 나은주 | 한민서 |
|:------------:|:------:|:------:|:------:|
| <img src="./assets/kim.jpg" width="200"> | <img src="./assets/lim.jpg" width="200"> | <img src="./assets/na.jpg" width="200"> | <img src="./assets/han.jpg" width="200"> |
| 하드웨어 / MQTT 통신 / 시스템 설계 | 채팅 기능 / MQTT 파싱 / Git 관리 | 웹 UI / 백엔드 / React 설계 | SPA 구조 / UI 설계 / React |

<br>

## 📁 프로젝트 구조

- 📂 **arduino**  
  └─ 센서 측정 및 팬 제어를 위한 아두이노 코드 

- 📂 **rasberrypi**  
  └─ MQTT 통신을 위한 라즈베리파이용 Python 스크립트

- 📂 **web-server**  
  └─ Spring Boot 및 Flask 기반 웹 서버 코드 

- 📂 **assets**  
  └─ README 및 문서에 사용되는 이미지 및 자료 

- 📂 **project-docs**  
  └─ 발표자료, 보고서 등 프로젝트 관련 문서 저장소

- 📄 **README.md**  
  └─ 프로젝트 전체 소개 및 설명 문서

<br>

## 📊 프로젝트 결과

### 📑 발표 자료 (PPT)

- 🏷️ **프로젝트 계획서**  
  ⤷ [SEMS_프로젝트계획서](./assets/SEMS_프로젝트계획서.pptx)

- 🏷️ **1학기 발표 자료**  
  ⤷ [SEMS_1학기 최종 발표 자료](./assets/404_찾을_수_없음_1학기최종발표.pptx)

---

- 🎥 **시연 영상**  
  ⤷ [스마트 환경관리 시스템_시연 영상](https://youtu.be/1bW3b991c54)

