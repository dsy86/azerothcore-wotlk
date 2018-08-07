#ifndef TRINITY_DSYMISCMGR_H
#define TRINITY_DSYMISCMGR_H
#pragma execution_character_set("UTF-8")

#include "Common.h"
#include "Chat.h"
#include "ItemPrototype.h"
#include "Item.h"
#include "SharedDefines.h"
#include "DBCEnums.h"
#include "Group.h"
#include "Player.h"
#include "Weather.h"
#include "World.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"


class DsyMiscMgr
{
public:
    friend class ACE_Singleton<DsyMiscMgr, ACE_Null_Mutex>;
    std::string GetItemLink(uint32 entry, bool noColor = false);
    std::string GetItemName(uint32 entry);
    std::string ShowImage(std::string name, uint32 width = 24, uint32 height = 24, uint32 x = 0, uint32 y = 0) const;
    std::string goldIcon = ShowImage("MONEYFRAME/UI-GoldIcon", 12, 12);
    std::string FFButton = ShowImage("TIMEMANAGER/FFButton");
    std::string ResetButton = ShowImage("TIMEMANAGER/ResetButton");
    std::string nextButton = ShowImage("BUTTONS/UI-SpellbookIcon-NextPage-Up");
    std::string prevButton = ShowImage("BUTTONS/UI-SpellbookIcon-PrevPage-Up");
    std::string ui_raidframe_arrow = ShowImage("RAIDFRAME/UI-RAIDFRAME-ARROW");
    std::string addButton = ShowImage("GuildBankFrame/UI-GuildBankFrame-NewTab");
    std::string addButton2 = ShowImage("PaperDollInfoFrame/Character-Plus", 12, 12);
    std::string blizzard = ShowImage("CHATFRAME/UI-CHATICON-BLIZZ", 24, 12);

    void LoadDsyTables();
    void LoadDsyPlayerInfos(Player* player);
    void SaveDsyPlayerInfos(Player* player, SQLTransaction& trans);

    string StatName(ItemModType statType)
    {
        switch (statType)
        {
            case ITEM_MOD_AGILITY: return "敏捷";
            case ITEM_MOD_ARMOR_PENETRATION_RATING: return "护甲穿透等级";
            case ITEM_MOD_ATTACK_POWER: return "攻击强度";
            case ITEM_MOD_BLOCK_RATING: return "盾牌格挡等级";
            case ITEM_MOD_CRIT_MELEE_RATING: return "近战爆击等级";
            case ITEM_MOD_CRIT_RANGED_RATING: return "远程爆击等级";
            case ITEM_MOD_CRIT_RATING: return "爆击等级";
            case ITEM_MOD_CRIT_SPELL_RATING: return "法术爆击等级";
            case ITEM_MOD_CRIT_TAKEN_MELEE_RATING: return "近战爆击躲闪等级";
            case ITEM_MOD_CRIT_TAKEN_RANGED_RATING: return "远程爆击躲闪等级";
            case ITEM_MOD_CRIT_TAKEN_RATING: return "爆击躲闪等级";
            case ITEM_MOD_CRIT_TAKEN_SPELL_RATING: return "法术爆击躲闪等级";
            case ITEM_MOD_DEFENSE_SKILL_RATING: return "防御等级";
            case ITEM_MOD_DODGE_RATING: return "躲闪等级";
            case ITEM_MOD_EXPERTISE_RATING: return "精准等级";
            case ITEM_MOD_HASTE_MELEE_RATING: return "近战急速等级";
            case ITEM_MOD_HASTE_RANGED_RATING: return "远程急速等级";
            case ITEM_MOD_HASTE_RATING: return "急速等级";
            case ITEM_MOD_HASTE_SPELL_RATING: return "法术急速等级";
            case ITEM_MOD_HEALTH: return "生命值";
            case ITEM_MOD_HIT_MELEE_RATING: return "近战命中等级";
            case ITEM_MOD_HIT_RANGED_RATING: return "远程命中等级";
            case ITEM_MOD_HIT_RATING: return "命中等级";
            case ITEM_MOD_HIT_SPELL_RATING: return "法术命中等级";
            case ITEM_MOD_HIT_TAKEN_MELEE_RATING: return "近战命中躲闪等级";
            case ITEM_MOD_HIT_TAKEN_RANGED_RATING: return "远程命中躲闪等级";
            case ITEM_MOD_HIT_TAKEN_RATING: return "命中躲闪等级";
            case ITEM_MOD_HIT_TAKEN_SPELL_RATING: return "法术命中躲闪等级";
            case ITEM_MOD_INTELLECT: return "智力";
            case ITEM_MOD_MANA: return "法力值";
            case ITEM_MOD_MANA_REGENERATION: return "每5秒回复法力值";
            case ITEM_MOD_PARRY_RATING: return "招架等级提高";
            case ITEM_MOD_RANGED_ATTACK_POWER: return "远程攻击强度";
            case ITEM_MOD_RESILIENCE_RATING: return "韧性等级";
            case ITEM_MOD_SPELL_DAMAGE_DONE: return "法术和魔法效果的伤害量";
            case ITEM_MOD_SPELL_HEALING_DONE: return "法术和魔法效果的治疗量";
            case ITEM_MOD_SPELL_POWER: return "法术强度";
            case ITEM_MOD_SPIRIT: return "精神";
            case ITEM_MOD_STAMINA: return "耐力";
            case ITEM_MOD_STRENGTH: return "力量";
            default: return "";
        }
        return "";
    }
};

#define sDsyMiscMgr ACE_Singleton<DsyMiscMgr, ACE_Null_Mutex>::instance()

#endif
