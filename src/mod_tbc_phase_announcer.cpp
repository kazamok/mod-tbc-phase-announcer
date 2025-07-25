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

const std::vector<uint32> g_phase5ItemsHautaAnwehu = {
    34887, 34888, 34889, 34890, 34891, 34892, 34893, 34894, 34895, 34896, 34898, 34900, 34901, 34902, 34903, 34904, 34905, 34906, 34910, 34911, 34912, 34914, 34916, 34917, 34918, 34919, 34921, 34922, 34923, 34924, 34925, 34926, 34927, 34928, 34929, 34930, 34931, 34932, 34933, 34934, 34935, 34936, 34937, 34938, 34939, 34940, 34941, 34942, 34943, 34944, 34945, 34946, 34947, 34949, 34950, 34951, 34952
};

const std::vector<uint32> g_phase5ItemsKeiri = {
    32785, 32786, 32787, 32788, 32789, 32790, 32791, 32792, 32793, 32794, 32795, 32796, 32797, 32798, 32799, 32800, 32801, 32802, 32803, 32804, 32805, 32806, 32807, 32808, 32809, 32810, 32811, 32812, 32813, 32814, 32816, 32817, 32818, 32819, 32820, 32821, 32979, 32980, 32981, 32988, 32989, 32990, 32997, 32998, 32999, 33811, 33812, 33813, 33876, 33877, 33878, 33879, 33880, 33881, 33882, 33883, 33884, 33885, 33886, 33887, 33888, 33889, 33890, 33891, 33892, 33893, 33894, 33895, 33896, 33897, 33898, 33899, 33900, 33901, 33902, 33903, 33904, 33905, 33906, 33907, 33908, 33909, 33910, 33911, 33912, 33913, 33914, 33915, 33916, 33917
};

const std::vector<uint32> g_phase5ItemsShaaniOntubo = {
    32227, 32228, 32229, 32230, 32231, 32249, 35238, 35239, 35240, 35241, 35242, 35243, 35244, 35245, 35246, 35247, 35248, 35249, 35250, 35251, 35252, 35253, 35254, 35255, 35256, 35257, 35258, 35259, 35260, 35261, 35262, 35263, 35264, 35265, 35266, 35267, 35268, 35269, 35270, 35271, 35322, 35323, 35325, 35766, 35767, 35768, 35769, 37504
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
    std::string npcList = "";
    for (uint32 npcEntry : g_badgeVendorNpcs)
    {
        if (!npcList.empty())
            npcList += ",";
        npcList += std::to_string(npcEntry);
    }
    if (!npcList.empty())
    {
        WorldDatabase.Execute("DELETE FROM npc_vendor WHERE entry IN ({})", npcList);
        LOG_INFO("server.world", "[TBC 페이즈 알리미] 정의의 휘장 판매 NPC들의 기존 아이템 삭제 완료.");
    }

    // 2. 현재 페이즈에 맞는 아이템 및 NPC 목록 구성
    std::vector<uint32> cumulativeItems;
    std::set<uint32> cumulativeNpcs;

    // 현재 페이즈까지의 모든 아이템을 누적하여 추가
    if (phase >= 1)
    {
        cumulativeItems.insert(cumulativeItems.end(), g_phase123Items.begin(), g_phase123Items.end());
        cumulativeNpcs.insert(18525); // 게라스 (1-4페이즈 기본 상인)
    }
    if (phase >= 4)
    {
        cumulativeItems.insert(cumulativeItems.end(), g_phase4Items.begin(), g_phase4Items.end());
    }
    // 5페이즈 상인별 아이템 판매
    if (phase >= 5)
    {
        // 하우타 (25046)
        for (uint32 itemId : g_phase5ItemsHautaAnwehu)
        {
            WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", 25046, itemId);
        }
        // 안웨후 (27667)
        for (uint32 itemId : g_phase5ItemsHautaAnwehu)
        {
            WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", 27667, itemId);
        }
        // 케이리 (26089)
        for (uint32 itemId : g_phase5ItemsKeiri)
        {
            WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", 26089, itemId);
        }
        // 샤아니 (25950)
        for (uint32 itemId : g_phase5ItemsShaaniOntubo)
        {
            WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", 25950, itemId);
        }
        // 온투보 (27666)
        for (uint32 itemId : g_phase5ItemsShaaniOntubo)
        {
            WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", 27666, itemId);
        }
    }
    else if (phase < 5) // 5페이즈 미만일 때 5페이즈 상인들은 아이템을 판매하지 않음
    {
        // 5페이즈 상인들의 판매 목록을 비움 (이미 위에서 전체 삭제했지만 명시적으로)
        // 이 부분은 사실상 필요 없지만, 로직의 명확성을 위해 남겨둠
    }

    // 3. 구성된 아이템 목록을 해당 NPC에 삽입
    if (!cumulativeItems.empty())
    {
        for (uint32 npcEntry : cumulativeNpcs)
        {
            for (uint32 itemId : cumulativeItems)
            {
                WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", npcEntry, itemId);
            }
        }
    }
    else
    {
        LOG_WARN("server.world", "[TBC 페이즈 알리미] 현재 페이즈({})에 삽입할 아이템이 없습니다.", phase);
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
