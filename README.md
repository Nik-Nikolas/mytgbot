Squad of telegram bots with the multiple features e.g. LLM chats, URL requests, memorizing etc.

---

Dependencies:

-cpp-subprocess (as a header file already placed in project include dir)

-llama.cpp (one needs to download repo https://github.com/ggerganov/llama.cpp and to build/install executable (resulted main exec file is being used as an LLM models launcher) as well as to download any desirable LLM model(s) file(s) like https://huggingface.co/TheBloke/Llama-2-7B-Chat-GGUF/blob/main/llama-2-7b-chat.Q3_K_L.gguf)

-tgbot-cpp (one needs to download repo https://github.com/reo7sp/tgbot-cpp and to build/install library libTgBot.a)

-at least two created Telegram bots (use Telegram BotFather) and its token placed to mytgbot/bot_token1, bot_token2 files

---

Authenticate/Build/run:

-create your own telegram bots (2 at the moment), obtain TG tokens and then create the files mytgbot/bot_token1, mytgbot/bot_token2 with the correspondent tokens

-make sure that Projects/tgbot-cpp and Projects/llama.cpp already cloned, built, deployed (libTgBot.a installed, any desirable models placed to Projects/llama.cpp/models,  llm/my_chat*.sh scripts tuned due to models files names)

-cd Projects && git clone mytgbot && cd mytgbot && git submodule add https://github.com/google/googletest.git && mkdir build && cd build && cmake .. && make && ./mytgbot

-OPTIONAL tune llm/my_chat*.sh llama launcher scripts for better performance

---

Команды чата: 

/stat: Общая статистика чата

---

Команды бота Чук: 

Чук погоду: покажет погоду

Чук шутку: скажет шутку

Чук Гек <подсказка> : запустит LLM диалог ботов на тему (промпт) <подсказка>

Чук запомни : запомнит запись

Чук напомни: покажет все записи

Чук забудь <запись>: забудет <запись>

Чук забудь <индекс>: забудет запись с индексом (номером) <индекс> (индексы стартуют от 1)

Чук замри: замолчит

Чук отомри: начнет общаться

Чук скажи  [детально] <промпт>: запустит LLM ответ бота. [детально]-опция длинного ответа LLM (умолчательно ответ короткий), <промпт>- запрос (prompt) для LLM

Чук поищи <текст>: поищет вхождения <текст> в истории переписки

Чук историю: выведет историю переписки

/canary= X: устанавливает режим канарейки (назначить режим канарейки с периодом Х сек.)

/llm1: llama-2-7b OR DELETED

/llm2: llama-2-13b-chat.Q4_0 (ENG)

/llm3: firefly-llama2-13b-chat.Q6_K (CHINESE)

/llm4: llama2-13b-estopia.Q8_0 (ENG)

/llm5: ruGPT-3.5-13B-Q8_0 (RUS)

---

Команды бота Гек: 

Гек замри: замолчит

Гек отомри: начнет общаться

Гек скажи  [детально] <промпт>: запустит LLM ответ бота. [детально]-опция длинного ответа LLM (умолчательно ответ короткий), <промпт>- запрос (prompt) для LLM

/canary= X: устанавливает режим канарейки (назначить режим канарейки с периодом Х сек.)
