#include <functional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cctype>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <chrono>
#include <cmath>
#include <iostream>
#include <ctime> 
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <random>
#include <algorithm>
#include <future>
#include <filesystem>

#include <tgbot/tgbot.h>
using namespace std;
using namespace TgBot;


string pathPrefix {}; 
string llamaOutput {}; 
string hexagrammsFile {};
string DBfile {}; 
string reminderFile {};

const vector<string> trusted_chats_titles {"People_v_lodke",  //main 
                                           "People_v_vodke"}; //test
                                           
#include "LLM_manager.h" 
#include "Command_manager.h" 
#include "Command_parser.h" 
#include "Bot_verbose.h" 
#include "CURL_helper.h" 
#include "utils.h" 

class BotManagerMocked: public BotManager{

    public:
        BotManagerMocked(std::shared_ptr<Bot> bot): BotManager(bot){            
        }

        virtual ~BotManagerMocked() = default;

        MOCK_METHOD(bool, deleteWebhook, (), (override));
        MOCK_METHOD(void, sendMessage, (std::int64_t, const string&), (const, override));
        MOCK_METHOD(void, sendChatAction, (std::int64_t, const string&), (override));
        MOCK_METHOD(const Api&, getApi, (), (override));
        MOCK_METHOD(EventBroadcaster&, getEvents, (), (override));
        MOCK_METHOD(void, startPoll, (), (override)); 
};

class MockedSubprocess{

    public:
        MockedSubprocess(const std::string&, std::vector<std::string>){        
        }

        std::ostream& in()  {
            return cout;
        }

        void close(){
        }
};

namespace mytgbottest{

    class TestBotVerbose : public ::testing::Test {

    protected:
        void SetUp() override
        {
            bot = std::make_shared<Bot>("12345");

            llamaOutput = "temp_llama.txt";
            ofstream f (llamaOutput);
            botVerbose = make_unique<BotVerbose<BotManagerMocked>>(bot, "bot", llamaOutput);
        }

        void TearDown()override
        {
            std::remove("temp_llama.txt"); 
        }

        std::shared_ptr<Bot> bot;
        std::unique_ptr<BotVerbose<BotManagerMocked>> botVerbose;
    };
}

using namespace mytgbottest;

TEST_F(TestBotVerbose, init) {
    EXPECT_CALL(botVerbose->getManager(), deleteWebhook()).Times(1);
    botVerbose->init();
}

TEST_F(TestBotVerbose, sayWordFault) {
    EXPECT_CALL(botVerbose->getManager(), sendMessage(123,"meow")).Times(0);
    botVerbose->sayWord("meow");
}

TEST_F(TestBotVerbose, sayWordSuccess) {
    botVerbose->saveID(123);
    EXPECT_CALL(botVerbose->getManager(), sendMessage(123,"meow")).Times(1);    
    botVerbose->sayWord("meow");
}

TEST_F(TestBotVerbose, startLLMSuccess) {

    const auto ID {123};
    botVerbose->saveID(ID);

    EXPECT_CALL(botVerbose->getManager(), sendChatAction(ID, "typing")).Times(::testing::AtLeast(2));    
    EXPECT_CALL(botVerbose->getManager(), sendMessage(ID, botVerbose->getName() + NotFoundResponse)).Times(1);
    botVerbose->startLLM<MockedSubprocess>(ID, "meow");
}