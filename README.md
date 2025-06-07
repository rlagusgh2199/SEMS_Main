# 🌿 Smart Environmental Management System

<p align="center">
  <img width="850" src="./assets/SEMS_logo.jpg" alt="SEMS Logo">
</p>

## 📁 프로젝트 폴더 구조

| 폴더명             | 설명 |
|-------------------|------|
| [arduino-code/](./arduino-code)     | 아두이노 센서 제어 및 모터 제어 코드 (.ino) |
| [raspberry-code/](./raspberry-code) | 라즈베리파이 MQTT 통신 및 시리얼 처리 파이썬 코드 |
| [web-server/](https://github.com/haha096/SEMS_Project)         | 🌐 웹 서버(Spring Boot + React) 서브모듈 |
| [project-docs/](https://github.com/yimjongwon/project_7)       | 📁 문서 저장소 (보고서, 발표자료 등) 서브모듈 |

# 📌 프로젝트 개요

### 🔍 개발 배경 및 필요성

<img src="https://github.com/user-attachments/assets/d1b5a6b0-91d6-4198-a2f6-a9770555fdb6" alt="개요 이미지" style="width:100%; max-width:1000px; display:block; margin: 0 auto;">
최근 미세먼지와 실내 공기질 문제가 심화됨에 따라, 가정·학교·사무실 등 밀폐 공간에서의 실내 공기 상태 관리가 중요해졌습니다. 기존 상용 공기청정기들은 단순 자동 정화에만 머물러 있고, 실시간 데이터 확인이나 에너지 분석 등의 고급 기능은 부족한 상황입니다.

---

### 🎯 개발 목표

- 실내 환경(PM1.0, PM2.5, PM10, 온도, 습도, 전류) 실시간 수집
- 환기 팬 자동/수동 제어 (AI 기반 + 사용자 제어 병행)
- 에너지 소비량 측정 및 시각화
- 관리자/사용자 권한 분리 + 실시간 웹 기반 제어 및 모니터링

---

### 🌱 기대 효과

- 사용자 친화적인 웹 UI를 통해 능동적인 환경 관리 가능
- 실시간 데이터 기반 제어로 에너지 절감 유도
- 교육 현장에서 데이터 리터러시 및 환경 인식 향상 효과
