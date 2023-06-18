/*
    AsiPluginTemplate source file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#include "hookD3D9.hpp"
#include "base_safe_check.hpp"

namespace base {
    namespace hooks {

        hkD3D9Game* hkD3D9Game::getRef() {
            static std::unique_ptr<hkD3D9Game> singl = std::make_unique<hkD3D9Game>();
            return singl.get();
        }
        hkD3D9Game::hkD3D9Game() {
            m_pDevice = *reinterpret_cast<LPDIRECT3DDEVICE9*>(0xC97C28);
            m_pDevice->AddRef();
            initHooks();
        }

        hkD3D9SAMP* hkD3D9SAMP::getRef() {
            static std::unique_ptr<hkD3D9SAMP> singl = std::make_unique<hkD3D9SAMP>();
            return singl.get();
        }
        hkD3D9SAMP::hkD3D9SAMP() {
            SAMP::getRef()->events.onInitNetGame.connect(
                [&]() {
                    const DWORD OFFSETS[6][1]{
                        /*pDevice*/
                        {0x21A0A8}, // 037-r1
                        {0x21A0B0}, // 037-r2
                        {0x26E888}, // 037-r3_1
                        {0x26E9B8}, // 037-r4
                        {0x26E9B8}, // 037-r4_2
                        {0x26EB40}, // 037-r5_1
                    };
                    BASE_CHECK_ARRAYSIZE(OFFSETS, SAMP::SAMP_COUNT_SUPPORT_VERSIONS);

                    const auto& offset = OFFSETS[static_cast<size_t>(SAMP::getRef()->getSAMPVersion())];
                    m_pDevice = *SAMP::getRef()->getDLL().getAddr<LPDIRECT3DDEVICE9*>(offset[0]);
                    m_pDevice->AddRef();
                    initHooks();
                });
        }

        void hkD3D9Intefrace::initHooks() {
            onInitDevice(m_pDevice);

            auto getDeviceAddress = [&](int VTableIndex) {
                PDWORD VTable;
                *reinterpret_cast<DWORD*>(&VTable) = *reinterpret_cast<DWORD*>(m_pDevice);
                return VTable[VTableIndex];
            };

            m_reset = std::make_unique<typename decltype(m_reset)::element_type>(
                std::move(reset_t(getDeviceAddress(16))),
                std::move(std::function(
                    [&](reset_t orig, LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams) -> HRESULT {
                        onLostDevice(pDevice, pPresentParams);
                        HRESULT res = orig(pDevice, pPresentParams);
                        onResetDevice(res, pDevice, pPresentParams);
                        return res;
                    })));
            m_present = std::make_unique<typename decltype(m_present)::element_type>(
                std::move(present_t(getDeviceAddress(17))),
                std::move(std::function(
                    [&](present_t         orig,
                        LPDIRECT3DDEVICE9 pDevice, const LPRECT pSrcRect, const LPRECT pDestRect, HWND hWnd, const LPRGNDATA pDirtyRegion) -> HRESULT {
                        onPresentEvent(pDevice, pSrcRect, pDestRect, hWnd, pDirtyRegion);
                        return orig(pDevice, pSrcRect, pDestRect, hWnd, pDirtyRegion);
                    })));
        }
    }
}