# AsiPluginTemplate
Шаблон для создания ASI плагина.
## Описание и примеры
### SAMP
```cpp
// Methods:

using cmdproc_t = void(__cdecl*)(const char* param);
void addCmd(std::string_view cmd, cmdproc_t proc); // Добавляет команду в список команд SA-MP
// args:
// cmd - Название команды
// proc - функция, которая будет вызываться после вызова команды
// example:
base::SAMP::getRef()->addCmd("test", [](const char* param) { base::SAMP::getRef()->addMessage(-1, "Шалом православные :D"); });
// ----------------
void addMessage(DWORD color, const char* fmt, ...); // Добавляет сообщение в чат
// args:
// color - цвет сообщения ARGB (Alpha канал в любом случае будет 0xFF. Он не учитывается)
// fmt - сообщение
// example:
base::SAMP::getRef()->addMessage(0xFFAA00, "XOR ecx, ecx | CALL :heart: %s", "=(");
// ----------------
inline const auto& getDLL() const noexcept; // Возвращает snippets::DynamicLibrary samp.dll
// return snippets::DynamicLibrary m_samp{"samp.dll"};
// example(very bad):
base::SAMP::getRef()->getDLL.getAddr<void(__thiscall*)(PVOID pChat, const char* msg, DWORD color)>(0xFF)(nullptr, "test crash =D", 0xFF'FF'AA'FF);
// ----------------
inline SAMP_VERSION getSAMPVersion(); // Возвращает версию сампа. Может вызвать исключение
// return enum SAMP_VERSION
// example:
if (base::SAMP::getSAMPVersion() == base::SAMP::SAMP_VERSION::v037_r2) {}

// Enums:
enum class SAMP_VERSION : size_t {
  v037_r1,
  v037_r2,

  v037_r3_1,
  v037_r4,
  v037_r4_2,
  v037_r5_1,

  COUNT, // Нужен для автоматизации создания переменной, в которой хранится кол-во версий
  UNKOWN = 0xFF'FF'FF'FF
};

// Members: 
struct {
  boost::signals2::signal<void()> onInitNetGame; // Вызывается сразу же, после создания NetGame
} enums;
// example:
base::SAMP::getRef()->events.onInitNetGame.connect([](){ base::SAMP::getRef()->addMessage(-1, "Юр, это притча, о Боге на войне..."); });
```
## dependencies
### build, vcpkg packets
```shell
./vcpkg install polyhook2:x86-windows-static-md xbyak:x86-windows-static-md boost-signals2:x86-windows-static-md
```
### game
```
gta_sa.exe v1.00 US - https://www.blast.hk/threads/1830/
SA-MP v037(r1, r2, r3-1, r4, r4-2 or r5-1) - https://www.sa-mp.com/download.php
Silent's ASI Loader - https://www.gtagarage.com/mods/show.php?id=21709
```
## third-party
```
name | author | license | url
-----------------------------
cyanide - r4nx & imring - unlicense - https://github.com/imring/cyanide/
polyhook2_0 - stevemk14ebr - MIT License - https://github.com/stevemk14ebr/PolyHook_2_0
boost - Boost Software License - https://www.boost.org/
```
