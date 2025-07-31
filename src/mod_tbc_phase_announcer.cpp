#include "mod_tbc_phase_announcer.h"
#include "DatabaseEnv.h"
#include "Map.h"
#include "SharedDefines.h"
#include "WorldSessionMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "IWorld.h"
#include <vector>
#include <set>

// 전역 변수 정의
uint32 g_currentPhase = 1;
std::string g_phaseDateOne = "";
std::string g_phaseDateTwo = "";
std::string g_phaseDateThree = "";
std::string g_phaseDateFour = "";
std::string g_phaseDateFive = "";

// 페이즈별 NPC 목록 정의
const std::vector<uint32> g_phase2Npcs = { 19431, 21978, 19684 };
const std::vector<uint32> g_phase3Npcs = { 21700, 18302, 18422 };
const std::vector<uint32> g_phase4Npcs = { 23257,22987,22940,22931,23113,23115,23432,23362,23112,23253,22264,22271,23233,23347,23316,23300,23110,22266,22270,23428,23256,23334,23208,23392,23120,23473,23413,23335,};
const std::vector<uint32> g_phase5Npcs_Scourge = { 37541,37539,37538,27059,25003,25031,25001,25002,24999,25030,25027,25028,25047,24966,24960,29341 };
const std::vector<uint32> g_phase5Npcs_ShatteredSun = { 37523,37527,25170,25175,24994,26253,25236,25059,24938,25115,24965,25108,24967,25046,25069,25061,25088,25037,25043,25112,25163,25032,24972,25057,25133,24813,25035,25034,25144,37509,37512,37211,25164 };
const std::vector<uint32> g_phase5Npcs_Dawnblade = { 24979,25087,24978,25063,24976,25161 };
const std::vector<uint32> g_phase5Npcs_Misc = { 27666,27667,37542,37552,37205,25174,25169,25060,25073,26092,26560,26091,25977,26090,25039,26089,25976,25162,25036,24975,25950,25045,25049,25084,6491,30481,25225,37707 };
const std::vector<uint32> g_phase5Npcs_Shattrath = { 19202,19216,19475,24938,25134,25135,25136,25137,25138,25140,25141,25142,25143,25153,25155,25167,25885 };

// 페이즈별 오브젝트 목록
const std::vector<uint32> g_phase5Objs = { 187056 }; // 쿠웰다나스 섬 차원문 (샤트라스)


// 페이즈별 퀘스트 목록
const std::vector<uint32> g_phase4Quests = { 10984 }; // 오그릴라 시작 퀘 (샤트라스)
const std::vector<uint32> g_phase5Quests = { 11481, 11482 }; // cris at the sunwell (샤트라스) // 추가됨


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
        {4, g_phase4Quests}
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

    UpdateVendorItems(phase);
    UpdateNpcVisibility(phase);
    UpdateGameObjectVisibility(phase); // 게임오브젝트 가시성 업데이트 호출
    UpdateQuestAvailability(phase); // 퀘스트 가용성 업데이트 호출

    WorldDatabase.Execute("UPDATE mod_tbc_phase_status SET phase = {}, phase_date_one = '{}', phase_date_two = '{}', phase_date_three = '{}', phase_date_four = '{}', phase_date_five = '{}'", phase, g_phaseDateOne, g_phaseDateTwo, g_phaseDateThree, g_phaseDateFour, g_phaseDateFive);

    std::string msg = TBC_PHASE_MESSAGES[phase];
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

    std::vector<uint32> itemsToInsert;
    std::set<uint32> npcsToUpdate;

    npcsToUpdate.insert(18525);

    switch (phase)
    {
        case 1:
        case 2:
        case 3:
            itemsToInsert.insert(itemsToInsert.end(), g_phase123Items.begin(), g_phase123Items.end());
            break;
        case 4:
            itemsToInsert.insert(itemsToInsert.end(), g_phase123Items.begin(), g_phase123Items.end());
            itemsToInsert.insert(itemsToInsert.end(), g_phase4Items.begin(), g_phase4Items.end());
            break;
        case 5:
            for (uint32 itemId : g_phase5ItemsHautaAnwehu)
            {
                WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", 25046, itemId);
                WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", 27667, itemId);
            }
            for (uint32 itemId : g_phase5ItemsKeiri)
            {
                WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", 26089, itemId);
            }
            for (uint32 itemId : g_phase5ItemsShaaniOntubo)
            {
                WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", 25950, itemId);
                WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", 27666, itemId);
            }
            break;
        default:
            LOG_WARN("server.world", "[TBC 페이즈 알리미] 알 수 없는 페이즈({})에 대한 아이템 설정입니다.", phase);
            break;
    }

    if (phase < 5 && !itemsToInsert.empty())
    {
        for (uint32 npcEntry : npcsToUpdate)
        {
            for (uint32 itemId : itemsToInsert)
            {
                WorldDatabase.Execute("INSERT INTO npc_vendor (entry, item, maxcount, incrtime, ExtendedCost) VALUES ({}, {}, 0, 0, 0)", npcEntry, itemId);
            }
        }
    }
    else if (phase < 5 && itemsToInsert.empty())
    {
        LOG_WARN("server.world", "[TBC 페이즈 알리미] 현재 페이즈({})에 삽입할 아이템이 없습니다.", phase);
    }

    LOG_INFO("server.world", "[TBC 페이즈 알리미] 정의의 휘장 판매 목록 업데이트 완료 (페이즈: {}).", phase);
}

bool mod_tbc_phase_announcer_player_script::OnPlayerBeforeTeleport(Player* player, uint32 mapId, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    uint32 requiredPhase = 0;
    switch(mapId)
    {
        case 548: case 550: requiredPhase = 2; break;
        case 534: case 564: requiredPhase = 3; break;
        case 568: requiredPhase = 4; break;
        case 580: requiredPhase = 5; break;
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

void Addmod_tbc_phase_announcerScripts()
{
    new mod_tbc_phase_announcer_player_script();
    new mod_tbc_phase_announcer_world_script();
}
