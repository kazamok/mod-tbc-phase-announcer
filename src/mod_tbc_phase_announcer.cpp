
/*
mod_tbc_phase_announcer.cpp */

#include "mod_tbc_phase_announcer.h"
#include "DatabaseEnv.h"
#include "Map.h"
#include "SharedDefines.h"
#include "WorldSessionMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "IWorld.h"

// 전역 변수 정의
uint32 g_currentPhase = 1;
std::string g_phaseDateOne = "";
std::string g_phaseDateTwo = "";
std::string g_phaseDateThree = "";
std::string g_phaseDateFour = "";
std::string g_phaseDateFive = "";

void ApplyPhaseChange(uint32 phase)
{
    // 이 함수가 페이즈 변경의 핵심 뼈대입니다.
    g_currentPhase = phase;

    LOG_INFO("server.world", "[TBC 페이즈 알리미] 페이즈 {} 적용 중...", phase);

    switch (phase)
    {
        case 1:
            LOG_INFO("server.world", "[TBC 페이즈 알리미] 1페이즈 컨텐츠 활성화.");
            break;
        case 2:
            LOG_INFO("server.world", "[TBC 페이즈 알리미] 2페이즈 컨텐츠 활성화.");
            break;
        case 3:
            LOG_INFO("server.world", "[TBC 페이즈 알리미] 3페이즈 컨텐츠 활성화.");
            break;
        case 4:
            LOG_INFO("server.world", "[TBC 페이즈 알리미] 4페이즈 컨텐츠 활성화.");
            break;
        case 5:
            LOG_INFO("server.world", "[TBC 페이즈 알리미] 5페이즈 컨텐츠 활성화.");
            break;
        default:
            LOG_WARN("server.world", "[TBC 페이즈 알리미] 알 수 없는 페이즈를 적용하려 했습니다: {}.", phase);
            return;
    }

    WorldDatabase.Execute("UPDATE mod_tbc_phase_status SET phase = {}, phase_date_one = '{}', phase_date_two = '{}', phase_date_three = '{}', phase_date_four = '{}', phase_date_five = '{}'", phase, g_phaseDateOne, g_phaseDateTwo, g_phaseDateThree, g_phaseDateFour, g_phaseDateFive);

    // 모든 플레이어에게 페이즈 변경 안내 메시지 전송
    std::string msg = TBC_PHASE_MESSAGES[phase];
    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, msg);

    LOG_INFO("server.world", "[TBC 페이즈 알리미] 페이즈 {}가 성공적으로 적용되었습니다.", phase);
}

void mod_tbc_phase_announcer_world_script::OnAfterConfigLoad(bool reload)
{
    

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
}

bool mod_tbc_phase_announcer_player_script::OnPlayerBeforeTeleport(Player* player, uint32 mapId, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    // 각 페이즈별 인스턴스 Map ID
    // 페이즈 1: 카라잔(532), 마그테리돈의 둥지(544), 그룰의 둥지(565)
    // 페이즈 2: 불뱀 제단(548), 폭풍우 요새(550)
    // 페이즈 3: 하이잘 정상(534), 검은 사원(564)
    // 페이즈 4: 줄아만(568)
    // 페이즈 5: 태양샘 고원(580)

    uint32 requiredPhase = 0;
    switch(mapId)
    {
        case 548: // 불뱀 제단
        case 550: // 폭풍우 요새
            requiredPhase = 2;
            break;
        case 534: // 하이잘 정상
        case 564: // 검은 사원
            requiredPhase = 3;
            break;
        case 568: // 줄아만
            requiredPhase = 4;
            break;
        case 580: // 태양샘 고원
            requiredPhase = 5;
            break;
        default:
            return true; // 1페이즈 또는 기타 인스턴스는 항상 허용
    }

    if (g_currentPhase < requiredPhase)
    {
        std::string expectedOpenDate = "미정";
        switch (requiredPhase)
        {
            case 1: expectedOpenDate = g_phaseDateOne; break;
            case 2: expectedOpenDate = g_phaseDateTwo; break;
            case 3: expectedOpenDate = g_phaseDateThree; break;
            case 4: expectedOpenDate = g_phaseDateFour; break;
            case 5: expectedOpenDate = g_phaseDateFive; break;
            default: break;
        }
        std::string formattedMessage = Acore::StringFormat("해당 인스턴스는 현재 서버에서 비활성화 상태입니다. (필요 페이즈: {}, 오픈예정: {})", requiredPhase, expectedOpenDate);
        ChatHandler(player->GetSession()).SendSysMessage(formattedMessage);
        return false; // 입장 거부
    }
    return true; // 입장 허용
}

void mod_tbc_phase_announcer_player_script::OnPlayerLogin(Player* player)
{
    std::string message;
    if (g_currentPhase > 0 && g_currentPhase < MAX_TBC_PHASE_MESSAGE)
        message = TBC_PHASE_MESSAGES[g_currentPhase];
    else
        message = TBC_PHASE_MESSAGES[0];

    if(message.empty())
    {
        return;
    }

    player->GetSession()->SendAreaTriggerMessage(message);
    ChatHandler(player->GetSession()).PSendSysMessage(message);
}

void Addmod_tbc_phase_announcerScripts()
{
    new mod_tbc_phase_announcer_player_script();
    new mod_tbc_phase_announcer_world_script();
}
