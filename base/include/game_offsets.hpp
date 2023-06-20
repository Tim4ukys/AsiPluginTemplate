/*
    AsiPluginTemplate header file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#ifndef __BASE_GAME_OFFSETS_HPP__
#define __BASE_GAME_OFFSETS_HPP__

#include <cinttypes>
#include <utility>

namespace base {
    namespace offsets {
        template<std::uintptr_t addr>
        class offset {
        public:
            static constexpr std::uintptr_t ADDR = addr;

            template<typename T>
            static constexpr T get() noexcept {
                return reinterpret_cast<std::decay_t<T>>(ADDR);
            }
        };

        namespace func {
            constexpr offset<0x747EB0> WNDPROC;
        }

        namespace vars {
            constexpr offset<0xC17054> WIN_HWND; // HWND*
            constexpr offset<0xC97C20> D3D9; // IDirect3D*
            constexpr offset<0xC97C28> D3D9DEVICE; // IDirect3DDevice9*
        }
    }
}

#endif