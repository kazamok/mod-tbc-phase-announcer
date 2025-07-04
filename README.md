# TBC Phase Announcer Module

## 개요

이 모듈은 AzerothCore 기반의 리치 왕의 분노(WotLK) 서버에서 불타는 성전(TBC) 콘텐츠의 단계별 오픈을 관리하고 플레이어에게 현재 및 예정된 페이즈 정보를 효과적으로 안내하기 위해 설계되었습니다.

## 핵심 기능

*   **수동 페이즈 제어:** 서버 관리자가 `worldserver.conf` 파일을 통해 TBC 콘텐츠 페이즈를 수동으로 설정할 수 있습니다.
*   **인스턴스 입장 제한:** 현재 설정된 페이즈에 맞지 않는 상위 단계의 인스턴스 던전 입장을 자동으로 제한합니다.
*   **플레이어 알림:** 플레이어 로그인 시 현재 페이즈 정보를 안내하며, 페이즈 변경 시 모든 접속 중인 플레이어에게 실시간으로 공지합니다.
*   **예상 오픈 날짜 안내:** 각 페이즈별 예상 오픈 날짜를 설정하여 플레이어에게 다음 콘텐츠 일정을 미리 알릴 수 있습니다.
*   **자동 DB 테이블 생성:** 모듈 활성화 시 필요한 데이터베이스 테이블(`mod_tbc_phase_status`)이 자동으로 생성됩니다.

## TBC 콘텐츠 페이즈 정보

이 모듈은 불타는 성전 확장팩의 콘텐츠를 다음과 같은 5단계 페이즈로 구분하여 관리합니다.

*   **페이즈 1:** 카라잔, 그룰의 둥지, 마그테리돈의 둥지
*   **페이즈 2:** 불뱀 제단, 폭풍우 요새
*   **페이즈 3:** 하이잘 정상, 검은 사원
*   **페이즈 4:** 줄아만
*   **페이즈 5:** 태양샘 고원

## 설치 방법

1.  **모듈 파일 배치:**
    이 모듈의 모든 파일을 `your_azerothcore_root/modules/mod-tbc-phase-announcer/` 경로에 배치합니다.

2.  **CMake 설정:**
    `your_azerothcore_root/CMakeLists.txt` 파일을 열고 `add_subdirectory(modules)` 라인 아래에 다음 라인을 추가하여 모듈을 활성화합니다.
    ```cmake
    add_subdirectory(modules/mod-tbc-phase-announcer)
    ```
    **참고:** 최신 AzerothCore 버전에서는 `modules/CMakeLists.txt`가 자동으로 하위 모듈을 감지하므로, 이 단계가 필요 없을 수도 있습니다. (현재 이 모듈은 자체 `CMakeLists.txt`를 가지고 있습니다.)

3.  **CMake 재실행 및 프로젝트 빌드:**
    CMake를 다시 실행하여 프로젝트를 재구성하고 빌드합니다. (예: `cmake . -B build`, `cmake --build build`)

4.  **데이터베이스 업데이트:**
    월드 서버를 처음 실행할 때 `mod_tbc_phase_status` 테이블이 자동으로 생성됩니다. 만약 테이블이 생성되지 않거나 오류가 발생하면, `acore-db-updater` 도구를 수동으로 실행하여 데이터베이스를 업데이트할 수 있습니다.
    ```bash
    cd your_azerothcore_root/bin
    ./acore-db-updater # Linux/macOS
    acore-db-updater.exe # Windows
    ```

5.  **설정 파일 복사:**
    `your_azerothcore_root/modules/mod-tbc-phase-announcer/conf/mod-tbc-phase-announcer.conf.dist` 파일을 `your_azerothcore_root/etc/` (Linux/macOS) 또는 `your_azerothcore_root/configs/` (Windows) 디렉토리로 복사하고 `mod-tbc-phase-announcer.conf`로 이름을 변경합니다.

## 사용법

1.  **현재 페이즈 설정:**
    `your_azerothcore_root/etc/mod-tbc-phase-announcer.conf` 파일을 열고 `TBC.ContentPhase` 값을 원하는 페이즈 번호(1-5)로 변경합니다.
    ```
    TBC.ContentPhase = 1
    ```

2.  **설정 적용:**
    서버를 재시작하거나, 인게임에서 `.reload config` 명령어를 사용하여 변경된 설정을 적용합니다.

3.  **예상 오픈 날짜 설정 (선택 사항):**
    `mod-tbc-phase-announcer.conf` 파일에서 `TBC.PhaseDateONE`부터 `TBC.PhaseDateFIVE`까지의 값을 `YYYY-MM-DD` 형식으로 설정하여 플레이어에게 안내할 예상 오픈 날짜를 지정할 수 있습니다.

## 기여

이 모듈의 개선에 기여하고 싶으시다면 언제든지 환영합니다. 풀 리퀘스트를 보내주시거나 이슈를 보고해 주세요.

## 라이선스

이 모듈은 GNU Affero General Public License v3 (AGPLv3)에 따라 라이선스됩니다. 자세한 내용은 `LICENSE` 파일을 참조하십시오.