/****************************************************
*                                                   *
*    Developer: Tim4ukys                            *
*                                                   *
*    email: tim4ukys.dev@yandex.ru                  *
*    vk: vk.com/tim4ukys                            *
*                                                   *
*    License: GNU GPLv3                             *
*                                                   *
****************************************************/
#include <SAMP.hpp>

namespace base {

    SAMP* SAMP::getRef() {
        static std::unique_ptr<SAMP> singl = std::make_unique<SAMP>();
        return singl.get();
    }

    SAMP::SAMP() {
        initEvents();
    }

    void SAMP::initPointers() {
        const DWORD OFFSETS[SAMP_COUNT_SUPPORT_VERSIONS][4]{
            /*ChatInfo, InputInfo, AddMsg, CmdRect*/
            {0x026E8C8, 0x026E8CC, 0x679F0, 0x69000}, // 037-r3_1
            {0x026EB80, 0x026EB84, 0x68170, 0x69770}, // 037-r5_1
            {0x026E9F8, 0x026E9FC, 0x68170, 0x69770}, // 037-r4_2
            //{0x021A0E4, 0x021A0E8, 0x645A0, 0x65AD0}, // 037-r1
        };
        const auto& offs = OFFSETS[static_cast<size_t>(getSAMPVersion())];

        pChat = *m_samp.getAddr<PVOID*>(offs[0]);
        pInput = *m_samp.getAddr<PVOID*>(offs[1]);
        pAddMsg = m_samp.getAddr<decltype(pAddMsg)>(offs[2]);
        pCmdRect = m_samp.getAddr<decltype(pCmdRect)>(offs[3]);
    }

    void SAMP::initEvents() {
        const uint64_t OFFSETS[SAMP_COUNT_SUPPORT_VERSIONS][1]{
            /*initNetGame*/
            {0x000000B5F0}, // 037-r3_1
            {0x000000B930}, // 037-r5_1
            {0x000000B930}, // 037-r4_2
        };
        const auto& offs = OFFSETS[static_cast<size_t>(getSAMPVersion())];

        m_initNetGame = std::make_unique<typename decltype(m_initNetGame)::element_type>(
            std::move(m_samp.getAddr<init_net_game_t>(offs[0])),
            std::move(std::bind_front(
                [&](init_net_game_t orig, 
                    PVOID pthis, const char* szHost, int nPort, const char* szNick, const char* szPass)->PVOID {
                    const auto netGame = orig(pthis, szHost, nPort, szNick, szPass);
                    if (!pNetGame) {
                        initPointers();
                        pNetGame = netGame;
                        events.onInitNetGame();
                    }
                    return netGame;
                }
            )));
        m_initNetGame->install();
    }

    void SAMP::addCmd(std::string_view cmd, cmdproc_t proc) {
        pCmdRect(pInput, cmd.data(), proc);
    }

    void SAMP::addMessage(DWORD color, const char* fmt, ...) {
        char buff[512]{};

        va_list args;
        va_start(args, fmt);
        vsprintf_s(buff, fmt, args);
        va_end(args);
        buff[144] = '\0';
        pAddMsg(pChat, color, buff);
    }
}
