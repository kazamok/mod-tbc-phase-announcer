#include "mod_tbc_phase_announcer.h"

void mod_tbc_phase_announcer_player_script::OnPlayerLogin(Player* player)
{
    // TBC 콘텐츠 페이즈 안내 메시지 전송 로직
    // 플레이어가 월드에 완전히 추가된 후 메시지를 보냅니다.
    uint32 currentTBCPhase = sConfigMgr->GetOption<uint32>("TBC.ContentPhase", 1);
    std::string message;
    if (currentTBCPhase > 0 && currentTBCPhase < MAX_TBC_PHASE_MESSAGE)
        message = TBC_PHASE_MESSAGES[currentTBCPhase];
    else
        message = TBC_PHASE_MESSAGES[0]; // 알 수 없는 페이즈 메시지

    if( message.empty())
    {
        LOG_INFO("server.world", "{}는 {}단계에 대한 빈 TBC 단계 메시지를 가지고 있습니다.", player->GetName(), currentTBCPhase);
        return;
    }
    else
    {
        LOG_INFO("server.world", "{}는 단계{}에 대한 TBC 단계 메시지를 가지고 있습니다: {}", player->GetName(), currentTBCPhase, message);

        // 화면 상단에 메시지 표시
        player->GetSession()->SendAreaTriggerMessage(message);

        // 채팅 메시지로도 출력
        ChatHandler(player->GetSession()).PSendSysMessage("%s", message.c_str());
    }
}

void Addmod_tbc_phase_announcerScripts()
{
    new mod_tbc_phase_announcer_player_script();
}
