#include "mod_tbc_phase_announcer.h"
#include "DatabaseEnv.h"
#include "Map.h"
#include "SharedDefines.h"
#include "WorldSessionMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "IWorld.h"
#include "Config.h" // Added for config reading
#include <vector>
#include <set>
#include <string>
#include <sstream>

// 전역 변수 정의
uint32 g_currentPhase = 0;
std::string g_phaseDateOne = "";
std::string g_phaseDateTwo = "";
std::string g_phaseDateThree = "";
std::string g_phaseDateFour = "";
std::string g_phaseDateFive = "";

// 게라스 판매 아이템 목록 (설정 파일에서 로드)
std::vector<std::pair<uint32, uint32>> g_gerasVendorItemsPhase123;
std::vector<std::pair<uint32, uint32>> g_gerasVendorItemsPhase4;

// 페이즈별 NPC 목록 정의 (추가 가능)
const std::vector<uint32> g_phase2Npcs = { 19431, 21978, 19684 };
const std::vector<uint32> g_phase3Npcs = { 21700, 18302, 18422 };

// 오그릴라
const std::vector<uint32> g_phase4Npcs = { 23257,22987,22940,22931,23113,23115,23432,23362,23112,23253,22264,22271,23233,23347,23316,23300,23110,22266,22270,23428,23256,23334,23208,23392,23120,23473,23413,23335,};

// 태양샘 5페이즈
// const std::vector<uint32> g_phase5Npcs_Scourge = { 37541,37539,37538,27059,25003,25031,25001,25002,24999,25030,25027,25028,25047,24966,24960,29341 };
// const std::vector<uint32> g_phase5Npcs_ShatteredSun = { 37523,37527,25170,25175,24994,26253,25236,25059,24938,25115,24965,25108,24967,25069,25061,25088,25037,25043,25112,25163,25032,24972,25057,25133,24813,25035,25034,25144,37509,37512,37211,25164 };
// const std::vector<uint32> g_phase5Npcs_Dawnblade = { 24979,25087,24978,25063,24976,25161 };
// const std::vector<uint32> g_phase5Npcs_Misc = { 37542,37552,37205,25174,25169,25060,25073,26092,26560,26091,25977,26090,25039,25976,25162,25036,24975,25045,25049,25084,6491,30481,25225,37707 };
const std::vector<uint32> g_phase5Npcs_Scourge = { 200065 };
const std::vector<uint32> g_phase5Npcs_ShatteredSun = { 200066 };
const std::vector<uint32> g_phase5Npcs_Dawnblade = { 200067 };
const std::vector<uint32> g_phase5Npcs_Misc = { 200068 };
// const std::vector<uint32> g_phase5Npcs_Shattrath = { 200069 }; 샤트라스NPC만 가시성 조절
const std::vector<uint32> g_phase5Npcs_Shattrath = { 19202,19216,19475,24938,25134,25135,25136,25137,25138,25140,25141,25142,25143,25153,25155,25167,25885 };


// 페이즈별 오브젝트 목록
const std::vector<uint32> g_phase5Objs = { /* 187056 */ 1000010 }; // 쿠웰다나스 섬 차원문 (샤트라스) 페이즈별로 자동 구현되어 삭제,, 1000010은 유지 문맥 상 코드 오류 우회


// 페이즈별 퀘스트 목록
const std::vector<uint32> g_phase4Quests = { 10984 }; // 오그릴라 시작 퀘 (샤트라스)
const std::vector<uint32> g_phase5Quests = { 11499, 11500, 11481, 11482 }; // 태양샘 고원 일일 퀘스트, 그 다음은 시작퀘


// 정의의 휘장 판매 NPC ID
const std::set<uint32> g_badgeVendorNpcs = {
    18525  // 게라스 만 판매아이템 조절하면 됨
    // 25046, // 하우타
    // 27667, // 안웨후
    // 26089, // 케이리
    // 25950, // 샤아니
    // 27666  // 온투보
	// 200070 // TEMP_NPC_NAME
};

// 헬퍼 함수 선언
std::vector<std::pair<uint32, uint32>> ParseVendorItemsConfig(const std::string& configString);
void UpdateVendorItems(uint32 phase);

// 헬퍼 함수 구현
std::vector<std::pair<uint32, uint32>> ParseVendorItemsConfig(const std::string& configString)
{
    std::vector<std::pair<uint32, uint32>> items;
    std::stringstream ss(configString);
    std::string segment;

    while (std::getline(ss, segment, ';'))
    {
        std::stringstream itemSs(segment);
        std::string itemIdStr, extendedCostStr;

        if (std::getline(itemSs, itemIdStr, ',') && std::getline(itemSs, extendedCostStr))
        {
            try
            {
                uint32 itemId = std::stoul(itemIdStr);
                uint32 extendedCost = std::stoul(extendedCostStr);
                items.push_back({itemId, extendedCost});
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("server.world", "[TBC Phase Announcer] Failed to parse vendor item config segment '{}': {}", segment, e.what());
            }
        }
    }
    return items;
}

void CreateNpcSpawnMaskBackupTable()
{
    WorldDatabase.Execute("CREATE TABLE IF NOT EXISTS `mod_tbc_npc_spawnmask_backup` (`guid` INT UNSIGNED NOT NULL PRIMARY KEY, `original_spawnmask` INT UNSIGNED NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;");
    LOG_INFO("server.world", "[TBC 페이즈 알리미] `mod_tbc_npc_spawnmask_backup` 테이블 확인/생성 완료.");
}

void CreateGameObjectSpawnMaskBackupTable()
{
    WorldDatabase.Execute("CREATE TABLE IF NOT EXISTS `mod_tbc_gameobject_spawnmask_backup` (`guid` INT UNSIGNED NOT NULL PRIMARY KEY, `original_spawnmask` INT UNSIGNED NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;");
    LOG_INFO("server.world", "[TBC 페이즈 알리미] `mod_tbc_gameobject_spawnmask_backup` 테이블 확인/생성 완료.");
}

void CreateQuestMinLevelBackupTable()
{
    WorldDatabase.Execute("CREATE TABLE IF NOT EXISTS `mod_tbc_quest_minlevel_backup` (`id` INT UNSIGNED NOT NULL PRIMARY KEY, `original_minlevel` INT UNSIGNED NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;");
    LOG_INFO("server.world", "[TBC 페이즈 알리미] `mod_tbc_quest_minlevel_backup` 테이블 확인/생성 완료.");
}

void UpdateNpcVisibility(uint32 phase)
{
    LOG_INFO("server.world", "[TBC 페이즈 알리미] NPC 가시성 업데이트 중 (현재 페이즈: {})...", phase);

    const std::vector<std::pair<uint32, const std::vector<uint32>&>> phaseNpcMap = {
        {2, g_phase2Npcs},
        {3, g_phase3Npcs},
        {4, g_phase4Npcs},
        {5, g_phase5Npcs_Scourge},
        {5, g_phase5Npcs_ShatteredSun},
        {5, g_phase5Npcs_Dawnblade},
        {5, g_phase5Npcs_Misc},
        {5, g_phase5Npcs_Shattrath}
    };

    for (const auto& pair : phaseNpcMap)
    {
        uint32 requiredPhase = pair.first;
        const std::vector<uint32>& npcList = pair.second;

        for (uint32 npcEntry : npcList)
        {
            QueryResult result = WorldDatabase.Query("SELECT guid, spawnMask FROM creature WHERE id1 = {}", npcEntry);
            if (!result)
            {
                continue;
            }

            do
            {
                Field* fields = result->Fetch();
                uint32 guid = fields[0].Get<uint32>();
                uint32 originalSpawnMask = fields[1].Get<uint32>();

                if (phase < requiredPhase) // NPC를 숨겨야 하는 경우
                {
                    QueryResult backupResult = WorldDatabase.Query("SELECT 1 FROM mod_tbc_npc_spawnmask_backup WHERE guid = {}", guid);
                    if (!backupResult)
                    {
                        WorldDatabase.Execute("INSERT IGNORE INTO mod_tbc_npc_spawnmask_backup (guid, original_spawnmask) VALUES ({}, {})", guid, originalSpawnMask);
                    }
                    if (originalSpawnMask != 0)
                    {
                        WorldDatabase.Execute("UPDATE creature SET spawnMask = 0 WHERE guid = {}", guid);
                    }
                }
                else // NPC를 다시 표시해야 하는 경우
                {
                    QueryResult backupResult = WorldDatabase.Query("SELECT original_spawnmask FROM mod_tbc_npc_spawnmask_backup WHERE guid = {}", guid);
                    if (backupResult)
                    {
                        uint32 backedUpSpawnMask = (*backupResult)[0].Get<uint32>();
                        WorldDatabase.Execute("UPDATE creature SET spawnMask = {} WHERE guid = {}", backedUpSpawnMask, guid);
                        WorldDatabase.Execute("DELETE FROM mod_tbc_npc_spawnmask_backup WHERE guid = {}", guid);
                    }
                }
            } while (result->NextRow());
        }
    }
    LOG_INFO("server.world", "[TBC 페이즈 알리미] NPC 가시성 업데이트 완료.");
}

void UpdateGameObjectVisibility(uint32 phase)
{
    LOG_INFO("server.world", "[TBC 페이즈 알리미] 게임오브젝트 가시성 업데이트 중 (현재 페이즈: {})...", phase);

    const std::vector<std::pair<uint32, const std::vector<uint32>&>> phaseGameObjectMap = {
        {5, g_phase5Objs}
    };

    for (const auto& pair : phaseGameObjectMap)
    {
        uint32 requiredPhase = pair.first;
        const std::vector<uint32>& goList = pair.second;

        for (uint32 goEntry : goList)
        {
            QueryResult result = WorldDatabase.Query("SELECT guid, spawnMask FROM gameobject WHERE id = {}", goEntry);
            if (!result)
            {
                continue;
            }

            do
            {
                Field* fields = result->Fetch();
                uint32 guid = fields[0].Get<uint32>();
                uint32 originalSpawnMask = fields[1].Get<uint32>();

                if (phase < requiredPhase) // 게임오브젝트를 숨겨야 하는 경우
                {
                    QueryResult backupResult = WorldDatabase.Query("SELECT 1 FROM mod_tbc_gameobject_spawnmask_backup WHERE guid = {}", guid);
                    if (!backupResult)
                    {
                        WorldDatabase.Execute("INSERT IGNORE INTO mod_tbc_gameobject_spawnmask_backup (guid, original_spawnmask) VALUES ({}, {})", guid, originalSpawnMask);
                    }
                    if (originalSpawnMask != 0)
                    {
                        WorldDatabase.Execute("UPDATE gameobject SET spawnMask = 0 WHERE guid = {}", guid);
                    }
                }
                else // 게임오브젝트를 다시 표시해야 하는 경우
                {
                    QueryResult backupResult = WorldDatabase.Query("SELECT original_spawnmask FROM mod_tbc_gameobject_spawnmask_backup WHERE guid = {}", guid);
                    if (backupResult)
                    {
                        uint32 backedUpSpawnMask = (*backupResult)[0].Get<uint32>();
                        WorldDatabase.Execute("UPDATE gameobject SET spawnMask = {} WHERE guid = {}", backedUpSpawnMask, guid);
                        WorldDatabase.Execute("DELETE FROM mod_tbc_gameobject_spawnmask_backup WHERE guid = {}", guid);
                    }
                }
            } while (result->NextRow());
        }
    }
    LOG_INFO("server.world", "[TBC 페이즈 알리미] 게임오브젝트 가시성 업데이트 완료.");
}

void UpdateQuestAvailability(uint32 phase)
{
    LOG_INFO("server.world", "[TBC 페이즈 알리미] 퀘스트 가용성 업데이트 중 (현재 페이즈: {})...", phase);

    const uint32 UNAVAILABLE_LEVEL = 99;

    const std::vector<std::pair<uint32, const std::vector<uint32>&>> phaseQuestMap = {
        {4, g_phase4Quests},
        {5, g_phase5Quests} // 에이전트 수정: 5페이즈 퀘스트 목록 추가
    };

    for (const auto& pair : phaseQuestMap)
    {
        uint32 requiredPhase = pair.first;
        const std::vector<uint32>& questList = pair.second;

        for (uint32 questId : questList)
        {
            QueryResult result = WorldDatabase.Query("SELECT MinLevel FROM quest_template WHERE id = {}", questId);
            if (!result)
            {
                continue;
            }

            uint32 originalMinLevel = (*result)[0].Get<uint32>();

            if (phase < requiredPhase) // 퀘스트를 비활성화해야 하는 경우
            {
                QueryResult backupResult = WorldDatabase.Query("SELECT 1 FROM mod_tbc_quest_minlevel_backup WHERE id = {}", questId);
                if (!backupResult)
                {                    WorldDatabase.Execute("INSERT IGNORE INTO mod_tbc_quest_minlevel_backup (id, original_minlevel) VALUES ({}, {})", questId, originalMinLevel);
                }
                if (originalMinLevel != UNAVAILABLE_LEVEL)
                {
                    WorldDatabase.Execute("UPDATE quest_template SET MinLevel = {} WHERE id = {}", UNAVAILABLE_LEVEL, questId);
                }
            }
            else // 퀘스트를 다시 활성화해야 하는 경우
            {
                QueryResult backupResult = WorldDatabase.Query("SELECT original_minlevel FROM mod_tbc_quest_minlevel_backup WHERE id = {}", questId);
                if (backupResult)
                {
                    uint32 backedUpMinLevel = (*backupResult)[0].Get<uint32>();
                    WorldDatabase.Execute("UPDATE quest_template SET MinLevel = {} WHERE id = {}", backedUpMinLevel, questId);
                    WorldDatabase.Execute("DELETE FROM mod_tbc_quest_minlevel_backup WHERE id = {}", questId);
                }
            }
        }
    }
    LOG_INFO("server.world", "[TBC 페이즈 알리미] 퀘스트 가용성 업데이트 완료.");
}

void ApplyPhaseChange(uint32 phase)
{
    // Read Expansion setting from worldserver.conf
    int32 expansion = sConfigMgr->GetOption("Expansion", 2); // Default to 2 (WotLK) if not found

    if (expansion == 0)
    {
        LOG_INFO("server.world", "[TBC 페이즈 알리미] worldserver.conf의 Expansion 설정이 0이므로 TBC 페이즈 알리미 모듈이 비활성화됩니다.");
        return; // Disable module functionality
    }

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
            LOG_WARN("server.world", "[TBC 페이즈 알리미] 초기화 페이즈({})를 적용합니다. 모든 페이즈 컨텐츠를 비활성화합니다.", phase);
            break;
    }

    UpdateVendorItems(phase);
    UpdateNpcVisibility(phase);
    UpdateGameObjectVisibility(phase);
    UpdateQuestAvailability(phase);

    WorldDatabase.Execute("UPDATE mod_tbc_phase_status SET phase = {}, phase_date_one = '{}', phase_date_two = '{}', phase_date_three = '{}', phase_date_four = '{}', phase_date_five = '{}'", phase, g_phaseDateOne, g_phaseDateTwo, g_phaseDateThree, g_phaseDateFour, g_phaseDateFive);

    std::string msg = (phase > 0 && phase < MAX_TBC_PHASE_MESSAGE) ? TBC_PHASE_MESSAGES[phase] : TBC_PHASE_MESSAGES[0];
    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, msg);

    LOG_INFO("server.world", "[TBC 페이즈 알리미] 페이즈 {}가 성공적으로 적용되었습니다.", phase);
}

void UpdateVendorItems(uint32 phase)
{
    LOG_INFO("server.world", "[TBC 페이즈 알리미] 정의의 휘장 판매 목록 업데이트 중 (페이즈: {})...", phase);

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

    std::vector<std::pair<uint32, uint32>> itemsToInsert;
    std::set<uint32> npcsToUpdate;

    npcsToUpdate.insert(18525);

    switch (phase)
    {
        case 1:
        case 2:
        case 3:
            itemsToInsert.insert(itemsToInsert.end(), g_gerasVendorItemsPhase123.begin(), g_gerasVendorItemsPhase123.end());
            break;
        case 4:
        case 5:
            itemsToInsert.insert(itemsToInsert.end(), g_gerasVendorItemsPhase123.begin(), g_gerasVendorItemsPhase123.end());
            itemsToInsert.insert(itemsToInsert.end(), g_gerasVendorItemsPhase4.begin(), g_gerasVendorItemsPhase4.end());
            break;
        case 0: // 페이즈 0 명시적 처리
            break;
        default:
            LOG_WARN("server.world", "[TBC 페이즈 알리미] 초기화 페이즈({})에 대한 아이템 설정입니다. 아이템을 추가하지 않습니다.", phase);
            break;
    }

    if (!itemsToInsert.empty())
    {
        for (uint32 npcEntry : npcsToUpdate)
        {
            for (const auto& itemPair : itemsToInsert)
            {
                WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, {})", npcEntry, itemPair.first, itemPair.second);
            }
        }
    }
    else
    {
        LOG_INFO("server.world", "[TBC 페이즈 알리미] 현재 페이즈({})에 추가할 아이템이 없습니다.", phase);
    }

    LOG_INFO("server.world", "[TBC 페이즈 알리미] 정의의 휘장 판매 목록 업데이트 완료 (페이즈: {}).", phase);
}

bool mod_tbc_phase_announcer_player_script::OnPlayerBeforeTeleport(Player* player, uint32 mapId, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    // GM 계정은 페이즈 제한을 무시
    if (player->IsGameMaster())
        return true;

    uint32 requiredPhase = 0;
    switch(mapId)
    {
        case 548: case 550: requiredPhase = 2; break;
        case 534: case 564: requiredPhase = 3; break;
        case 568: requiredPhase = 4; break;
        case 580: case 585: requiredPhase = 5; break;
        default: return true;
    }

    if (g_currentPhase < requiredPhase)
    {
        std::string expectedOpenDate = "미정";
        switch (requiredPhase)
        {
            case 2: expectedOpenDate = g_phaseDateTwo; break;
            case 3: expectedOpenDate = g_phaseDateThree; break;
            case 4: expectedOpenDate = g_phaseDateFour; break;
            case 5: expectedOpenDate = g_phaseDateFive; break;
        }
        ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormat("해당 인스턴스는 현재 서버에서 비활성화 상태입니다. (필요 페이즈: {}, 오픈예정: {})", requiredPhase, expectedOpenDate));
        return false;
    }
    return true;
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

void mod_tbc_phase_announcer_player_script::OnPlayerUpdateArea(Player* player, uint32 /*oldArea*/, uint32 /*newArea*/)
{
    if (g_currentPhase < 5)
    {
        if (player->GetZoneId() == 4080) // Isle of Quel'Danas Zone ID
        {
            if (player->IsGameMaster())
            {
                ChatHandler(player->GetSession()).PSendSysMessage("현재 {}페이즈에 있지만, GM 상태로 액세스할 수 있습니다.", g_currentPhase);
                return;
            }

            player->GetSession()->SendAreaTriggerMessage("쿠엘다나스 섬은 아직 접근이 불가합니다.");
            player->TeleportTo(530, -1846.856f, 5479.334f, -12.428f, 1.6f); // Teleport to Shattrath
        }
    }
}

void Addmod_tbc_phase_announcerScripts()
{
    new mod_tbc_phase_announcer_player_script();
    new mod_tbc_phase_announcer_world_script();
}
