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
#ifndef __BASE_SAMP_HPP__
#define __BASE_SAMP_HPP__

#include <memory>
#include <string_view>
#include <exception>
#include <Windows.h>
#include <cyanide/hook_impl_polyhook.hpp>
#include <boost/signals2.hpp>
#include "snippets.hpp"

namespace base {

    class SAMP {
    public:
        explicit SAMP();
        ~SAMP() = default;

        using cmdproc_t = void(__cdecl*)(const char* param);
        /**
        * @brief ��������� �������
        * @param cmd �������
        * @param proc ����������
        * @example pSAMP->addCmd("test", [](const char* param) { pSAMP->addMessage(-1, "Hello wolrd"); });
        */
        [[maybe_unused]] void addCmd(std::string_view cmd, cmdproc_t proc);
        /**
        * @brief ��������� ��������� � ���
        * @param color ���� ���������(RGB)
        * @param fmt ����� ���������
        * @warning ������������ ���-�� �������� 144.
        */
        [[maybe_unused]] void addMessage(DWORD color, const char* fmt, ...);

        [[maybe_unused]] static SAMP* getRef();

    private:

        PVOID pNetGame{}, pChat, pInput;
        void(__thiscall* pAddMsg)(PVOID pthis, DWORD color, const char* text);
        void(__thiscall* pCmdRect)(PVOID pthis, const char* cmd, cmdproc_t cmdProc);
        [[maybe_unused]] void initPointers();

        snippets::DynamicLibrary m_samp{"samp.dll"};

        static constexpr size_t SAMP_COUNT_SUPPORT_VERSIONS = 2;
    public:
        enum class SAMP_VERSION : size_t {
            v037_r3_1,
            v037_r5_1,

            UNKOWN = 0xFF'FF'FF'FF
        };
        inline SAMP_VERSION getSAMPVersion() {
            switch (m_samp.getNTHeader()->OptionalHeader.AddressOfEntryPoint) {
            case 0xCC4D0: return SAMP_VERSION::v037_r3_1;
            case 0xCBC90: return SAMP_VERSION::v037_r5_1;
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

        [[maybe_unused]] void initEvents();
    };

}

#endif