#include "mod_tbc_phase_announcer.h"
#include "DatabaseEnv.h"
#include "Map.h"
#include "SharedDefines.h"
#include "WorldSessionMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "IWorld.h"
#include <vector> // 추가
#include <set>    // 추가

// 전역 변수 정의
uint32 g_currentPhase = 1;
std::string g_phaseDateOne = "";
std::string g_phaseDateTwo = "";
std::string g_phaseDateThree = "";
std::string g_phaseDateFour = "";
std::string g_phaseDateFive = "";

// 정의의 휘장 판매 NPC ID
const std::set<uint32> g_badgeVendorNpcs = {
    18525, // 게라스
    25046, // 하우타
    27667, // 안웨후
    26089, // 케이리
    25950, // 샤아니
    27666  // 온투보
};

// 페이즈별 아이템 ID 목록
const std::vector<uint32> g_phase123Items = {
    32083, 32088, 32084, 32090, 29387, 29384, 32089, 29368, 29268, 29386, 29369, 29367, 29381, 29267, 29269, 29376, 29379, 29374, 29272, 29385, 29370, 29275, 29375, 30776, 30778, 30780, 30779, 30769, 30767, 30766, 30768, 30773, 30772, 30774, 30770, 30761, 30762, 30764, 30763, 32085, 32087, 29273, 29271, 29266, 32086, 29383, 29274, 29382, 29270, 23572, 29373, 29388, 29389, 29390
};

const std::vector<uint32> g_phase4Items = {
    33557, 33538, 33522, 33583, 33513, 33516, 33334, 33518, 33509, 33296, 33514, 33585, 33520, 33207, 33552, 33577, 33974, 33535, 33484, 33507, 33579, 33588, 33539, 33386, 33279, 33532, 33527, 33559, 33325, 33291, 33973, 33331, 35321, 33584, 33531, 33580, 33517, 33505, 33324, 33519, 33970, 33965, 33587, 33524, 33510, 33593, 33504, 33503, 35324, 33810, 33529, 33287, 33537, 33972, 33192, 33540, 33222, 33534, 33530, 33528, 33280, 33304, 33523, 33586, 33832, 35326, 34050, 34163, 34162, 34049, 33566, 33502, 33589, 33333, 33578, 33536, 33508, 33501, 33506, 30183, 33515, 33512, 33582
};

const std::vector<uint32> g_phase5Items = {
    34892, 34888, 34925, 34931, 34936, 34939, 34918, 34896, 34902, 34917, 34889, 34919, 34934, 34943, 34930, 34893, 34951, 34950, 34903, 34942, 34949, 34912, 34938, 34945, 34891, 34926, 34921, 34933, 34944, 34905, 34924, 34901, 34898, 34916, 34890, 34887, 34946, 34910, 34927, 34932, 34935, 34906, 34937, 34941, 34900, 34952, 34928, 34895, 34947, 34929, 34914, 34911, 34940, 34894, 34922, 34904, 34923, 32790, 32814, 32802, 32789, 32813, 32801, 32796, 32821, 32808, 32999, 32998, 32997, 32811, 32787, 32799, 32981, 32980, 32979, 32990, 32989, 32988, 32794, 32819, 32806, 32820, 32795, 32807, 32785, 32797, 32809, 32791, 32803, 32816, 32792, 32804, 32817, 32786, 32810, 32798, 32788, 32812, 32800, 32793, 32818, 32805, 32249, 32229, 32230, 32231, 32227, 32228
};

// 헬퍼 함수 선언
void UpdateVendorItems(uint32 phase);

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

    // 정의의 휘장 판매 목록 업데이트
    UpdateVendorItems(phase);

    WorldDatabase.Execute("UPDATE mod_tbc_phase_status SET phase = {}, phase_date_one = '{}', phase_date_two = '{}', phase_date_three = '{}', phase_date_four = '{}', phase_date_five = '{}'", phase, g_phaseDateOne, g_phaseDateTwo, g_phaseDateThree, g_phaseDateFour, g_phaseDateFive);

    // 모든 플레이어에게 페이즈 변경 안내 메시지 전송
    std::string msg = TBC_PHASE_MESSAGES[phase];
    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, msg);

    LOG_INFO("server.world", "[TBC 페이즈 알리미] 페이즈 {}가 성공적으로 적용되었습니다.", phase);
}

void UpdateVendorItems(uint32 phase)
{
    LOG_INFO("server.world", "[TBC 페이즈 알리미] 정의의 휘장 판매 목록 업데이트 중 (페이즈: {})...", phase);

    // 1. 모든 정의의 휘장 판매 NPC의 기존 아이템 삭제
    for (uint32 npcEntry : g_badgeVendorNpcs)
    {
        WorldDatabase.Execute("DELETE FROM npc_vendor WHERE entry = {}", npcEntry);
    }

    // 2. 현재 페이즈에 맞는 아이템 삽입
    const std::vector<uint32>* itemsToInsert = nullptr;
    std::set<uint32> npcsToUpdate; // NPC별로 아이템을 삽입하기 위한 셋

    switch (phase)
    {
        case 1:
        case 2:
        case 3:
            itemsToInsert = &g_phase123Items;
            npcsToUpdate.insert(18525); // 게라스
            break;
        case 4:
            itemsToInsert = &g_phase4Items;
            npcsToUpdate.insert(18525); // 게라스
            break;
        case 5:
            itemsToInsert = &g_phase5Items;
            npcsToUpdate.insert(25046); // 하우타
            npcsToUpdate.insert(27667); // 안웨후
            npcsToUpdate.insert(26089); // 케이리
            npcsToUpdate.insert(25950); // 샤아니
            npcsToUpdate.insert(27666); // 온투보
            break;
        default:
            LOG_WARN("server.world", "[TBC 페이즈 알리미] 알 수 없는 페이즈({})에 대한 판매 목록 업데이트 요청.", phase);
            return;
    }

    if (itemsToInsert)
    {
        for (uint32 npcEntry : npcsToUpdate)
        {
            for (uint32 itemId : *itemsToInsert)
            {
                // npc_vendor 테이블에 아이템 삽입 (maxcount, incrtime, ExtendedCost는 0으로 설정)
                WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", npcEntry, itemId);
            }
        }
    }

    LOG_INFO("server.world", "[TBC 페이즈 알리미] 정의의 휘장 판매 목록 업데이트 완료 (페이즈: {}).", phase);
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
