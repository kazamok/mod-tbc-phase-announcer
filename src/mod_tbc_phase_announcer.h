#ifndef MOD_TBC_PHASE_ANNOUNCER_H
#define MOD_TBC_PHASE_ANNOUNCER_H

#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "World.h"
#include "WorldScript.h"

// 전역 변수로 현재 페이즈를 저장
extern uint32 g_currentPhase;

// TBC 콘텐츠 페이즈별 예상 오픈 날짜
extern std::string g_phaseDateOne;
extern std::string g_phaseDateTwo;
extern std::string g_phaseDateThree;
extern std::string g_phaseDateFour;
extern std::string g_phaseDateFive;

// TBC 콘텐츠 페이즈 안내 메시지
static const char* TBC_PHASE_MESSAGES[] =
{
    "|CFFFFFFFF[TBC 페이즈 알리미]|r 알 수 없는 페이즈입니다.", // 0번 인덱스는 사용하지 않음
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
};

class mod_tbc_phase_announcer_world_script : public WorldScript
{
public:
    mod_tbc_phase_announcer_world_script() : WorldScript("mod_tbc_phase_announcer_world_script") { }

    void OnAfterConfigLoad(bool reload) override;
};

void ApplyPhaseChange(uint32 phase);

#endif // MOD_TBC_PHASE_ANNOUNCER_H
