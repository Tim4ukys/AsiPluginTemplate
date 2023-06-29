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
#include <wrl/client.h>
#include "SAMP.hpp"

namespace base {
    namespace hooks {
        class hkD3D9 {
            static LPDIRECT3DDEVICE9 __fastcall hkInit(hkD3D9* pthis, LPDIRECT3DDEVICE9 pDevice);

        public:
            explicit hkD3D9();
            ~hkD3D9() = default;

            [[maybe_unused]] static hkD3D9* getRef();

            boost::signals2::signal<void(LPDIRECT3DDEVICE9, const LPRECT, const LPRECT, HWND, const LPRGNDATA)> onPresentEvent;
            boost::signals2::signal<void(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*)>                            onLostDevice;
            boost::signals2::signal<void(HRESULT, LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*)>                   onResetDevice;
            boost::signals2::signal<void(LPDIRECT3DDEVICE9)>                                                    onInitDevice;

        protected:
            Microsoft::WRL::ComPtr<IDirect3DDevice9> m_pDevice;

            [[maybe_unused]] void initHooks();

            using reset_t = HRESULT(__stdcall*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
            using present_t = HRESULT(__stdcall*)(LPDIRECT3DDEVICE9, const LPRECT, const LPRECT, HWND, const LPRGNDATA);
            std::unique_ptr<cyanide::polyhook_x86<reset_t, std::function<HRESULT(reset_t, LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*)>>>                                m_reset;
            std::unique_ptr<cyanide::polyhook_x86<present_t, std::function<HRESULT(present_t, LPDIRECT3DDEVICE9, const LPRECT, const LPRECT, HWND, const LPRGNDATA)>>> m_present;
        };

    } // namespace hooks
} // namespace base

#endif
