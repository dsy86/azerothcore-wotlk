#pragma execution_character_set("UTF-8")
#include "LegendLevel.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Creature.h"
#include "Map.h"
#include "Group.h"
#include "ItemPrototype.h"
#include "DsyMiscMgr.h"

LegendLevelMgr::LegendLevelMgr() { }
LegendLevelMgr::~LegendLevelMgr() { }

uint32 LegendLevelMgr::GetMaxLLevel()
{
    return sTokenMgr->GetTokenTemplate(LEGEND_LEVEL)->maxValue;
}

void LegendLevelMgr::LoadXPperLegendLevelData()
{
    uint32 oldMSTime = getMSTime();

    m_XPperLLevel.resize(GetMaxLLevel());
    for (uint8 level = 0; level < GetMaxLLevel(); ++level)
        m_XPperLLevel[level] = 0;

    //                                                 0    1
    QueryResult result = WorldDatabase.Query("SELECT lvl, xp_for_next_level FROM _legendlevel_xp");

    if (!result)
    {
        sLog->outString(">> Loaded 0 xp for legend level definitions. DB table `_legendlevel_xp` is empty.");
        sLog->outString();
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 current_level = fields[0].GetUInt32();
        uint32 current_xp    = fields[1].GetUInt32();

        if (current_level > GetMaxLLevel())
        {
            sLog->outString("Unused legend level %u in `_legendlevel_xp` table, ignoring.", current_level);
            ++count;                                // make result loading percent "expected" correct in case disabled detail mode for example.
            continue;
        }
        //PlayerXPperLegendLevel
        m_XPperLLevel[current_level] = current_xp;
        ++count;
    } while (result->NextRow());

    // fill level gaps
    for (uint32 level = 1; level < GetMaxLLevel(); ++level)
    {
        if (m_XPperLLevel[level] == 0)
        {
            sLog->outError("Level %i does not have XP for level data. Using data of level [%i] + 100.", level + 1, level);
            m_XPperLLevel[level] = m_XPperLLevel[level - 1] + 100;
        }
    }

    sLog->outString(">> Loaded %u xp for legend level definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void LegendLevelMgr::LoadLLevelBaseStats()
{
    uint32 oldMSTime = getMSTime();
    QueryResult result = WorldDatabase.Query("SELECT legendLevel,ehp,edps FROM _legendlevel_stats_base");

    if (!result)
    {
        sLog->outString(">> Loaded 0 base stats. DB table `_legendlevel_stats_base` is empty.");
        sLog->outString();
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        LLevelBaseStats data;
        data.llevel = fields[0].GetUInt32();
        data.ehp    = fields[1].GetFloat();
        data.edps   = fields[2].GetFloat();
        m_llevelBaseStatsStore[data.llevel] = data;
        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u base stats in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

LLevelBaseStats const * LegendLevelMgr::GetLLevelBaseStats(uint32 llevel)
{
    LLevelBaseStatsContainer::const_iterator it = m_llevelBaseStatsStore.find(llevel);

    if (it != m_llevelBaseStatsStore.end())
        return &(it->second);
    else
        return GetLLevelBaseStats(1);
}

void LegendLevelMgr::LoadLLevelCreatureStats()
{
    uint32 oldMSTime = getMSTime();
    QueryResult result = WorldDatabase.Query("SELECT entry,legendLevel,mapId,hpRate,dpsRate,attackTime,rangeAttackTime,rank FROM _legendlevel_stats_creature");

    if (!result)
    {
        sLog->outString(">> Loaded 0 creature stats. DB table `_legendlevel_stats_creature` is empty.");
        sLog->outString();
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        LLevelCreatureStats data;
        data.entry           = fields[0].GetUInt32();
        data.llevel          = fields[1].GetUInt32();
        data.mapId           = fields[2].GetUInt32();
        data.hpRate          = fields[3].GetFloat();
        data.dpsRate         = fields[4].GetFloat();
        data.attackTime      = fields[5].GetUInt32();
        data.rangeAttackTime = fields[6].GetUInt32();
        data.rank            = fields[7].GetInt32();
        m_llevelCreatureStatsStore[data.entry] = data;
        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u creature stats in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

LLevelCreatureStats const * LegendLevelMgr::GetLLevelCreatureStats(uint32 entry)
{
    LLevelCreatureStatsContainer::const_iterator it = m_llevelCreatureStatsStore.find(entry);

    if (it != m_llevelCreatureStatsStore.end())
        return &(it->second);
    else
        return nullptr;
}

void LegendLevelMgr::LoadLLevelRankStats()
{
    uint32 oldMSTime = getMSTime();
    QueryResult result = WorldDatabase.Query("SELECT rank,hpRate,dpsRate,meleeRate,spellRate FROM _legendlevel_stats_rank");

    if (!result)
    {
        sLog->outString(">> Loaded 0 rank stats. DB table `_legendlevel_stats_rank` is empty.");
        sLog->outString();
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        LLevelRankStats data;
        data.rank      = fields[0].GetUInt32();
        data.hpRate    = fields[1].GetFloat();
        data.dpsRate   = fields[2].GetFloat();
        data.meleeRate = fields[3].GetFloat();
        data.spellRate = fields[4].GetFloat();
        m_llevelRankStatsStore[data.rank] = data;
        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u rank stats in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

LLevelRankStats const * LegendLevelMgr::GetLLevelRankStats(uint32 rank)
{
    LLevelRankStatsContainer::const_iterator it = m_llevelRankStatsStore.find(rank);

    if (it != m_llevelRankStatsStore.end())
        return &(it->second);
    else
        return GetLLevelRankStats(0);
}

void LegendLevelMgr::LoadLLevelMapStats()
{
    uint32 oldMSTime = getMSTime();
    QueryResult result = WorldDatabase.Query("SELECT id,map,difficulty,legendLevel FROM _legendlevel_stats_map");

    if (!result)
    {
        sLog->outString(">> Loaded 0 map stats. DB table `_legendlevel_stats_map` is empty.");
        sLog->outString();
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        LLevelMapStats data;
        data.id         = fields[0].GetUInt32();
        data.map        = fields[1].GetUInt32();
        data.difficulty = fields[2].GetUInt32();
        data.llevel     = fields[3].GetUInt32();
        m_llevelMapStatsStore[data.id] = data;
        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u map stats in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

LLevelMapStats const * LegendLevelMgr::GetLLevelMapStats(uint32 map, uint32 difficulty)
{
    for (LLevelMapStatsContainer::const_iterator it = m_llevelMapStatsStore.begin(); it != m_llevelMapStatsStore.end(); ++it)
    {
        if (it->second.map == map && it->second.difficulty == difficulty)
            return &(it->second);
    }
    if (difficulty > 0)
        GetLLevelMapStats(map, difficulty - 1);

    return nullptr;
}

void LegendLevelMgr::LoadLLevelLevelStats()
{
    uint32 oldMSTime = getMSTime();
    QueryResult result = WorldDatabase.Query("SELECT llevel,strength,agility,stamina,intellect,spirit,attackpower,spellpower,haste,crit,hit FROM _legendlevel_levelstats");

    if (!result)
    {
        sLog->outString(">> Loaded 0 level stats. DB table `_legendlevel_levelstats` is empty.");
        sLog->outString();
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        LLevelLevelStats data;
        data.llevel      = fields[0].GetUInt32();
        data.strength    = fields[1].GetUInt32();
        data.agility     = fields[2].GetUInt32();
        data.stamina     = fields[3].GetUInt32();
        data.intellect   = fields[4].GetUInt32();
        data.spirit      = fields[5].GetUInt32();
        data.attackpower = fields[6].GetUInt32();
        data.spellpower  = fields[7].GetUInt32();
        data.haste       = fields[8].GetUInt32();
        data.crit        = fields[9].GetUInt32();
        data.hit         = fields[10].GetUInt32();
        m_llevelLevelStatsStore[data.llevel] = data;
        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u level stats in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();

}

LLevelLevelStats const * LegendLevelMgr::GetLLevelLevelStats(uint32 llevel)
{
    if (llevel <= 0)
        return nullptr;
    LLevelLevelStatsContainer::const_iterator it = m_llevelLevelStatsStore.find(llevel);

    if (it != m_llevelLevelStatsStore.end())
        return &(it->second);
    else
        return GetLLevelLevelStats(llevel - 1);
}

uint32 LegendLevelMgr::GetCustomRank(Creature * creature)
{
    uint32 entry = creature->GetEntry();
    LLevelCreatureStats const* creatureStats = GetLLevelCreatureStats(entry);
    if (creatureStats != nullptr && creatureStats->rank > -1)
        return creatureStats->rank;
    Map* map = creature->GetMap();
    if (creature->isWorldBoss() && !map->IsRaid())
        return 6; // 世界BOSS
    if (creature->isWorldBoss() && map->IsRaid())
        return 5; // 团队副本BOSS
    if (!creature->isWorldBoss() && creature->isElite() && map->IsRaid())
        return 4; // 团队副本小怪
    if (creature->IsDungeonBoss() && map->IsNonRaidDungeon())
        return 2; // 单人副本BOSS
    if (!creature->IsDungeonBoss() && creature->isElite() && map->IsNonRaidDungeon())
        return 1; // 单人副本小怪
    if (creature->isElite() && !map->IsDungeon())
        return 1; // 野外精英怪
    return 0;
}

void LegendLevelMgr::CalculateStats(Creature* creature, float &hp, float &dmg)
{
    // dsy: init some params
    hp = 1.0f;
    dmg = 1.0f;
    uint32 llevel = 1;
    uint32 rank = GetCustomRank(creature);
    float creatureHpRate = 1.0f;
    float creatureDpsRate = 1.0f;

    // dsy: get llevel from map if map is instance and have custom difficulty
    Map* map = creature->GetMap();
    uint32 diff = map->IsDungeon() ? map->ToInstanceMap()->GetCustomDifficulty() : 0;
    LLevelMapStats const* mapStats = GetLLevelMapStats(map->GetId(), diff);
    if (mapStats != nullptr)
        llevel = mapStats->llevel;

    // dsy: get stats from creature stats table
    //CreatureTemplate const* cinfo = creature->GetCreatureTemplate();
    LLevelCreatureStats const* creatureStats = GetLLevelCreatureStats(creature->GetEntry());
    if (creatureStats != nullptr)
    {
        llevel = creatureStats->llevel > 0 ? creatureStats->llevel : llevel;
        creatureHpRate = creatureStats->hpRate;
        creatureDpsRate = creatureStats->dpsRate;
    }

    // dsy: get base stats using new llevel
    LLevelBaseStats const* baseStats = GetLLevelBaseStats(llevel);

    // dsy: get rank stats using rank in creature table, if not found, using rank=0 data
    LLevelRankStats const* rankStats = GetLLevelRankStats(rank);

    // dsy: calculate final stats. stat = base stat * rank rate * creature rate
    if (baseStats->ehp > 0)
        hp = baseStats->ehp;
    if (rankStats->hpRate > 0.001f)
        hp *= rankStats->hpRate;
    if (creatureHpRate > 0.001f)
        hp *= creatureHpRate;
    hp = hp > 1.0f ? hp : 1.0f;

    if (baseStats->edps > 0)
        dmg = baseStats->edps;
    if (rankStats->dpsRate > 0.001f)
        dmg *= rankStats->dpsRate;
    if (rankStats->meleeRate > 0.001f)
        dmg *= rankStats->meleeRate;
    if (creatureDpsRate > 0.001f)
        dmg *= creatureDpsRate;
    dmg = dmg > 1.0f ? dmg : 1.0f;
}

void Player::LLevelup(int32 value)
{
    if (value == 0)
        value = 1;
    ApplyLegendLevelStats(GetToken(LEGEND_LEVEL), false);
    AddToken(LEGEND_LEVEL, value, "legend level up");
    uint32 nowLLevel = GetToken(LEGEND_LEVEL);
    ApplyLegendLevelStats(nowLLevel, true);
    SendLLevelupPacket(nowLLevel);
    SetToken(LEGEND_EXP, 0, "legend level up set legend exp to zero");
    if (value > 0)
    {
        LLevelLevelStats const* levelStatsNow = sLegendLevelMgr->GetLLevelLevelStats(nowLLevel);
        LLevelLevelStats const* levelStats = sLegendLevelMgr->GetLLevelLevelStats(nowLLevel - value);

        uint32 strdiff = levelStatsNow->strength - levelStats->strength;
        uint32 agidiff = levelStatsNow->agility - levelStats->agility;
        uint32 stadiff = levelStatsNow->stamina - levelStats->stamina;
        uint32 intediff = levelStatsNow->intellect - levelStats->intellect;
        uint32 spidiff = levelStatsNow->spirit - levelStats->spirit;
        uint32 apdiff = levelStatsNow->attackpower - levelStats->attackpower;
        uint32 spdiff = levelStatsNow->spellpower - levelStats->spellpower;
        uint32 hastediff = levelStatsNow->haste - levelStats->haste;
        uint32 critdiff = levelStatsNow->crit - levelStats->crit;
        uint32 hitdiff = levelStatsNow->hit - levelStats->hit;

        string msg = "巅峰等级升级至" + to_string(nowLLevel) + "级！";
        msg += "|r |cff1eff00";
        if (strdiff)
            msg += "\n" + sDsyMiscMgr->StatName(ITEM_MOD_STRENGTH) + " + " + to_string(strdiff);
        if (agidiff)
            msg += "\n" + sDsyMiscMgr->StatName(ITEM_MOD_AGILITY) + " + " + to_string(agidiff);
        if (stadiff)
            msg += "\n" + sDsyMiscMgr->StatName(ITEM_MOD_STAMINA) + " + " + to_string(stadiff);
        if (intediff)
            msg += "\n" + sDsyMiscMgr->StatName(ITEM_MOD_INTELLECT) + " + " + to_string(intediff);
        if (spidiff)
            msg += "\n" + sDsyMiscMgr->StatName(ITEM_MOD_SPIRIT) + " + " + to_string(spidiff);
        if (apdiff)
            msg += "\n" + sDsyMiscMgr->StatName(ITEM_MOD_ATTACK_POWER) + " + " + to_string(apdiff);
        if (spdiff)
            msg += "\n" + sDsyMiscMgr->StatName(ITEM_MOD_SPELL_POWER) + " + " + to_string(spdiff);
        if (hastediff)
            msg += "\n" + sDsyMiscMgr->StatName(ITEM_MOD_HASTE_RATING) + " + " + to_string(hastediff);
        if (critdiff)
            msg += "\n" + sDsyMiscMgr->StatName(ITEM_MOD_CRIT_RATING) + " + " + to_string(critdiff);
        if (hitdiff)
            msg += "\n" + sDsyMiscMgr->StatName(ITEM_MOD_HIT_RATING) + " + " + to_string(hitdiff);

        SendMsgHint(msg);
    }
    // set current level health and mana/energy to maximum after applying all mods.
    SetFullHealth();
    SetPower(POWER_MANA, GetMaxPower(POWER_MANA));
    SetPower(POWER_ENERGY, GetMaxPower(POWER_ENERGY));
    if (GetPower(POWER_RAGE) > GetMaxPower(POWER_RAGE))
        SetPower(POWER_RAGE, GetMaxPower(POWER_RAGE));
    SetPower(POWER_FOCUS, 0);
    SetPower(POWER_HAPPINESS, 0);

    //RefreshName();
    //SendLLevelupPacket(GetToken(LEGEND_LEVEL));
}

void Player::RefreshName()
{
    uint64 guid = GetGUID();
    GlobalPlayerData const* playerData = sWorld->GetGlobalPlayerData(GUID_LOPART(guid));

    WorldPacket data(SMSG_NAME_QUERY_RESPONSE, (8 + 1 + 1 + 1 + 1 + 1 + 10));
    data.appendPackGUID(guid);
    if (!playerData)
    {
        data << uint8(1);                           // name unknown
        sWorld->SendGlobalMessage(&data);
        return;
    }

    string name = playerData->name/* + "·巅峰" + to_string(GetToken(LEGEND_LEVEL)) + "级"*/;

    data << uint8(0);                               // name known
    data << name;                                   // played name
    data << uint8(0);                               // realm name - only set for cross realm interaction (such as Battlegrounds)
    data << uint8(playerData->race);
    data << uint8(playerData->gender);
    data << uint8(playerData->playerClass);

    // pussywizard: optimization
    /*Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(guid);
    if (DeclinedName const* names = (player ? player->GetDeclinedNames() : NULL))
    {
    data << uint8(1);                           // Name is declined
    for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
    data << names->name[i];
    }
    else*/
    data << uint8(0);                           // Name is not declined

    sWorld->SendGlobalMessage(&data);
}

void Player::AddLLevelExp(uint32 value, Unit* victim)
{
    if (value == 0) return;
    SendLogXPGain(value, victim, 0, false);
    uint32 oriLLevel = GetToken(LEGEND_LEVEL);
    uint32 nextLvlXP = sLegendLevelMgr->GetXPforNextLegendLevel(oriLLevel);
    uint32 newXP = GetToken(LEGEND_EXP) + value;
    uint32 newLLevel = oriLLevel;

    while (newXP >= nextLvlXP && newLLevel < sLegendLevelMgr->GetMaxLLevel())
    {
        newXP -= nextLvlXP;

        if (newLLevel < sLegendLevelMgr->GetMaxLLevel())
            newLLevel += 1;

        nextLvlXP = sLegendLevelMgr->GetXPforNextLegendLevel(newLLevel);
    }
    if (newLLevel > oriLLevel)
        LLevelup(newLLevel - oriLLevel);
    SetToken(LEGEND_EXP, newXP, "gained legend exp");
}

void Player::SendLLevelupPacket(uint32 value)
{
    PlayerLevelInfo info;
    sObjectMgr->GetPlayerLevelInfo(getRace(true), getClass(), 80, &info);

    PlayerClassLevelInfo classInfo;
    sObjectMgr->GetPlayerClassLevelInfo(getClass(), 80, &classInfo);

    // send levelup info to client
    WorldPacket data(SMSG_LEVELUP_INFO, (4 + 4 + MAX_POWERS * 4 + MAX_STATS * 4));
    data << uint32(value);
    data << uint32(int32(classInfo.basehealth) - int32(GetCreateHealth()));
    // for (int i = 0; i < MAX_POWERS; ++i)                  // Powers loop (0-6)
    data << uint32(int32(classInfo.basemana) - int32(GetCreateMana()));
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    // end for
    for (uint8 i = STAT_STRENGTH; i < MAX_STATS; ++i)          // Stats loop (0-4)
        data << uint32(int32(info.stats[i]) - GetCreateStat(Stats(i)));

    GetSession()->SendPacket(&data);
}

void Player::LoadDifficulty()
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_DIFFICULTY);
    stmt->setUInt32(0, GetGUIDLow());
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        do {
            Field* fields = result->Fetch();
            m_selectedDifficulty = fields[1].GetUInt32();
        } while (result->NextRow());
    }
}

void Player::SaveDifficulty(SQLTransaction& trans)
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_CHARACTER_DIFFICULTY);
    stmt->setUInt32(0, GetGUIDLow());
    stmt->setUInt32(1, GetSelectedDifficulty());
    trans->Append(stmt);
}

string LegendLevelMgr::DifficultyName(uint32 value)
{
    switch (value)
    {
    case 0:
        return "[普通]";
    case 1:
        return "[普通二]";
    case 2:
        return "[普通三]";
    case 3:
        return "[噩梦一]";
    case 4:
        return "[噩梦二]";
    case 5:
        return "[噩梦三]";
    case 6:
        return "[地狱一]";
    case 7:
        return "[地狱二]";
    case 8:
        return "[地狱三]";
    case 9:
        return "[炼狱一]";
    case 10:
        return "[炼狱二]";
    case 11:
        return "[炼狱三]";
    default:
        return "[无此难度]";
    }

}

void Player::SetSelectedDifficulty(uint32 value)
{
    Group* group = GetGroup();
    if (m_selectedDifficulty == value || (group && group->IsLeader(GetGUID()) && group->GetSelectedDifficulty() == value))
        return;
    if (GetMap()->IsDungeon())
    {
        SendErrorMsgHint("请离开副本后，再选择其他难度的副本");
        return;
    }
    m_selectedDifficulty = value;
    string difficultyName = sLegendLevelMgr->DifficultyName(value);
    SendMsgHint("您选择的副本难度为" + difficultyName);

    if (group)
    {
        string msg2 = "";
        if (group->IsLeader(GetGUID()))
        {
            group->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false, this);
            group->SetSelectedDifficulty(value);
            msg2 = "由于您是队长，因此您的队伍(团队)也将难度切换成" + difficultyName;
        }
        else
            msg2 = "由于您不是队长，您所在队伍(团队)参加的副本难度并没有切换。";
        SendMsgHint(msg2);
    }
    else
    {
        Player::ResetInstances(GetGUIDLow(), INSTANCE_RESET_CHANGE_DIFFICULTY, false);
    }
    SendMsgHint("由于您切换了难度，除团队副本以外的所有副本已被重置");
    return;
}

void Group::SetSelectedDifficulty(uint32 value)
{
    m_selectedDifficulty = value;
    if (!isBGGroup() && !isBFGroup())
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_CUSTOMDIFFICULTY);
        stmt->setUInt32(0, m_selectedDifficulty);
        stmt->setUInt32(1, GetLowGUID());
        CharacterDatabase.Execute(stmt);
    }
}

void Player::ApplyLegendLevelStats(uint32 llevel, bool apply)
{
    if (apply == m_legendLevelApplied)
        return;
    LLevelLevelStats const* levelStats = sLegendLevelMgr->GetLLevelLevelStats(llevel);
    ApplyAddtionStats(ITEM_MOD_STRENGTH, levelStats->strength, apply);
    ApplyAddtionStats(ITEM_MOD_AGILITY, levelStats->agility, apply);
    ApplyAddtionStats(ITEM_MOD_STAMINA, levelStats->stamina, apply);
    ApplyAddtionStats(ITEM_MOD_INTELLECT, levelStats->intellect, apply);
    ApplyAddtionStats(ITEM_MOD_SPIRIT, levelStats->spirit, apply);
    ApplyAddtionStats(ITEM_MOD_ATTACK_POWER, levelStats->attackpower, apply);
    ApplyAddtionStats(ITEM_MOD_SPELL_POWER, levelStats->spellpower, apply);
    ApplyAddtionStats(ITEM_MOD_HASTE_RATING, levelStats->haste, apply);
    ApplyAddtionStats(ITEM_MOD_CRIT_RATING, levelStats->crit, apply);
    ApplyAddtionStats(ITEM_MOD_HIT_RATING, levelStats->hit, apply);
    m_legendLevelApplied = apply;
}
