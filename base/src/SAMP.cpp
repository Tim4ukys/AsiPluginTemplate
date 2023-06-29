/*
    AsiPluginTemplate source file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#include <SAMP.hpp>
#include "patch.hpp"
#include "base_safe_check.hpp"

namespace base {

    SAMP* SAMP::getRef() {
        static std::unique_ptr<SAMP> singl = std::make_unique<SAMP>();
        return singl.get();
    }

    SAMP::SAMP() {
        initEvents();
    }

    void SAMP::initPointers() {
        const DWORD OFFSETS[6][4]{
            /*ChatInfo, InputInfo, AddMsg, CmdRect*/
            {0x021A0E4, 0x021A0E8, 0x645A0, 0x65AD0}, // 037-r1
            {0x021A0EC, 0x021A0F0, 0x64670, 0x65BA0}, // 037-r2

            {0x026E8C8, 0x026E8CC, 0x679F0, 0x69000}, // 037-r3_1
            {0x026E9F8, 0x026E9FC, 0x68130, 0x69730}, // 037-r4
            {0x026E9F8, 0x026E9FC, 0x68170, 0x69770}, // 037-r4_2
            {0x026EB80, 0x026EB84, 0x68170, 0x69770}, // 037-r5_1
        };
        BASE_CHECK_ARRAYSIZE(OFFSETS, SAMP::SAMP_COUNT_SUPPORT_VERSIONS);
        const auto& offs = OFFSETS[static_cast<size_t>(getSAMPVersion())];

        pChat = *m_samp.getAddr<PVOID*>(offs[0]);
        pInput = *m_samp.getAddr<PVOID*>(offs[1]);
        pAddMsg = m_samp.getAddr<decltype(pAddMsg)>(offs[2]);
        pCmdRect = m_samp.getAddr<decltype(pCmdRect)>(offs[3]);
    }

    void SAMP::initEvents() {
        const uint64_t OFFSETS[6][1]{
            /*check isNetGameInit | shell code*/
            {0x2565EF}, // 037-r1
            {0x2CCC84}, // 037-r2

            /*initNetGame*/
            {0x000000B5F0}, // 037-r3_1
            {0x000000B920}, // 037-r4
            {0x000000B930}, // 037-r4_2
            {0x000000B930}, // 037-r5_1
        };
        BASE_CHECK_ARRAYSIZE(OFFSETS, SAMP::SAMP_COUNT_SUPPORT_VERSIONS);
        const auto  vers = getSAMPVersion();
        const auto& offs = OFFSETS[static_cast<size_t>(vers)];
        
        if (vers != SAMP_VERSION::v037_r2 && vers != SAMP_VERSION::v037_r1) {
            m_initNetGame = std::make_unique<typename decltype(m_initNetGame)::element_type>(
                std::move(m_samp.getAddr<init_net_game_t>(offs[0])),
                std::move([&](init_net_game_t orig,
                              PVOID pthis, const char* szHost, int nPort, const char* szNick, const char* szPass) -> PVOID {
                    const auto netGame = orig(pthis, szHost, nPort, szNick, szPass);
                    if (!pNetGame) {
                        initPointers();
                        pNetGame = netGame;
                        events.onInitNetGame();
                    }
                    return netGame;
                }));
            m_initNetGame->install();
        } else {
            m_initNetGameCodeShell = std::make_unique<Xbyak::CodeGenerator>();
            using namespace Xbyak::util;
            m_initNetGameCodeShell->mov(ecx, DWORD(this));
            m_initNetGameCodeShell->push(edx);
            m_initNetGameCodeShell->mov(edx, eax);
            PVOID(__fastcall * initNetGameShell)(SAMP*, PVOID) = [](SAMP* pthis, PVOID netGame)->PVOID {
                if (!pthis->pNetGame) {
                    pthis->initPointers();
                    pthis->pNetGame = netGame;
                    pthis->events.onInitNetGame();
                }
                return netGame;
            };
            m_initNetGameCodeShell->call(initNetGameShell);
            m_initNetGameCodeShell->pop(edx);
            patch::setShellCodeThroughJump(m_samp.getAddr<uintptr_t>(offs[0]), *m_initNetGameCodeShell, 10u, TRUE);
        }
    }

    void SAMP::addCmd(std::string_view cmd, cmdproc_t proc) {
        pCmdRect(pInput, cmd.data(), proc);
    }

    class cmdProcWrapper {
        std::function<void(const char*)> m_callback;
    public:
        cmdProcWrapper(std::function<void(const char*)> clb)
            : m_callback(clb){}

        NOINLINE static void __fastcall proc(cmdProcWrapper* wr, const char* param) {
            wr->m_callback(param);
        }
    };

    void SAMP::addCmd(std::string_view cmd, std::function<void(const char*)> proc) {
        using namespace Xbyak::util;
        Xbyak::CodeGenerator* code = new Xbyak::CodeGenerator;
        cmdProcWrapper*       wrapper = new cmdProcWrapper(proc);

        code->mov(ecx, DWORD(wrapper));
        code->mov(edx, edi);
        code->call(&cmdProcWrapper::proc);
        code->ret();

        pCmdRect(pInput, cmd.data(), code->getCode<cmdproc_t>());
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
