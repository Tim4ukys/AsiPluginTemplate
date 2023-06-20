/*
    AsiPluginTemplate source file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#include "hookD3D9.hpp"
#include "base_safe_check.hpp"
#include <xbyak/xbyak.h>
#include <functional>
#include <patch.hpp>

namespace base {
    namespace hooks {

        void __fastcall hkD3D9::hkInit(hkD3D9* pthis, LPDIRECT3DDEVICE9 pDevice) {
            pthis->m_pDevice = pDevice;
            pthis->initHooks();
        }
        hkD3D9::hkD3D9() {
            auto pDevice = *reinterpret_cast<LPDIRECT3DDEVICE9*>(0xC97C28);
            if (!pDevice) {
                using namespace Xbyak::util;
                Xbyak::CodeGenerator* code = new Xbyak::CodeGenerator;
                // eax = pDevice
                code->mov(edx, eax);
                code->push(eax); // save eax
                code->push(edx); // pDevice
                code->mov(ecx, DWORD(this));
                code->push(ecx); // pthis
                code->call(hkInit);
                code->add(esp, 8);
                code->pop(eax); // get eax

                patch::setShellCodeThroughJump(0x7F67C6, *code, 5u, TRUE);
            } else {
                m_pDevice = pDevice;
                initHooks();
            }
        }

        hkD3D9* hkD3D9::getRef() {
            static std::unique_ptr<hkD3D9> singl = std::make_unique<hkD3D9>();
            return singl.get();
        }

        void hkD3D9::initHooks() {
            onInitDevice(m_pDevice.Get());

            auto getDeviceAddress = [&](int VTableIndex) {
                PDWORD VTable;
                *reinterpret_cast<DWORD*>(&VTable) = *reinterpret_cast<DWORD*>(m_pDevice.Get());
                return VTable[VTableIndex];
            };

            m_reset = std::make_unique<typename decltype(m_reset)::element_type>(
                std::move(reset_t(getDeviceAddress(16))),
                std::move(
                    [&](reset_t orig, LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams) -> HRESULT {
                        onLostDevice(pDevice, pPresentParams);
                        HRESULT res = orig(pDevice, pPresentParams);
                        onResetDevice(res, pDevice, pPresentParams);
                        return res;
                    }));
            m_reset->install();
            m_present = std::make_unique<typename decltype(m_present)::element_type>(
                std::move(present_t(getDeviceAddress(17))),
                std::move(
                    [&](present_t         orig,
                        LPDIRECT3DDEVICE9 pDevice, const LPRECT pSrcRect, const LPRECT pDestRect, HWND hWnd, const LPRGNDATA pDirtyRegion) -> HRESULT {
                        onPresentEvent(pDevice, pSrcRect, pDestRect, hWnd, pDirtyRegion);
                        return orig(pDevice, pSrcRect, pDestRect, hWnd, pDirtyRegion);
                    }));
            m_present->install();
        }
    }
}