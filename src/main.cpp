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

//dependencies exernal:

//tgbot-cpp library (should be built and installed)
#include <tgbot/tools/StringTools.h>
#include <tgbot/tgbot.h>

// #include <curl/curl.h> // REST API requests
#include "subprocess.hpp" // llm launcher
//llama.cpp (should be built and installed) since the app is being called with bash script via subprocess

using namespace std;
using namespace TgBot;

const vector<string> trusted_chats_titles {"People_v_lodke",  //main 
                                           "People_v_vodke"}; //test

const string bot1Name{"Чук"};
const string bot2Name{"Гек"};

string pathPrefix {}; 
string llamaOutput {}; 
string hexagrammsFile {};
string DBfile {}; 
string reminderFile {};

#include "LLM_manager.h" 
#include "Command_manager.h" 
#include "Command_parser.h" 
#include "Bot_verbose.h" 
#include "CURL_helper.h" 
#include "utils.h" 

int main() {
    setup_paths(pathPrefix, llamaOutput, hexagrammsFile, DBfile, reminderFile);

    string trumpToken {};
    string bidenToken{};

    //Aythenticate bots
    get_token(pathPrefix + "bot_token1", trumpToken); // file path_prefix + "bot_token1" should present in binary root dir
    get_token(pathPrefix + "bot_token2", bidenToken);
    Bot_verbose bot1(trumpToken, bot1Name, llamaOutput);
    Bot_verbose bot2(bidenToken, bot2Name, llamaOutput);

    const auto req_token_weather = bot1.getName() + " погоду";
    const auto req_token_joke = bot1.getName() + " шутку";
    const auto req_token_dialog = bot1.getName() + " " + bot2.getName() + " ";
    const auto req_token_remember = bot1.getName() + " запомни" + " ";
    const auto req_token_remind = bot1.getName() + " напомни";
    const auto req_token_forget = bot1.getName() + " забудь" + " ";
    const auto req_token_freeze_1 = bot1.getName() + " замри";
    const auto req_token_start_1 = bot1.getName() + " отомри";
    const auto req_token_req_1 = bot1.getName() + " скажи" + " ";
    const auto req_token_search_1 = bot1.getName() + " поищи" + " ";
    const auto req_token_canary_1 = "/canary="s;
    const auto req_token_history = bot1.getName() + " историю";

    bot1.register_command({
    req_token_weather, 
    ": покажет погоду",
    [&](int64_t id, const string& req){
        const auto current = "latitude=44.0486&longitude=43.0594&current=rain,temperature_2m,wind_speed_10m"s;
        bot1.getApi().sendMessage(id, "Weather:\n" + CURL_helper::GET("https://api.open-meteo.com/v1/forecast?" + current)); 
    }});

    bot1.register_command({
    req_token_joke, 
    ": скажет шутку",
    [&](int64_t id, const string& req){
        bot1.getApi().sendMessage(id, CURL_helper::GET("https://official-joke-api.appspot.com/random_joke")); 
    }});
        
    bot1.register_command(
    {req_token_dialog, 
    "<подсказка> : запустит LLM диалог ботов",
    [&](int64_t id, const string& req){
        auto temp_req = req;
        temp_req.replace(req.find(req_token_dialog),req_token_dialog.length(), "");   

        std::size_t count_max{5};
        while(count_max--){
            auto res = bot1.start_llm(id, temp_req);      
            temp_req = bot2.start_llm(id, res); 
        }
    }});
                
    bot1.register_command({
    req_token_remember, 
    ": запомнит напоминалку",
    [&](int64_t id, const string& req){
        auto temp_req = req;

        temp_req.replace(req.find(req_token_remember),req_token_remember.length(), "");  

        file_write_line(reminderFile, temp_req);          
        ifstream f(reminderFile);
        std::ostringstream ss;
        ss << f.rdbuf();
        bot1.sayWord("Текущие напоминания: \n" + ss.str());
    }});

    bot1.register_command({
    req_token_remind, 
    ": напомнит напоминалку",
    [&](int64_t id, const string& req){        
        ifstream f(reminderFile);
        std::ostringstream ss;
        ss << f.rdbuf();
        bot1.sayWord("Текущие напоминания: \n" + ss.str());
    }});
             
    bot1.register_command({
    req_token_forget, 
    ": забудет напоминалку",
    [&](int64_t id, const string& req){
        auto temp_req = req;
        temp_req.replace(req.find(req_token_forget),req_token_forget.length(), "");   

        file_clear_line(reminderFile, temp_req);   
        ifstream f(reminderFile);
        std::ostringstream ss;
        ss << f.rdbuf();
        bot1.sayWord("Текущие напоминания: \n" + ss.str()); 
    }});

    bot1.register_command({
    req_token_freeze_1,
     ": замолчит",
    [&](int64_t id, const string& req){
        bot1.set_silent(true);
        bot1.getApi().sendMessage(id, "Хорошо..."); 
    }});

    bot1.register_command({
    req_token_start_1, 
    ": начнет болтать снова",
    [&](int64_t id, const string& req){
        bot1.set_silent(false);
    }});

    bot1.register_command({
    req_token_req_1, 
    " <детально> <инструкция/команда>: триггернёт бота. <детально>-опция длинного ответа LLM (умолчательно ответ короткий), <инструкция/команда>- запрос (prompt) для LLM",
    [&](int64_t id, const string& req){
        auto temp_req = req;
        temp_req.replace(req.find(req_token_req_1),req_token_req_1.length(), "");  

        if(!bot1.is_silent()){                 
            bot1.start_llm(id, temp_req); 
        }
    }});

    bot1.register_command({
    req_token_search_1, 
    ": поищет совпадения в истории переписки",
    [&](int64_t id, const string& req){
        auto temp_req = req;
        temp_req.replace(req.find(req_token_search_1),req_token_search_1.length(), "");  
        string author, DBline;
        if(find_partial(temp_req, author, DBline)){

            std::vector<std::string> hexagramms;
            prepare_hexagramms(hexagramms);

            bot1.getApi().sendMessage(id, "Автор " + 
            author + 
            " уже высказывался на эту тему: \n" + 
            DBline +
            "\nПозволю себе цитату по этому поводу: \n\n" +
            select_random_line(hexagramms));
        }
    }});

    //canary= X is a pseudo command due to args followed by, so process it here (not in onCommand!)
    bot1.register_command({
    req_token_canary_1,
    ": устанавливает режим канарейки. canary= X: назначить режим канарейки с периодом Х сек.",
    [&](int64_t id, const string& req){

        if(CommandParser::parseDelayMessage(req, req_token_canary_1)){
            cout << bot1.getName() << " set canary delay: " << CommandParser::delayValue() << endl;
            bot1.setCanaryDelay(CommandParser::delayValue());
        }
    }});

    bot1.register_command({
    req_token_history, 
    ": выведет историю переписки",
    [&](int64_t id, const string& req){        
        ifstream f(DBfile);
        std::ostringstream ss;
        ss << f.rdbuf();
        auto s = ss.str();
        const auto max_history_bytes{8192 * 4};
        if(s.length() > max_history_bytes)
            s = s.substr(s.length() - max_history_bytes,max_history_bytes);
        bot1.sayWord(s);
    }});

    const auto req_token_freeze_2 = bot2.getName() + " замри";
    const auto req_token_start_2 = bot2.getName() + " отомри";
    const auto req_token_req_2 = bot2.getName() + " скажи" + " ";
    const auto req_token_canary_2 = "/canary="s;

    bot2.register_command({
    req_token_freeze_2,
     ": замолчит",
    [&](int64_t id, const string& req){
        bot2.set_silent(true);
        bot2.getApi().sendMessage(id, "Хорошо..."); 
    }});

    bot2.register_command({
    req_token_start_2, 
    ": начнет болтать снова",
    [&](int64_t id, const string& req){
        bot2.set_silent(false);
    }});

    bot2.register_command({
    req_token_req_2, 
    " <детально> <инструкция/команда>: триггернёт бота. <детально>-опция длинного ответа LLM (умолчательно ответ короткий), <инструкция/команда>- запрос (prompt) для LLM",
    [&](int64_t id, const string& req){
        auto temp_req = req;
        temp_req.replace(req.find(req_token_req_2),req_token_req_2.length(), ""); 

        if(!bot2.is_silent()){                 
            bot2.start_llm(id, temp_req); 
        }
    }});

    //canary= X is a pseudo command due to args followed by, so process it here (not in onCommand!)
    bot2.register_command({
    req_token_canary_2,
    ": устанавливает режим канарейки. canary= X: назначить режим канарейки с периодом Х сек.",
    [&](int64_t id, const string& req){

        if(CommandParser::parseDelayMessage(req, req_token_canary_2)){
            cout << bot2.getName() << " set canary delay: " << CommandParser::delayValue() << endl;
            bot2.setCanaryDelay(CommandParser::delayValue());
        }
    }});

    std::future<void> result = std::async(std::launch::async, canary_call, std::ref(bot1));
    std::future<void> result2 = std::async(std::launch::async, canary_call, std::ref(bot2));

    srand(time(0)); 
    auto start = std::chrono::high_resolution_clock::now();

    vector<pair<string,int>> users_stat;

    bot1.getEvents().onCommand(Command_manager::get_register()[0].first, [&bot1, &users_stat, &start](Message::Ptr message) {

        if(!is_secured_chat(message->chat->title))
            return;        
        
        // Stop the timer
        auto stop = std::chrono::high_resolution_clock::now();
        
        // Calculate the duration
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(stop - start);
        
        auto tsize = " минут"s;
        size_t count = duration.count();
        if(count > 60){
            count /= 60;
            tsize = " часов"s;
            if(count > 24){
                count /= 24;
                tsize = " дней"s;
            }
        }

        bot1.getApi().sendMessage(message->chat->id, "***********\nСтатистика чата: \n\nЗа последние " + to_string(count) + tsize );
        for(auto& el: users_stat){
            bot1.getApi().sendMessage(message->chat->id, "Товарищ " + el.first + " написал сообщ.: " + to_string(el.second));
        }

        string commands;
        commands.append(
            "\n\n***********\nКоманды чата: \n\n");
        for(auto& el: Command_manager::get_register()){
            commands.append(string("/") + el.first + ": " + el.second.first + "\n");
        }
        commands.append("\n\n***********\nКоманды бота " + bot1.getName() + ": \n\n");

        for(auto& el: bot1.get_commands()){
            commands.append(el.m_command + el.m_desctipt + "\n");
        }

        for(auto& el: bot1.get_LLM_manager().get_all_descr()){
            commands.append(string("/") + el.first + ": " + el.second.first + "\n");
        }
        bot1.getApi().sendMessage(message->chat->id, commands); 

        bot1.describe_LLM(message->chat->id);
    });

    //setup LLM type switch processors
    size_t index{0};
    for(const auto& el: bot1.get_LLM_manager().get_all_descr()){

        bot1.getEvents().onCommand(el.first, [&bot1, index](Message::Ptr message) {
            if(!is_secured_chat(message->chat->title))
                return;    

            bot1.get_LLM_manager().set_launcher(index);
        });

        ++index;
    }

    bot1.getEvents().onAnyMessage([&](Message::Ptr message) {

        if(is_command(message->text))
            return;

         if(!is_secured_chat(message->chat->title))
            return;

        //to be able to send canary songs to
        bot1.saveID(message->chat->id);

        //Collect users for stat
        const auto username = get_username(message);
        auto it = find_if(users_stat.begin(),users_stat.end(),[&message, &username](auto& el){return el.first == username;});
        if(it != users_stat.end()){
            it->second++;
        }
        else{ 
            if(!username.empty()){
                users_stat.emplace_back(username, 1);
            }
        }

        printf("User messageId %i\n", message->messageId);
        printf("From chat %li\n", message->chat->id);
        printf("User wrote %s\n", message->text.c_str());

        const auto req = message->text;

        if(!req.empty()){
            file_write_line(DBfile, username + " : " + req + " : " + get_timestamp());

            bot1.process_command(message->chat->id, req);
        }
    });

    bot2.getEvents().onCommand(Command_manager::get_register()[0].first, [&bot2](Message::Ptr message) {

        if(!is_secured_chat(message->chat->title))
            return;        

        string commands;   
        commands.append("\n\n***********\nКоманды бота " + bot2.getName() + ": \n\n");
     
        for(auto& el: bot2.get_commands()){
            commands.append(el.m_command + el.m_desctipt + "\n");
        }
        
        for(auto& el: bot2.get_LLM_manager().get_all_descr()){
            commands.append(string("/") + el.first + ": " + el.second.first + "\n");
        }        
        bot2.getApi().sendMessage(message->chat->id, commands);

        bot2.describe_LLM(message->chat->id);
    });

    index = 0;
    for(const auto& el: bot2.get_LLM_manager().get_all_descr()){

        bot2.getEvents().onCommand(el.first, [&bot2, index](Message::Ptr message) {
            if(!is_secured_chat(message->chat->title))
                return;    

            bot2.get_LLM_manager().set_launcher(index);
        });
        ++index;
    }

    bot2.getEvents().onAnyMessage([&](Message::Ptr message) {
        const auto req = message->text;

        if(is_command(req))
            return;

        if(!is_secured_chat(message->chat->title))
            return;

        //to be able to send canary songs to
        bot2.saveID(message->chat->id);

        if(!req.empty()){
            bot2.process_command(message->chat->id, req);
        }      
    });

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });
 
    size_t count{0};
    vector<TgLongPoll> polls;
    vector<std::reference_wrapper<Bot_verbose>> bots{bot1, bot2};
    for(auto& bot: bots){
        bot.get().getApi().deleteWebhook();
        polls.push_back(bot.get());
    }
    while (true) {
        try {
            while (true) {
                for(auto& poll: polls){
                    poll.start();
                }

                for(auto& bot: bots){
                    cout << bot.get().getApi().getMe()->username <<  ": silence=" << bot.get().is_silent() << " canary delay=" << bot.get().canaryDelay() << "sec. Cycle#=" << count << endl;
                }
                ++count;
            }
        } catch (exception& e) {
            printf("error: %s\n", e.what());
            this_thread::sleep_for(1s);
        }
    }

    return 0;
}
