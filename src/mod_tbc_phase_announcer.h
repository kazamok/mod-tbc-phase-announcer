
/*
mod_tbc_phase_announcer.h */

#ifndef MOD_TBC_PHASE_ANNOUNCER_H
#define MOD_TBC_PHASE_ANNOUNCER_H

#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "World.h"
#include "WorldScript.h"
#include <vector>
#include <set>
#include "PlayerScript.h"

// 전역 변수로 현재 페이즈를 저장
extern uint32 g_currentPhase;

// TBC 콘텐츠 페이즈별 예상 오픈 날짜
extern std::string g_phaseDateOne;
extern std::string g_phaseDateTwo;
extern std::string g_phaseDateThree;
extern std::string g_phaseDateFour;
extern std::string g_phaseDateFive;

// 페이즈별 NPC 목록
extern const std::vector<uint32> g_phase2Npcs;
extern const std::vector<uint32> g_phase3Npcs;
extern const std::vector<uint32> g_phase4Npcs;
extern const std::vector<uint32> g_phase5Npcs_Scourge;
extern const std::vector<uint32> g_phase5Npcs_ShatteredSun;
extern const std::vector<uint32> g_phase5Npcs_Dawnblade;
extern const std::vector<uint32> g_phase5Npcs_Misc;
extern const std::vector<uint32> g_phase5Npcs_Shattrath;

// 페이즈별 오브젝트 목록
extern const std::vector<uint32> g_phase5Objs;

// 페이즈별 퀘스트 목록
extern const std::vector<uint32> g_phase4Quests;
extern const std::vector<uint32> g_phase5Quests;

// 게라스 판매 아이템 목록 (설정 파일에서 로드)
extern std::vector<std::pair<uint32, uint32>> g_gerasVendorItemsPhase123;
extern std::vector<std::pair<uint32, uint32>> g_gerasVendorItemsPhase4;


void ApplyPhaseChange(uint32 phase);
void UpdateNpcVisibility(uint32 phase);
void CreateNpcSpawnMaskBackupTable();
void UpdateGameObjectVisibility(uint32 phase);
void CreateGameObjectSpawnMaskBackupTable();
void UpdateQuestAvailability(uint32 phase);
void CreateQuestMinLevelBackupTable();
std::vector<std::pair<uint32, uint32>> ParseVendorItemsConfig(const std::string& configString);


// TBC 콘텐츠 페이즈 안내 메시지
static const char* TBC_PHASE_MESSAGES[] =
{
    "|CFFFFFFFF[TBC 페이즈 알리미]|r 초기화 0페이즈입니다.", // 0번 인덱스는 사용하지 않음
    "|CFFFFFFFF[TBC 페이즈 알리미]|r 이 서버는 현재 1페이즈 (카라잔, 그룰의 둥지, 마그테리돈의 둥지) 진행중입니다.",
    "|CFFFFFFFF[TBC 페이즈 알리미]|r 이 서버는 현재 2페이즈 (불뱀 제단, 폭풍우 요새) 진행중입니다.",
    "|CFFFFFFFF[TBC 페이즈 알리미]|r 이 서버는 현재 3페이즈 (하이잘 정상, 검은 사원) 진행중입니다.",
    "|CFFFFFFFF[TBC 페이즈 알리미]|r 이 서버는 현재 4페이즈 (줄아만) 진행중입니다.",
    "|CFFFFFFFF[TBC 페이즈 알리미]|r 이 서버는 현재 5페이즈 (태양샘 고원) 진행중입니다."
};
static const uint32 MAX_TBC_PHASE_MESSAGE = sizeof(TBC_PHASE_MESSAGES) / sizeof(TBC_PHASE_MESSAGES[0]);

class mod_tbc_phase_announcer_player_script : public PlayerScript
{
public:
    mod_tbc_phase_announcer_player_script() : PlayerScript("mod_tbc_phase_announcer_player_script") { }

    void OnPlayerLogin(Player* player) override;
    bool OnPlayerBeforeTeleport(Player* player, uint32 mapid, float x, float y, float z, float orientation, uint32 options, Unit* target) override;
    void OnPlayerUpdateArea(Player* player, uint32 oldArea, uint32 newArea) override;
};

class mod_tbc_phase_announcer_world_script : public WorldScript
{
public:
    mod_tbc_phase_announcer_world_script() : WorldScript("mod_tbc_phase_announcer_world_script") { }

    void OnStartup() override
    {
        CreateNpcSpawnMaskBackupTable();
        CreateGameObjectSpawnMaskBackupTable();
        CreateQuestMinLevelBackupTable();
        UpdateNpcVisibility(g_currentPhase);
        UpdateGameObjectVisibility(g_currentPhase);
        UpdateQuestAvailability(g_currentPhase);
    }

    void OnAfterConfigLoad(bool reload) override
    {
        // 설정 파일에서 판매 아이템 목록 읽기
        std::string itemsPhase123 = sConfigMgr->GetOption<std::string>("Geras.Vendor.Items.Phase123", "");
        g_gerasVendorItemsPhase123 = ParseVendorItemsConfig(itemsPhase123);
        LOG_INFO("server.world", "[TBC 페이즈 알리미] 1-3페이즈 아이템 {}개 로드.", g_gerasVendorItemsPhase123.size());

        std::string itemsPhase4 = sConfigMgr->GetOption<std::string>("Geras.Vendor.Items.Phase4", "");
        g_gerasVendorItemsPhase4 = ParseVendorItemsConfig(itemsPhase4);
        LOG_INFO("server.world", "[TBC 페이즈 알리미] 4페이즈 아이템 {}개 로드.", g_gerasVendorItemsPhase4.size());

        // 설정 파일에서 날짜 정보 읽기
        g_phaseDateOne = sConfigMgr->GetOption<std::string>("TBC.PhaseDateONE", "미정");
        g_phaseDateTwo = sConfigMgr->GetOption<std::string>("TBC.PhaseDateTWO", "미정");
        g_phaseDateThree = sConfigMgr->GetOption<std::string>("TBC.PhaseDateTHREE", "미정");
        g_phaseDateFour = sConfigMgr->GetOption<std::string>("TBC.PhaseDateFOUR", "미정");
        g_phaseDateFive = sConfigMgr->GetOption<std::string>("TBC.PhaseDateFIVE", "미정");

        uint32 configPhase = sConfigMgr->GetOption<uint32>("TBC.ContentPhase", 1);

        QueryResult result = WorldDatabase.Query("SELECT phase FROM mod_tbc_phase_status");
        if (!result)
        {
            g_currentPhase = 1; // DB 오류 시 기본값으로 설정
            LOG_ERROR("server.world", "[TBC 페이즈 알리미] `mod_tbc_phase_status` 테이블에서 페이즈를 읽을 수 없습니다. 기본 페이즈 1을 사용합니다.");
            return;
        }

        uint32 dbPhase = (*result)[0].Get<uint32>();

        if (configPhase != dbPhase)
        {
            LOG_INFO("server.world", "[TBC 페이즈 알리미] 설정 페이즈({})가 데이터베이스 페이즈({})와 다릅니다. 변경을 적용합니다...", configPhase, dbPhase);
            ApplyPhaseChange(configPhase);
        }
        else
        {
            g_currentPhase = dbPhase;
            if (reload)
            {
                LOG_INFO("server.world", "[TBC 페이즈 알리미] 설정이 다시 로드되었지만, 페이즈는 {}로 변경되지 않았습니다.", std::to_string(g_currentPhase));
            }
            else
            {
                LOG_INFO("server.world", "[TBC 페이즈 알리미] 페이즈 {}로 초기화되었습니다.", std::to_string(g_currentPhase));
            }
        }
    };
};

#endif // MOD_TBC_PHASE_ANNOUNCER_H
