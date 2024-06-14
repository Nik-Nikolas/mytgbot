Squad of telegram bots with the multiple features e.g. LLM chats, URL requests, memorizing etc.

---

Dependencies:

-cpp-subprocess (as a header file already placed in project include dir)

-llama.cpp (one needs to download and to build it as well as to download any desirable LLM model(s))

-tgbot-cpp (one needs to download and to build+install it)

---

Authenticate/Build/run:

-create your own telegram bots (2 at the moment), obtain TG tokens and then create the files mytgbot/bot_token1, mytgbot/bot_token2 with the correspondent tokens

-git clone mytgbot && cd mytgbot && mkdir build && cd build && cmake .. && make && ./mytgbot

-OPTIONAL tune my_chat*.sh llama launcher scripts for better performance

---

Команды чата: 

/stat: Общая статистика чата

---

Команды бота Чук: 

Чук погоду: покажет погоду

Чук шутку: скажет шутку

Чук Гек <подсказка> : запустит LLM диалог ботов

Чук запомни : запомнит напоминалку

Чук напомни: напомнит напоминалку

Чук забудь : забудет напоминалку

Чук замри: замолчит

Чук отомри: начнет болтать снова

Чук скажи  <детально> <инструкция/команда>: триггернёт бота. <детально>-опция длинного ответа LLM (умолчательно ответ короткий), <инструкция/команда>- запрос (prompt) для LLM

Чук поищи : поищет совпадения в истории переписки

/canary=: устанавливает режим канарейки. canary= X: назначить режим канарейки с периодом Х сек.

Чук историю: выведет историю переписки

/llm1: llama-2-7b OR DELETED

/llm2: llama-2-13b-chat.Q4_0 (ENG)

/llm3: firefly-llama2-13b-chat.Q6_K (CHINESE)

/llm4: llama2-13b-estopia.Q8_0 (ENG)

/llm5: ruGPT-3.5-13B-Q8_0 (RUS)

---

Команды бота Гек: 

Гек замри: замолчит

Гек отомри: начнет болтать снова

Гек скажи  <детально> <инструкция/команда>: триггернёт бота. <детально>-опция длинного ответа LLM (умолчательно ответ короткий), <инструкция/команда>- запрос (prompt) для LLM

/canary=: устанавливает режим канарейки. canary= X: назначить режим канарейки с периодом Х сек.