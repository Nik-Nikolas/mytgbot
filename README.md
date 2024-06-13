Squad of telegram bots with the multiple features e.g. LLM chats, URL requests, memorizing etc.

Dependencies:
-cpp-subprocess (as a header file already placed in project include dir)
-llama.cpp (one needs to download and to build it)
-tgbot-cpp (one needs to download and to build+install it)

Authenticate/Build/run:

-create your own telegram bots (2 at the moment), obtain TG tokens and then create the files mytgbot/bot_token1, mytgbot/bot_token2 with the correspondent tokens
-git clone mytgbot && cd mytgbot && mkdir build && cd build && cmake .. && make && ./mytgbot

