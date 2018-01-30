#include "Configuration/Config.h"
#include "Player.h"
#include "Creature.h"
#include "AccountMgr.h"
#include "ScriptedAI\ScriptedCreature.h"
#include "ScriptMgr.h"
#include "Define.h"
#include "GossipDef.h"
#include "Pet.h"

uint32 SUMMON_CHEST;
uint32 KillAnnounce;
bool spawnchestIP;
uint32 chest_despawn;
std::vector<uint32> MapstoIgnore = { 489, 592, 30, 566, 607, 628, 562, 618, 617, 559, 572 };
std::vector<uint32> AreatoIgnore = { 1741/*Gurubashi*/, 2177 };

class PvPScript : public PlayerScript
{
public:
    PvPScript() :   PlayerScript("PvPScript") {}

    void OnPlayerKilledByCreature(Creature* killer, Player* killed)
    {
        if (Player* player = killer->GetAffectingPlayer())
            OnPVPKill(player, killed);
    }

    void OnPVPKill(Player* killer, Player* killed)
    {
        if (!sConfigMgr->GetBoolDefault("PvPChest", true))
            return;
        //相同IP不掉
        if (spawnchestIP)
            if (killer->GetSession()->GetRemoteAddress() == killed->GetSession()->GetRemoteAddress())
                return;

        // 有虚弱buff不掉
        if (killed->HasAura(15007))
            return;

        // 不在战场不掉
        if (!killer->InBattleground())
            return;

        // 战场没在进行中的不掉
        if (killer->InBattleground() && killer->GetBattleground()->GetStatus() != STATUS_IN_PROGRESS)
            return;

        if (GameObject* go = killer->SummonGameObject(SUMMON_CHEST, killed->GetPositionX(), killed->GetPositionY(), killed->GetPositionZ(), killed->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, chest_despawn, false))
        {
            killer->AddGameObject(go);
            go->SetOwnerGUID(NULL); //This is so killed players can also loot the chest

            for (int i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
                if (Item* pItem = killed->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                {
                    uint8 slot = pItem->GetSlot();
                    LootStoreItem storeItem = LootStoreItem(pItem->GetEntry(), 100, LOOT_MODE_DEFAULT, 0, 1, 1);
                    go->loot.AddItem(storeItem);
                    killed->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
                }
        }
    }
};

class PvPScript_conf : public WorldScript
{
public:
    PvPScript_conf() : WorldScript("PvPScriptConf") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string cfg_file = "PvPScript.conf";
            std::string cfg_def_file = cfg_file + ".dist";

            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());

            SUMMON_CHEST = sConfigMgr->GetIntDefault("ChestID", 179697);
            chest_despawn = sConfigMgr->GetIntDefault("ChestTimer", 120);
            spawnchestIP = sConfigMgr->GetBoolDefault("spawnchestIP", true);
        }
    }
};

void AddPvPScripts()
{
    new PvPScript();
    new PvPScript_conf();
}