#ifndef UTILS_H
#define UTILS_H

void get_token(const std::string& pathToken, std::string& token){

    std::ifstream f(pathToken);
    std::ostringstream ss;
    ss << f.rdbuf();
    token = ss.str();
}

std::string select_random_line(std::vector<std::string> & lines){
    std::random_device device;
    std::default_random_engine engine(device());
    std::shuffle(begin(lines), end(lines), engine);

    return lines[0];
}

void make_lower(std::string& str){
    std::transform(str.begin(), str.end(), str.begin(),
    [](unsigned char c){ return isalpha(c) ? std::tolower(c) : c; });
}

void file_write_line(const std::string& file, const std::string& str){

    auto str_lower = str;
    make_lower(str_lower);

    replace(str_lower.begin(), str_lower.end(), '\n', ' ');

    std::ofstream f;
    f.open(file, std::ios::out | std::ios::app);

    if (!f) {
        std::string error {"Unable to open file " + file};
        std::cerr << error;
        throw runtime_error(error);
    }

    printf("File newline: %s\n", str_lower.c_str());
    f << str_lower << std::endl;
}

void file_clear_line(const std::string& file, const std::string& str, const std::string& temp = "temp.txt"){

    std::string strNew = "";
    std::ifstream filein(file); //File to read from
    std::ofstream fileout(temp); //Temporary file
    if(!filein)
    {
        std::string error {"Unable to open file " + file};
        cerr << error;
        throw runtime_error(error);
    }

    if(!fileout)
    {
        std::string error {"Unable to open file " + temp};
        cerr << error;
        throw std::runtime_error(error);
    }

    std::string strTemp;
    while(getline(filein, strTemp))
    {
        printf("Process line: %s\n", strTemp.c_str());

        if(strTemp == str){
            printf("Found match line: %s\n", str.c_str());
            strTemp = strNew;
        }
        if(!strTemp.empty())
            strTemp += "\n";
        
        fileout << strTemp;
    }
    filein.close();
    fileout.close();

    std::ifstream  src(temp, std::ios::binary);
    std::ofstream  dst(file,   std::ios::binary);
    dst << src.rdbuf();
}

bool is_secured_chat(const std::string& str){

    for(const auto& el: trusted_chats_titles){        
        if(str == el){
            std::cout << "Chat " << str << " is verified and trusted" << std::endl;
            return true;
        }
    }

    return false;
}

bool find_partial(const std::string& request, std::string& author, std::string& DBline){

    if(request.empty()) 
        return false;

    auto str_lower = request;
    make_lower(str_lower);

    std::ifstream myfile(DBfile);

    if(myfile.is_open())
    {
        const size_t token_min_size_bytes {8};
        const size_t tokens_min_match_amt {2};
        
        std::vector<std::string> requestTokens;
        std::stringstream   mySstream(str_lower);
        std::string         tempStr;
        while( getline( mySstream, tempStr, ' ' ) ) {            
            if(tempStr.size() >= token_min_size_bytes)
                requestTokens.push_back( tempStr );
        }
        sort(requestTokens.begin(), requestTokens.end());
        requestTokens.erase(unique(requestTokens.begin(), requestTokens.end()), requestTokens.end());

        unsigned int curLine = 0;
        while(getline(myfile, DBline)) { 

            curLine++;

            if(DBline.empty() || DBline.at(0) == '\n'){
                continue;
            }

            std::vector<std::string> DBlineTokens;
            std::stringstream   mySstream(DBline);
            while( getline( mySstream, tempStr, ' ' ) ) {
                if(tempStr.size() >= token_min_size_bytes)
                    DBlineTokens.push_back( tempStr );
            }
            author = DBlineTokens[0];
            sort(DBlineTokens.begin(), DBlineTokens.end());
            DBlineTokens.erase(unique(DBlineTokens.begin(), DBlineTokens.end()), DBlineTokens.end());

            size_t countRequestTokens{0};
            for(auto& el: requestTokens){
                // cout << "process: " << el << endl; 
                auto it = find(DBlineTokens.cbegin(), DBlineTokens.cend(), el);
                if(it != DBlineTokens.cend()){

                    ++countRequestTokens;
                    std::cout << "found: " << el << " in line: " << curLine << std::endl; 
                    if(countRequestTokens >= tokens_min_match_amt){
                        myfile.close();
                        return true;
                    }
                }
            }
        }
 
        myfile.close();
        return false;
    }
    else {
        std::cerr<<"Unable to open file";
        throw runtime_error("DB file is not ready.");
    }
}

std::string get_timestamp(){

    std::string output(25,'\0');
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    strcpy((char*)output.data(), asctime(timeinfo));
    std::cout << output;

    return output;
}

template<typename BotManagerType>
void canary_call(const BotVerbose<BotManagerType, subprocess::popen>& bot) {
    while (true) {
        const std::string message {string(". mode: canary singing") + " delay=" + to_string(bot.canaryDelay()) + " sec."};
        std::cout << bot.getName() <<  message << std::endl;
        bot.sayWord(message);

        std::ifstream file(reminderFile);
        std::ostringstream ss;
        ss << file.rdbuf();
        bot.sayWord("Текущие напоминания: \n" + ss.str());

        for(auto i=0;i<bot.canaryDelay();++i){
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

string get_username(Message::Ptr message){
    auto username = message->from->username;
    if(username.empty()){
        username.append(message->from->firstName + "_" + message->from->lastName);
    }

    return username;
}

void prepare_hexagramms(std::vector<std::string>& hexagramms){

    std::ifstream file(hexagrammsFile);

    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            hexagramms.push_back(line); 
        }
        file.close(); 
    } else {
        std::cerr << "Unable to open file: " << hexagrammsFile << std::endl;
        throw runtime_error("hexagramms file is not ready."); 
    }
}

bool is_command(const std::string& text){

     return StringTools::startsWith(text,  "/stat") ||
            StringTools::startsWith(text,  "/llm1") ||
            StringTools::startsWith(text,  "/llm2") ||
            StringTools::startsWith(text,  "/llm3") ||
            StringTools::startsWith(text,  "/llm4")||
            StringTools::startsWith(text,  "/llm5") ;
}

 void setup_paths(std::string& pathPrefix, std::string& llamaOutput, std::string& hexagrammsFile, std::string& DBfile, std::string& reminderFile){
    const auto cwd = filesystem::current_path();
    const std::string projects_prefix {cwd.string() + "/../../"}; 
    pathPrefix = projects_prefix + "mytgbot/"; 
    llamaOutput = projects_prefix + "llama.cpp/output.txt"; //one should place it's own or provide Projects/llama.cpp placement
    hexagrammsFile = pathPrefix + "hexagramms.txt";//just for fun- bot quotes ones when request storage DB matches
    DBfile = pathPrefix + "messages.txt"; //chat all people messages storage
    reminderFile = pathPrefix + "reminder.txt";//reminder storage
 }

#endif