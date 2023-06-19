/*
    AsiPluginTemplate header file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#ifndef __BASE_HOOK_D3D9_HPP__
#define __BASE_HOOK_D3D9_HPP__

#include <d3d9.h>
#include <memory>
#include <boost/signals2.hpp>
#include <cyanide/hook_impl_polyhook.hpp>
#include "SAMP.hpp"

namespace base {
    namespace hooks {
        class hkD3D9 {
        public:
            hkD3D9() = default;
            ~hkD3D9() {
                if (m_pDevice) {
                    m_pDevice->Release();
                    m_pDevice = nullptr;
                }
            }

            boost::signals2::signal<void(LPDIRECT3DDEVICE9, const LPRECT, const LPRECT, HWND, const LPRGNDATA)> onPresentEvent;
            boost::signals2::signal<void(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*)>                            onLostDevice;
            boost::signals2::signal<void(HRESULT, LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*)>                   onResetDevice;
            boost::signals2::signal<void(LPDIRECT3DDEVICE9)>                                                    onInitDevice;

        protected:
            LPDIRECT3DDEVICE9 m_pDevice{};

            [[maybe_unused]] void initHooks();

            using reset_t = HRESULT(__stdcall*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
            using present_t = HRESULT(__stdcall*)(LPDIRECT3DDEVICE9, const LPRECT, const LPRECT, HWND, const LPRGNDATA);
            std::unique_ptr<cyanide::polyhook_x86<reset_t, std::function<HRESULT(reset_t, LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*)>>>                                m_reset;
            std::unique_ptr<cyanide::polyhook_x86<present_t, std::function<HRESULT(present_t, LPDIRECT3DDEVICE9, const LPRECT, const LPRECT, HWND, const LPRGNDATA)>>> m_present;
        };

        class hkD3D9Game : public hkD3D9 {
        public:
            [[maybe_unused]] static hkD3D9Game* getRef();

            explicit hkD3D9Game();
        };

        class hkD3D9SAMP : public hkD3D9 {
        public:
            [[maybe_unused]] static hkD3D9SAMP* getRef();

            explicit hkD3D9SAMP();
        };

    } // namespace hooks
} // namespace base

#endif
