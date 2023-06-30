/*
    AsiPluginTemplate header file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#ifndef __BASE_SAMP_HPP__
#define __BASE_SAMP_HPP__

#include <memory>
#include <string_view>
#include <exception>
#include <Windows.h>
#include <cyanide/hook_impl_polyhook.hpp>
#include <boost/signals2.hpp>
#include <xbyak/xbyak.h>
#include "snippets.hpp"

namespace base {

    class SAMP {
    public:
        explicit SAMP();
        ~SAMP() = default;

        using cmdproc_t = void(__cdecl*)(const char* param);
        /**
        * @brief Добавляет команду
        * @param cmd команда
        * @param proc обработчик
        * @example pSAMP->addCmd("test", [](const char* param) { pSAMP->addMessage(-1, "Hello wolrd"); });
        */
        [[maybe_unused]] void addCmd(std::string_view cmd, cmdproc_t proc);

        /**
         * @brief Добавляет команду
         * @param cmd команда
         * @param proc обработчик(std::function)
         * @example pSAMP->addCmd("test", [&](const char* param) { pSAMP->addMessage(-1, "Hello wolrd"); });
         */
        [[maybe_unused]] void addCmd(std::string_view cmd, std::function<void(const char*)> proc);

        /**
        * @brief Добавляет сообщение в чат
        * @param color цвет сообщения(RGB)
        * @param fmt текст сообщения
        * @warning Максимальное кол-во символов 144.
        * @example pSAMP->addMessage(0xFFAA00, "Hello, {FFFFFF}%s!", "mr.putin");
        */
        [[maybe_unused]] void addMessage(DWORD color, const char* fmt, ...);

        [[maybe_unused]] static SAMP* getRef();

    
        class CPointers {
            std::uintptr_t m_nAddress{};
        public:
            CPointers() = default;
            ~CPointers() = default;

            template<typename T>
            inline T* get(std::uintptr_t offset) const noexcept {
                return reinterpret_cast<T*>(m_nAddress + offset);
            }

            inline operator PVOID() {
                return reinterpret_cast<PVOID>(m_nAddress);
            }
            inline PVOID operator=(const PVOID& a) {
                m_nAddress = reinterpret_cast<std::uintptr_t>(a);
                return a;
            }
            inline operator bool() const noexcept {
                return m_nAddress != 0;
            }
        };
        struct {
            CPointers pNetGame, pChat, pInput;
        } pointers;

    private:
        void(__thiscall* pAddMsg)(PVOID pthis, DWORD color, const char* text);
        void(__thiscall* pCmdRect)(PVOID pthis, const char* cmd, cmdproc_t cmdProc);
        [[maybe_unused]] void initPointers();

        snippets::DynamicLibrary m_samp{"samp.dll"};
    public:
        inline const auto& getDLL() const noexcept {
            return m_samp;
        }

        enum class SAMP_VERSION : size_t {
            v037_r1,
            v037_r2,

            v037_r3_1,
            v037_r4,
            v037_r4_2,
            v037_r5_1,

            COUNT,
            UNKOWN = 0xFF'FF'FF'FF
        };
        static constexpr size_t SAMP_COUNT_SUPPORT_VERSIONS = static_cast<size_t>(SAMP_VERSION::COUNT);
        inline SAMP_VERSION     getSAMPVersion() {
            switch (m_samp.getNTHeader()->OptionalHeader.AddressOfEntryPoint) {
            case 0x31DF13: return SAMP_VERSION::v037_r1;
            case 0xCC4D0: return SAMP_VERSION::v037_r3_1;
            case 0xCBC90: return SAMP_VERSION::v037_r5_1;
            case 0xCBCD0: return SAMP_VERSION::v037_r4_2;
            case 0xCBCB0: return SAMP_VERSION::v037_r4;
            case 0x3195DD: return SAMP_VERSION::v037_r2;
            default:
                throw std::runtime_error("unkown version samp");
                return SAMP_VERSION::UNKOWN;
            }
        }
    
        struct {
            boost::signals2::signal<void()> onInitNetGame;
        } events;

    private:
        using init_net_game_t = PVOID(__thiscall*)(PVOID, const char*, int, const char*, const char*);
        std::unique_ptr<cyanide::polyhook_x86<init_net_game_t, std::function<PVOID(init_net_game_t, PVOID, const char*, int, const char*, const char*)>>>
            m_initNetGame;
        std::unique_ptr<Xbyak::CodeGenerator> m_initNetGameCodeShell;


        [[maybe_unused]] void initEvents();
    };

}

#endif