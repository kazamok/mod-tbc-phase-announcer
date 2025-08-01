# TBC Phase Announcer Module

## 개요

이 모듈은 AzerothCore 기반의 리치 왕의 분노(WotLK) 서버에서 불타는 성전(TBC) 콘텐츠의 단계별 오픈을 관리하고 플레이어에게 현재 및 예정된 페이즈 정보를 효과적으로 안내하기 위해 설계되었습니다.

## 핵심 기능

이 모듈은 AzerothCore 기반의 리치 왕의 분노(WotLK) 서버에서 불타는 성전(TBC) 콘텐츠의 단계별 오픈을 관리하고 플레이어에게 현재 및 예정된 페이즈 정보를 효과적으로 안내하기 위해 설계되었습니다.

*   **전역 콘텐츠 페이즈 제어 (`TBC.ContentPhase`):**
    서버 관리자가 `mod-tbc-phase-announcer.conf` 파일을 통해 TBC 콘텐츠의 전역 페이즈를 수동으로 설정할 수 있습니다. 이 설정은 인스턴스 입장 제한, 특정 NPC/오브젝트/퀘스트의 가시성 및 가용성에 영향을 미칩니다.

*   **인스턴스 입장 제한:**
    현재 설정된 `TBC.ContentPhase`에 맞지 않는 상위 단계의 인스턴스 던전 입장을 자동으로 제한합니다.

*   **플레이어 알림:**
    플레이어 로그인 시 현재 `TBC.ContentPhase` 정보를 안내하며, 페이즈 변경 시 모든 접속 중인 플레이어에게 실시간으로 공지합니다.

*   **예상 오픈 날짜 안내:**
    각 `TBC.ContentPhase`별 예상 오픈 날짜를 설정하여 플레이어에게 다음 콘텐츠 일정을 미리 알릴 수 있습니다.

*   **자동 DB 테이블 생성:**
    모듈 활성화 시 필요한 데이터베이스 테이블(`mod_tbc_phase_status`, `mod_tbc_npc_spawnmask_backup`, `mod_tbc_gameobject_spawnmask_backup`, `mod_tbc_quest_minlevel_backup`)이 자동으로 생성됩니다.

*   **NPC 및 게임오브젝트 가시성 제어:**
    각 `TBC.ContentPhase`에 따라 특정 NPC 및 게임오브젝트(예: 퀘스트 오브젝트, 포탈 등)의 가시성을 동적으로 제어합니다. 아직 활성화되지 않은 페이즈의 NPC/게임오브젝트는 숨겨지고, 해당 페이즈가 되면 다시 나타납니다.

*   **퀘스트 가용성 제어:**
    *   **일반 TBC 퀘스트:** `TBC.ContentPhase`에 따라 특정 퀘스트의 활성화/비활성화를 제어합니다. 아직 활성화되지 않은 페이즈의 퀘스트는 플레이어가 받을 수 없도록 비활성화되고, 해당 페이즈가 되면 다시 활성화됩니다.
    *   **모듈 추가 퀘스트 (ID 11481, 11482):** 이 퀘스트들은 `TBC.ContentPhase`가 5페이즈(태양샘 고원)일 때만 활성화됩니다. 5페이즈 이전에는 비활성화되어 플레이어가 받을 수 없습니다.

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

3.  **예상 오픈 날짜 설정 (선택 사항):
    `mod-tbc-phase-announcer.conf` 파일에서 `TBC.PhaseDateONE`부터 `TBC.PhaseDateFIVE`까지의 값을 `YYYY-MM-DD` 형식으로 설정하여 플레이어에게 안내할 예상 오픈 날짜를 지정할 수 있습니다.

## 기여

이 모듈의 개선에 기여하고 싶으시다면 언제든지 환영합니다. 풀 리퀘스트를 보내주시거나 이슈를 보고해 주세요.

## 라이선스

이 모듈은 GNU Affero General Public License v3 (AGPLv3)에 따라 라이선스됩니다. 자세한 내용은 `LICENSE` 파일을 참조하십시오.

## 개선 사항

이 모듈은 다음과 같은 기능 개선이 이루어졌습니다:

*   **정의의 휘장 판매 NPC 아이템 삭제 로직 개선:** 페이즈 변경 시, 모든 정의의 휘장 판매 NPC의 기존 아이템을 삭제하는 방식이 단일 SQL 쿼리를 사용하여 더욱 견고하고 안정적으로 변경되었습니다. 이는 이전 페이즈의 아이템이 남아있는 문제를 방지합니다.
*   **페이즈별 아이템 판매 로직 개선:** 각 페이즈 시작 시, 해당 NPC의 판매 목록이 완전히 초기화된 후 현재 페이즈에 맞는 아이템만 판매됩니다.
    *   **1~3 페이즈:** 게라스는 1~3 페이즈 아이템만 판매합니다.
    *   **4 페이즈:** 게라스는 1~3 페이즈 아이템과 4 페이즈 아이템을 모두 판매합니다.
    *   **5 페이즈:** 게라스는 아이템을 판매하지 않으며, 5 페이즈 전용 상인들(하우타, 안웨후, 케이리, 샤아니, 온투보)이 각각 자신에게 할당된 고유한 아이템 목록을 판매합니다.
