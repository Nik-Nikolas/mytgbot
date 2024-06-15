#include <gtest/gtest.h>

#include <cctype>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iterator>
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

#include "Command_parser.h" 

namespace mytgbottest{

    class TestCommandParser : public ::testing::Test {

    protected:
        void SetUp() override
        {
        }

        void TearDown()override
        {
        }
    };

}

using namespace mytgbottest;

TEST_F(TestCommandParser, Correct_token) {
    const std::string token = "/canary=";
    ASSERT_TRUE(CommandParser::parseDelayMessage(token + " 42", token));
}

TEST_F(TestCommandParser, Wrong_token) {
    std::string token = "/canary=";
    std::string token_wrong = "Wrong_token";    
    ASSERT_FALSE(CommandParser::parseDelayMessage(token_wrong + " 42", token));
}