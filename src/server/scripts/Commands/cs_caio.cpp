/* ScriptData
Name: caio_commandscript
%Complete : 100
Comment : All AIO related server side commands
Category : commandscripts
EndScriptData */

#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "Language.h"

class caio_commandscript : public CommandScript
{
    public:
        caio_commandscript() : CommandScript("caio_commandscript") { }

        std::vector<ChatCommand> GetCommands() const override
        {
            static std::vector<ChatCommand> caioCommandTable =
            {
                { "version",        SEC_ADMINISTRATOR, true, &HandleVersionCommand,      ""},
                { "send",           SEC_ADMINISTRATOR, true, &HandleSendCommand,         ""},
                { "forcereload",    SEC_ADMINISTRATOR, true, &HandleReloadCommand,       ""},
                { "forcereset",     SEC_ADMINISTRATOR, true, &HandleResetCommand,        ""},
                { "sendall",        SEC_ADMINISTRATOR, true, &HandleSendAllCommand,      ""},
                { "forcereloadall", SEC_ADMINISTRATOR, true, &HandleReloadAllCommand,    ""},
                { "forceresetall",  SEC_ADMINISTRATOR, true, &HandleResetAllCommand,     ""},
                { "reloadaddons",   SEC_ADMINISTRATOR, true, &HandleReloadAddonsCommand, ""},
                { "addaddon",       SEC_ADMINISTRATOR, true, &HandleAddAddonCommand,     ""},
                { "removeaddon",    SEC_ADMINISTRATOR, true, &HandleRemoveAddonCommand,  ""}
            };
            static std::vector<ChatCommand> commandTable =
            {
                { "caio",           SEC_ADMINISTRATOR, true, nullptr,                    "", caioCommandTable }
            };
            return commandTable;
        }
        
        static bool HandleVersionCommand(ChatHandler* handler, char const* args)
        {
            handler->PSendSysMessage("AIO version %f.", AIO_VERSION);
            return true;
        }

        static bool HandleSendCommand(ChatHandler* handler, char const* args)
        {
            //Player name
            Player* target;
            if(!handler->extractPlayerTarget((char*)args, &target))
                return false;

            //Quoted message
            char* tailStr = strtok(NULL, "");
            if(!tailStr)
                return false;

            char* msg = handler->extractQuotedArg(tailStr);
            if(!msg)
                return false;

            target->SendSimpleAIOMessage(msg);
            handler->PSendSysMessage(LANG_SENDMESSAGE, target->GetName().c_str(), msg);
            return true;
        };

        static bool HandleReloadCommand(ChatHandler* handler, char const* args)
        {
            Player *target = 0;
            if(!handler->extractPlayerTarget((char*)args, &target, 0, 0))
                return false;

            target->ForceReloadAddons();
            handler->PSendSysMessage(LANG_CAIO_FORCERELOAD_SENT, target->GetName().c_str());
            return true;
        };

        static bool HandleResetCommand(ChatHandler* handler, char const* args)
        {
            Player *target = 0;
            if(!handler->extractPlayerTarget((char*)args, &target, 0, 0))
                return false;

            target->ForceResetAddons();
            handler->PSendSysMessage(LANG_CAIO_FORCERESET_SENT, target->GetName().c_str());
            return true;
        };

        static bool HandleSendAllCommand(ChatHandler* handler, char const* args)
        {
            if(!*args)
                return false;
            
            //Get message
            char *msg = handler->extractQuotedArg((char*)args);
            if(!msg)
                return false;

            sWorld->SendAllSimpleAIOMessage(msg);
            handler->PSendSysMessage(LANG_SENDMESSAGE, "all players", msg);
            return true;
        };

        static bool HandleReloadAllCommand(ChatHandler* handler, char const* args)
        {
            //Force reload required players
            sWorld->ForceReloadPlayerAddons();
            handler->PSendSysMessage(LANG_CAIO_FORCERELOAD_SENT, "all players");
            return true;
        };

        static bool HandleResetAllCommand(ChatHandler* handler, char const* args)
        {
            //Force reset required players
            sWorld->ForceResetPlayerAddons();
            handler->PSendSysMessage(LANG_CAIO_FORCERESET_SENT, "all players");
            return true;
        };

        static bool HandleReloadAddonsCommand(ChatHandler* handler, char const* args)
        {

            //Reload and force reload required players
            bool success = sWorld->ReloadAddons();
            if(success)
            {
                sWorld->ForceReloadPlayerAddons();
            }
            else
            {
                handler->SendSysMessage(LANG_CAIO_RELOADADDONS_ERROR);
            }
            return true;
        }

        static bool HandleAddAddonCommand(ChatHandler* handler, char const* args)
        {
            if(!*args)
                return false;

            //Addon name
            char *addonName = strtok((char*)args, " ");
            if(!addonName || addonName[0] == '"')
                return false;

            //File
            char *tailStr = strtok(NULL, "");
            char *addonFile = handler->extractQuotedArg(tailStr);
            if(!addonFile)
                return false;

            //Add
            World::AIOAddon newAddon(addonName, addonFile);
            bool added = sWorld->AddAddon(newAddon);
            if(added)
            {
                sWorld->ForceReloadPlayerAddons();
            }
            else
            {
                handler->PSendSysMessage(LANG_CAIO_ADDADDON_ERROR, addonName);
            }
            return true;
        }

        static bool HandleRemoveAddonCommand(ChatHandler* handler, char const* args)
        {
            if(!*args)
                return false;

            //Remove and reload required players
            sWorld->RemoveAddon(args);
            sWorld->ForceReloadPlayerAddons();
            return true;
        }
};

void AddSC_caio_commandscript()
{
    new caio_commandscript();
}