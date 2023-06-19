/*
    AsiPluginTemplate header file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#ifndef __BASE_WND_PROC_HPP__
#define __BASE_WND_PROC_HPP__

#include <boost/signals2.hpp>
#include <cyanide/hook_impl_polyhook.hpp>

namespace base {
    namespace hooks {

        class hkWndProc {
        public:
            using wndproc_t = HRESULT(__stdcall*)(HWND, UINT, WPARAM, LPARAM);
            using wndproc_hk_t = std::function<HRESULT(wndproc_t, HWND, UINT, WPARAM, LPARAM)>;

        protected:
            std::unique_ptr<cyanide::polyhook_x86<wndproc_t, wndproc_hk_t>> m_wndProc;
        };

        class hkWndProcSAMP : public hkWndProc {
        public:
            explicit hkWndProcSAMP(wndproc_hk_t proc);
        };

        class hkWndProcGame : public hkWndProc {
        public:
            explicit hkWndProcGame(wndproc_hk_t proc);
        };

    }
}

#endif
