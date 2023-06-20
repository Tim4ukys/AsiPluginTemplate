/*
    AsiPluginTemplate source file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#include "hookWndProc.hpp"
#include <Windows.h>
#include "SAMP.hpp"
#include "base_safe_check.hpp"
#include "game_offsets.hpp"

namespace base {
    namespace hooks {
        hkWndProcSAMP::hkWndProcSAMP(wndproc_hk_t proc) {
            const DWORD OFFSETS[6]{
                {0x5DB40}, // 037-r1
                {0x5DC10}, // 037-r2
                {0x60EE0}, // 037-r3_1
                {0x61610}, // 037-r4
                {0x61650}, // 037-r4_2
                {0x61650}, // 037-r5_1
            };
            BASE_CHECK_ARRAYSIZE(OFFSETS, SAMP::SAMP_COUNT_SUPPORT_VERSIONS);
            const auto& offset = OFFSETS[static_cast<int>(SAMP::getRef()->getSAMPVersion())];

            m_wndProc = std::make_unique<typename decltype(m_wndProc)::element_type>(
                std::move(SAMP::getRef()->getDLL().getAddr<wndproc_t>(offset)),
                std::move(proc));     
            m_wndProc->install();
        }

        hkWndProcGame::hkWndProcGame(wndproc_hk_t proc) {
            m_wndProc = std::make_unique<typename decltype(m_wndProc)::element_type>(
                std::move(offsets::func::WNDPROC.get<wndproc_t>()),
                std::move(proc));
            m_wndProc->install();
        }
    }
}