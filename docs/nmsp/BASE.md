# class SAMP
Нужен для взаимодействия с клиентом SAMP.

## Methods:
```cpp
// addMessage - Добавляет сообщение в чат
// args:
// color - цвет сообщения ARGB (Alpha канал в любом случае будет 0xFF. Он не учитывается)
// fmt - сообщение
void addMessage(DWORD color, const char* fmt, ...);

// example:
base::SAMP::getRef()->addMessage(0xFFAA00, "XOR ecx, ecx | CALL :heart: %s", "=(");

// ----------------

// addCmd - Добавляет команду в список команд SA-MP
// args:
// cmd - Название команды
// proc - функция, которая будет вызываться
using cmdproc_t = void(__cdecl*)(const char* param);
void addCmd(std::string_view cmd, cmdproc_t proc);

// Отличается от того метода тем, что в качестве обработчика будет std::function.
// Это позволит использоваться лямбды вырожения с захватом, с bind и т.д.
void addCmd(std::string_view cmd, std::function<void(const char*)> proc);


// example:
base::SAMP::getRef()->addCmd("test", [](const char* param) { base::SAMP::getRef()->addMessage(-1, "Шалом православные :D"); });
base::SAMP::getRef()->addCmd("test", [&](const char* param) { base::SAMP::getRef()->addMessage(-1, "Мы рифмуем на века, не для пабликов вк!"); });

// ----------------

// getDLL - Возвращает snippets::DynamicLibrary samp.dll
// return: snippets::DynamicLibrary m_samp{"samp.dll"};
inline const auto& getDLL() const noexcept;

// example(не повторять):
auto addClientMessage = base::SAMP::getRef()->getDLL.getAddr<void(__thiscall*)(PVOID pChat, const char* msg, DWORD color)>(0xFF);
addClientMessage(nullptr, "test crash =D", 0xFF'FF'AA'FF);

// ----------------

// getSAMPVersion - Возвращает версию сампа. Может вызвать исключение
// return: enum SAMP_VERSION
inline SAMP_VERSION getSAMPVersion();

// example:
if (base::SAMP::getSAMPVersion() == base::SAMP::SAMP_VERSION::v037_r2) {}
```

## Enums:
```cpp
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
```

## Public members
```cpp
struct {
  boost::signals2::signal<void()> onInitNetGame; // Вызывается сразу же, после создания NetGame
} events;

// example:
base::SAMP::getRef()->events.onInitNetGame.connect(
    [](){ 
        base::SAMP::getRef()->addMessage(-1, "Юр, это притча, о Боге на войне..."); 
    });
```