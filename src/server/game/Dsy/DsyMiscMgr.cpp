#include "DsyMiscMgr.h"
#include "Token.h"
#include "LegendLevel.h"
#include "Stone.h"

std::string DsyMiscMgr::GetItemLink(uint32 entry, bool noColor/* = false*/)
{
    const ItemTemplate* pItem = sObjectMgr->GetItemTemplate(entry);
    if (!pItem)
        return "";
    std::string name = pItem->Name1;
    if (ItemLocale const *il = sObjectMgr->GetItemLocale(entry))
        ObjectMgr::GetLocaleString(il->Name, LOCALE_zhCN, name);

    std::ostringstream oss;
    if (noColor)
        oss  << "|Hitem:" << entry << ":0:" << "0:0:0:0|h[" << name << "]|h";
    else
        oss << "|c" << std::hex << ItemQualityColors[pItem->Quality] << std::dec << "|Hitem:" << entry << ":0:" << "0:0:0:0|h[" << name << "]|h|r";
    return oss.str();
}


std::string DsyMiscMgr::GetItemName(uint32 entry)
{
    const ItemTemplate* pItem = sObjectMgr->GetItemTemplate(entry);
    if (!pItem)
        return "";
    std::string name = pItem->Name1;
    if (ItemLocale const *il = sObjectMgr->GetItemLocale(entry))
        ObjectMgr::GetLocaleString(il->Name, LOCALE_zhCN, name);
    return "[" + name + "]";
}

void Player::SendMsgHint(std::string msg, bool posstive/* = true*/)
{
    if (msg.length() > 0)
    {
        if (posstive)
            msg = "|cffffff00" + msg + "|r";
        else
            msg = "|cffff0000" + msg + "|r";
        GetSession()->SendAreaTriggerMessage(msg.c_str());
        ChatHandler(GetSession()).SendSysMessage(msg.c_str());
    }
}
void Player::SendErrorMsgHint(std::string msg)
{
    SendMsgHint(msg, false);
}

std::string DsyMiscMgr::ShowImage(std::string name, uint32 width/* = 24*/, uint32 height/* = 24*/, uint32 x/* = 0*/, uint32 y/* = 0*/) const
{
    std::string str = "|TInterface/" + name + ":" + std::to_string(height) + ":" + std::to_string(width) + ":" + std::to_string(x) + ":" + std::to_string(y) + "|t";
    return str;
}

void DsyMiscMgr::LoadDsyTables()
{
    sLog->outString("Loading Legend level base stats Data...");
    sLegendLevelMgr->LoadLLevelBaseStats();

    sLog->outString("Loading Legend level creature stats Data...");
    sLegendLevelMgr->LoadLLevelCreatureStats();

    sLog->outString("Loading Legend level rank stats Data...");
    sLegendLevelMgr->LoadLLevelRankStats();

    sLog->outString("Loading Legend level map stats Data...");
    sLegendLevelMgr->LoadLLevelMapStats();

    sLog->outString("Loading Legend level stats Data...");
    sLegendLevelMgr->LoadLLevelLevelStats();

    sLog->outString("Load Token templates...");
    sTokenMgr->LoadTokenTemplate();

    sLog->outString("Loading Legend level xp Data...");
    sLegendLevelMgr->LoadXPperLegendLevelData();

    sLog->outString("Deleting not exist stone instance...");
    CharacterDatabase.Execute("DELETE FROM _stone_instance WHERE guid NOT IN (SELECT guid FROM item_instance)");

    sLog->outString("Loading Stone level info...");
    sStoneMgr->LoadStoneLevelInfo(false);

    sLog->outString("Loading Stone grade info...");
    sStoneMgr->LoadStoneGradeInfo(false);


}

void DsyMiscMgr::LoadDsyPlayerInfos(Player * player)
{
    player->LoadToken();
    player->LoadDifficulty();
}

void DsyMiscMgr::SaveDsyPlayerInfos(Player * player, SQLTransaction & trans)
{
    // token
    player->SaveToken(trans);
    // difficulty
    player->SaveDifficulty(trans);
}

//void WorldSession::SendListInventory(uint32 creatureEntry, uint32 currencyItemEntry)

// copy from bool Player::BuyItemFromVendor(ObjectGuid vendorGuid, uint32 item, uint8 count, uint8 bag, uint8 slot)
//bool Player::BuySpecialItem(uint32 item, uint8 count, uint32 creatureEntry, uint32 currencyItemEntry)


//copy from void WorldSession::HandleBattlemasterJoinOpcode(WorldPacket & recv_data)
//void Player::JoinBattleGround(BattleGroundTypeId bgTypeId, bool joinAsGroup/* = false*/, uint32 instanceId/* = 0*/)

//void Player::JoinBattleGround(uint32 mapId, bool joinAsGroup/* = false*/, uint32 instanceId/* = 0*/)

void Player::ApplyAddtionStats(ItemModType statType, int32 val, bool apply)
{
    if (val == 0)
        return;

    switch (statType)
    {
    case ITEM_MOD_MANA:
        HandleStatModifier(UNIT_MOD_MANA, BASE_VALUE, float(val), apply);
        break;
    case ITEM_MOD_HEALTH:                           // modify HP
        HandleStatModifier(UNIT_MOD_HEALTH, BASE_VALUE, float(val), apply);
        break;
    case ITEM_MOD_AGILITY:                          // modify agility
        HandleStatModifier(UNIT_MOD_STAT_AGILITY, BASE_VALUE, float(val), apply);
        ApplyStatBuffMod(STAT_AGILITY, float(val), apply);
        break;
    case ITEM_MOD_STRENGTH:                         //modify strength
        HandleStatModifier(UNIT_MOD_STAT_STRENGTH, BASE_VALUE, float(val), apply);
        ApplyStatBuffMod(STAT_STRENGTH, float(val), apply);
        break;
    case ITEM_MOD_INTELLECT:                        //modify intellect
        HandleStatModifier(UNIT_MOD_STAT_INTELLECT, BASE_VALUE, float(val), apply);
        ApplyStatBuffMod(STAT_INTELLECT, float(val), apply);
        break;
    case ITEM_MOD_SPIRIT:                           //modify spirit
        HandleStatModifier(UNIT_MOD_STAT_SPIRIT, BASE_VALUE, float(val), apply);
        ApplyStatBuffMod(STAT_SPIRIT, float(val), apply);
        break;
    case ITEM_MOD_STAMINA:                          //modify stamina
        HandleStatModifier(UNIT_MOD_STAT_STAMINA, BASE_VALUE, float(val), apply);
        ApplyStatBuffMod(STAT_STAMINA, float(val), apply);
        break;
    case ITEM_MOD_DEFENSE_SKILL_RATING:
        ApplyRatingMod(CR_DEFENSE_SKILL, int32(val), apply);
        break;
    case ITEM_MOD_DODGE_RATING:
        ApplyRatingMod(CR_DODGE, int32(val), apply);
        break;
    case ITEM_MOD_PARRY_RATING:
        ApplyRatingMod(CR_PARRY, int32(val), apply);
        break;
    case ITEM_MOD_BLOCK_RATING:
        ApplyRatingMod(CR_BLOCK, int32(val), apply);
        break;
    case ITEM_MOD_HIT_MELEE_RATING:
        ApplyRatingMod(CR_HIT_MELEE, int32(val), apply);
        break;
    case ITEM_MOD_HIT_RANGED_RATING:
        ApplyRatingMod(CR_HIT_RANGED, int32(val), apply);
        break;
    case ITEM_MOD_HIT_SPELL_RATING:
        ApplyRatingMod(CR_HIT_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_CRIT_MELEE_RATING:
        ApplyRatingMod(CR_CRIT_MELEE, int32(val), apply);
        break;
    case ITEM_MOD_CRIT_RANGED_RATING:
        ApplyRatingMod(CR_CRIT_RANGED, int32(val), apply);
        break;
    case ITEM_MOD_CRIT_SPELL_RATING:
        ApplyRatingMod(CR_CRIT_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_HIT_TAKEN_MELEE_RATING:
        ApplyRatingMod(CR_HIT_TAKEN_MELEE, int32(val), apply);
        break;
    case ITEM_MOD_HIT_TAKEN_RANGED_RATING:
        ApplyRatingMod(CR_HIT_TAKEN_RANGED, int32(val), apply);
        break;
    case ITEM_MOD_HIT_TAKEN_SPELL_RATING:
        ApplyRatingMod(CR_HIT_TAKEN_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_CRIT_TAKEN_MELEE_RATING:
        ApplyRatingMod(CR_CRIT_TAKEN_MELEE, int32(val), apply);
        break;
    case ITEM_MOD_CRIT_TAKEN_RANGED_RATING:
        ApplyRatingMod(CR_CRIT_TAKEN_RANGED, int32(val), apply);
        break;
    case ITEM_MOD_CRIT_TAKEN_SPELL_RATING:
        ApplyRatingMod(CR_CRIT_TAKEN_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_HASTE_MELEE_RATING:
        ApplyRatingMod(CR_HASTE_MELEE, int32(val), apply);
        break;
    case ITEM_MOD_HASTE_RANGED_RATING:
        ApplyRatingMod(CR_HASTE_RANGED, int32(val), apply);
        break;
    case ITEM_MOD_HASTE_SPELL_RATING:
        ApplyRatingMod(CR_HASTE_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_HIT_RATING:
        ApplyRatingMod(CR_HIT_MELEE, int32(val), apply);
        ApplyRatingMod(CR_HIT_RANGED, int32(val), apply);
        ApplyRatingMod(CR_HIT_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_CRIT_RATING:
        ApplyRatingMod(CR_CRIT_MELEE, int32(val), apply);
        ApplyRatingMod(CR_CRIT_RANGED, int32(val), apply);
        ApplyRatingMod(CR_CRIT_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_HIT_TAKEN_RATING:
        ApplyRatingMod(CR_HIT_TAKEN_MELEE, int32(val), apply);
        ApplyRatingMod(CR_HIT_TAKEN_RANGED, int32(val), apply);
        ApplyRatingMod(CR_HIT_TAKEN_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_CRIT_TAKEN_RATING:
        ApplyRatingMod(CR_CRIT_TAKEN_MELEE, int32(val), apply);
        ApplyRatingMod(CR_CRIT_TAKEN_RANGED, int32(val), apply);
        ApplyRatingMod(CR_CRIT_TAKEN_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_RESILIENCE_RATING:
        ApplyRatingMod(CR_CRIT_TAKEN_MELEE, int32(val), apply);
        ApplyRatingMod(CR_CRIT_TAKEN_RANGED, int32(val), apply);
        ApplyRatingMod(CR_CRIT_TAKEN_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_HASTE_RATING:
        ApplyRatingMod(CR_HASTE_MELEE, int32(val), apply);
        ApplyRatingMod(CR_HASTE_RANGED, int32(val), apply);
        ApplyRatingMod(CR_HASTE_SPELL, int32(val), apply);
        break;
    case ITEM_MOD_EXPERTISE_RATING:
        ApplyRatingMod(CR_EXPERTISE, int32(val), apply);
        break;
    case ITEM_MOD_ATTACK_POWER:
        HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_VALUE, float(val), apply);
        HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, float(val), apply);
        break;
    case ITEM_MOD_RANGED_ATTACK_POWER:
        HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, float(val), apply);
        break;
        //            case ITEM_MOD_FERAL_ATTACK_POWER:
        //                ApplyFeralAPBonus(int32(val), apply);
        //                break;
    case ITEM_MOD_MANA_REGENERATION:
        ApplyManaRegenBonus(int32(val), apply);
        break;
    case ITEM_MOD_ARMOR_PENETRATION_RATING:
        ApplyRatingMod(CR_ARMOR_PENETRATION, int32(val), apply);
        break;
    case ITEM_MOD_SPELL_POWER:
        ApplySpellPowerBonus(int32(val), apply);
        break;
    case ITEM_MOD_HEALTH_REGEN:
        ApplyHealthRegenBonus(int32(val), apply);
        break;
    case ITEM_MOD_SPELL_PENETRATION:
        ApplySpellPenetrationBonus(val, apply);
        break;
    case ITEM_MOD_BLOCK_VALUE:
        HandleBaseModValue(SHIELD_BLOCK_VALUE, FLAT_MOD, float(val), apply);
        break;
        // deprecated item mods
    case ITEM_MOD_SPELL_HEALING_DONE:
    case ITEM_MOD_SPELL_DAMAGE_DONE:
        break;
    }
}
