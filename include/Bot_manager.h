#ifndef BOT_MANAGER_H
#define BOT_MANAGER_H

class BotManager{
    public:
        BotManager(
        std::shared_ptr<Bot> bot): 
        m_bot(bot), 
        m_poll(*m_bot){
        }

        virtual ~BotManager() = default;
        virtual bool deleteWebhook() {        
            return m_bot->getApi().deleteWebhook();
        };

        virtual void sendMessage(std::int64_t id, const string& message) const {        
            m_bot->getApi().sendMessage(id, message);
        };

        virtual void sendChatAction(std::int64_t id, const string& action) const {        
            m_bot->getApi().sendChatAction(id, action);
        };

        virtual const Api& getApi() {        
            return m_bot->getApi();
        };

        virtual EventBroadcaster& getEvents() {        
            return m_bot->getEvents();
        };

        virtual void startPoll() {        
            m_poll.start();
        };


    private:
        std::shared_ptr<Bot> m_bot;
        TgLongPoll           m_poll;
};
#endif