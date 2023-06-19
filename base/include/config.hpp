/*
    AsiPluginTemplate header file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#ifndef __BASE_CONFIG_HPP__
#define __BASE_CONFIG_HPP__

#include <string_view>
#include <map>
#include <codecvt>
#include <variant>
#include <exception>
#include <Windows.h>
#include "snippets.hpp"

#pragma warning(push)
#pragma warning(disable : 4996)

namespace base {

    template<typename typeValue>
    struct Value {
        typeValue         vl;
        std::wstring_view m_keyName;
        using valueType = std::decay_t<typeValue>;

        Value(std::wstring_view keyName, typeValue value)
            : vl(value),
              m_keyName(keyName) {
        }
    };

    class ConfigReg {
        HKEY m_hKey;

    public:
        using type = std::variant<std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t, std::int32_t, std::uint64_t, std::int64_t,
                                  float, double,
                                  std::wstring, std::string,
                                  bool>;

        template<typename... Args>
        explicit ConfigReg(HKEY key, std::wstring_view subKey, Args&&... args) {
            HKEY hKey;
            if (RegOpenKeyW(key, subKey.data(), &hKey) && RegCreateKeyW(key, subKey.data(), &hKey)) {
                throw std::exception("Can't create reg key");
            }
            m_hKey = hKey;

            (checkKey(std::forward<Args>(args)), ...);
        }
        ~ConfigReg() {
            for (auto& [key, var] : m_conf) {
                std::visit([&, &k = key](auto& arg) {
                    using tp = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<tp, std::wstring>) {
                        RegSetValueExW(m_hKey, k.c_str(), NULL, REG_SZ, PBYTE(arg.data()), DWORD(arg.size() * sizeof(wchar_t)));
                    } else if constexpr (std::is_same_v<tp, std::string>) {
                        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                        RegSetValueExA(m_hKey, converter.to_bytes(k).c_str(), NULL, REG_SZ, PBYTE(arg.data()), DWORD(arg.size()));
                    } else {
                        int flag;
                        if constexpr (sizeof(tp) == 4) {
                            flag = REG_DWORD;
                        } else if constexpr (sizeof(tp) == 8) {
                            flag = REG_QWORD;
                        } else {
                            flag = REG_BINARY;
                        }

                        RegSetValueExW(m_hKey, k.c_str(), NULL, flag, PBYTE(&arg), sizeof(tp));
                    }
                },
                           var);
            }
            RegCloseKey(m_hKey);
        };

        auto& operator[](const std::wstring& key) {
            return m_conf[key];
        }
        template<typename T>
        auto& get(const std::wstring& key) {
            if constexpr (std::is_same_v<std::decay_t<T>, std::decay_t<type>>) {
                return m_conf[key];
            } else {
                return std::get<T>(m_conf[key]);
            }
        }
        auto& getConf() noexcept {
            return m_conf;
        }

    private:
        std::map<std::wstring, type> m_conf;

        template<typename T>
        void checkKey(T&& val) {
            if constexpr (snippets::if_one_same<decltype(T::vl), std::wstring_view, std::wstring>) {
                DWORD        sz;
                std::wstring text;
                if (RegGetValueW(m_hKey, NULL, val.m_keyName.data(), RRF_RT_REG_SZ, NULL, NULL, &sz) == ERROR_SUCCESS) {
                    sz -= 1 * sizeof(wchar_t);
                    text.resize(sz / sizeof(wchar_t));
                    if (RegGetValueW(m_hKey, NULL, val.m_keyName.data(), RRF_RT_REG_SZ, NULL, text.data(), &sz) == ERROR_SUCCESS) {
                        text.pop_back();
                        m_conf.insert({std::move(std::wstring(val.m_keyName)), std::move(text)});
                        return;
                    }
                }
                RegSetValueExW(m_hKey, val.m_keyName.data(), NULL, REG_SZ, PBYTE(val.vl.data()), DWORD(val.vl.size() * sizeof(wchar_t)));
                m_conf.insert({std::move(std::wstring(val.m_keyName)), std::move(std::wstring(val.vl))});
            } else if constexpr (snippets::if_one_same<decltype(T::vl), std::string_view, std::string>) {
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                DWORD                                                  sz;
                std::string                                            key{std::move(converter.to_bytes(val.m_keyName.data()))};
                std::string                                            text;
                if (RegGetValueA(m_hKey, NULL, key.data(), RRF_RT_REG_SZ, NULL, NULL, &sz) == ERROR_SUCCESS) {
                    sz -= 1;
                    text.resize(sz);
                    if (RegGetValueA(m_hKey, NULL, key.data(), RRF_RT_REG_SZ, NULL, text.data(), &sz) == ERROR_SUCCESS) {
                        text.pop_back();
                        m_conf.insert({std::move(std::wstring(val.m_keyName)), std::move(text)});
                        return;
                    }
                }
                RegSetValueExA(m_hKey, key.c_str(), NULL, REG_SZ, PBYTE(val.vl.data()), DWORD(val.vl.size()));
                m_conf.insert({std::move(std::wstring(val.m_keyName)), std::move(std::string(val.vl))});
            } else {
                DWORD sz{sizeof(T::valueType)};

                int flag1, flag2;
                if constexpr (sizeof(T::valueType) == 4) {
                    flag1 = RRF_RT_DWORD;
                    flag2 = REG_DWORD;
                } else if constexpr (sizeof(T::valueType) == 8) {
                    flag1 = RRF_RT_QWORD;
                    flag2 = REG_QWORD;
                } else {
                    flag1 = RRF_RT_REG_BINARY;
                    flag2 = REG_BINARY;
                }

                if (RegGetValueW(m_hKey, NULL, val.m_keyName.data(), flag1, NULL, &val.vl, &sz) != ERROR_SUCCESS) {
                    RegSetValueExW(m_hKey, val.m_keyName.data(), NULL, flag2, PBYTE(&val.vl), sz);
                }
                m_conf.insert({std::wstring(val.m_keyName.data()), val.vl});
            }
        }
    };
} // namespace base

#pragma warning(pop)

#endif