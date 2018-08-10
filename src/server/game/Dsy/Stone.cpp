#pragma execution_character_set("UTF-8")
#include "Stone.h"
#include "Player.h"


void StoneMgr::LoadStoneLevelInfo(bool reload)
{
    uint32 oldMSTime = getMSTime();
    m_stoneLevelInfo.clear();
    //                                               0     1        2         3        4       5       6         7      8           9          10    11   12
    QueryResult result = WorldDatabase.Query("SELECT level,req_item,req_count,strength,agility,stamina,intellect,spirit,attackpower,spellpower,haste,crit,hit FROM _stone_level_template");

    if (!result)
    {
        sLog->outString(">> Loaded 0 stone level templates. DB table `_stone_level_template` is empty.");
        sLog->outString();
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        StoneLevelInfo data;
        uint32 level     = fields[0].GetUInt32();
        data.level       = level;
        data.reqItem     = fields[1].GetUInt32();
        data.reqCount    = fields[2].GetUInt32();
        data.strength    = fields[3].GetUInt32();
        data.agility     = fields[4].GetUInt32();
        data.stamina     = fields[5].GetUInt32();
        data.intellect   = fields[6].GetUInt32();
        data.spirit      = fields[7].GetUInt32();
        data.attackpower = fields[8].GetUInt32();
        data.spellpower  = fields[9].GetUInt32();
        data.haste       = fields[10].GetUInt32();
        data.crit        = fields[11].GetUInt32();
        data.hit         = fields[12].GetUInt32();
        m_stoneLevelInfo[level] = data;
        m_maxLevel = level;
        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u stone level templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

void StoneMgr::LoadStoneGradeInfo(bool reload)
{
    uint32 oldMSTime = getMSTime();
    m_stoneGradeInfo.clear();
    //                                               0     1         2        3        4        5        6        7        8
    QueryResult result = WorldDatabase.Query("SELECT grade,req_count,quality0,quality1,quality2,quality3,quality4,quality5,quality6 FROM _stone_grade_template");

    if (!result)
    {
        sLog->outString(">> Loaded 0 stone grade templates. DB table `_stone_grade_template` is empty.");
        sLog->outString();
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        StoneGradeInfo data;

        uint32 grade  = fields[0].GetUInt32();
        data.grade    = grade;
        data.reqCount = fields[1].GetUInt32();
        data.rate[0]  = fields[2].GetFloat();
        data.rate[1]  = fields[3].GetFloat();
        data.rate[2]  = fields[4].GetFloat();
        data.rate[3]  = fields[5].GetFloat();
        data.rate[4]  = fields[6].GetFloat();
        data.rate[5]  = fields[7].GetFloat();
        data.rate[6]  = fields[8].GetFloat();
        m_stoneGradeInfo[grade] = data;
        m_maxGrade = grade;
        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u stone grade templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

bool Stone::CanAddLevel()
{
    StoneLevelInfo const* levelInfo = sStoneMgr->GetStoneLevelInfo(m_level);
    if (m_level >= sStoneMgr->GetMaxLevel())
    {
        m_addLevelReason = MAX_LEVEL_OR_GRADE;
        return false;
    }
    if (!GetOwner() || GetOwner()->GetTypeId() != TYPEID_PLAYER)
    {
        m_addLevelReason = NO_REASON;
        return false;
    }
    if (!GetOwner()->HasItemCount(levelInfo->reqItem, levelInfo->reqCount))
    {
        m_addLevelReason = REQUIRE_ITEM;
        return false;
    }
    m_addLevelReason = SUCCEED;
    return true;
}

bool Stone::CanAddGrade()
{
    StoneGradeInfo const* gradeInfo = sStoneMgr->GetStoneGradeInfo(m_grade);
    if (m_grade >= sStoneMgr->GetMaxGrade())
    {
        m_addGradeReason = MAX_LEVEL_OR_GRADE;
        return false;
    }
    if (!GetOwner() || GetOwner()->GetTypeId() != TYPEID_PLAYER)
    {
        m_addGradeReason = NO_REASON;
        return false;
    }
    if (!GetOwner()->HasItemCount(GetEntry(), (gradeInfo->reqCount + 1))) //加1是为了别把本体扣了
    {
        m_addGradeReason = REQUIRE_ITEM;
        return false;
    }
    m_addGradeReason = SUCCEED;
    return true;
}

bool Stone::AddLevel()
{
    if (!GetOwner() || GetOwner()->GetTypeId() != TYPEID_PLAYER) return false;
    Player *owner = GetOwner();
    if (!owner) return false;
    if (CanAddLevel())
    {
        StoneLevelInfo const* levelInfo = sStoneMgr->GetStoneLevelInfo(m_level);
        owner->DestroyItemCount(levelInfo->reqItem, levelInfo->reqCount, true);
        SetLevel(m_level + 1);
        owner->SendMsgHint("护身符升级成功");
        return true;
    }
    switch (m_addLevelReason)
    {
        case MAX_LEVEL_OR_GRADE:
            owner->SendErrorMsgHint("护身符已经升级至顶级，无法继续升级");
            break;
        case REQUIRE_ITEM:
            owner->SendErrorMsgHint("材料不足，无法升级");
            break;
        case SUCCEED:
        case NO_REASON:
        default:
            break;
    }
    return false;
}

bool Stone::AddGrade()
{
    if (!GetOwner() || GetOwner()->GetTypeId() != TYPEID_PLAYER) return false;
    Player *owner = GetOwner();
    if (!owner) return false;
    if (CanAddGrade())
    {
        StoneGradeInfo const* gradeInfo = sStoneMgr->GetStoneGradeInfo(m_grade);
        owner->DestroyItemCount(GetEntry(), gradeInfo->reqCount, true);
        SetGrade(m_grade + 1);
        owner->SendMsgHint("护身符升阶成功");
        return true;
    }
    switch (m_addGradeReason)
    {
        case MAX_LEVEL_OR_GRADE:
            owner->SendErrorMsgHint("护身符已经升阶到头了，无法继续升阶");
            break;
        case REQUIRE_ITEM:
            owner->SendErrorMsgHint("材料不足，无法升阶");
            break;
        case SUCCEED:
        case NO_REASON:
        default:
            break;
    }
    return false;
}

void Stone::SetLevel(uint32 level)
{
    if (m_level == level) return;
    ApplyStoneStats(false);
    m_level = level;
    ApplyStoneStats(true);
    SetState(ITEM_CHANGED);
}

void Stone::SetGrade(uint32 grade)
{
    if (m_grade == grade) return;
    ApplyStoneStats(false);
    m_grade = grade;
    ApplyStoneStats(true);
    SetState(ITEM_CHANGED);
}

uint32 Stone::GetStatValue(ItemModType statType)
{
    float value = 0;
	StoneLevelInfo const* levelInfo = sStoneMgr->GetStoneLevelInfo(m_level);
	StoneGradeInfo const* gradeInfo = sStoneMgr->GetStoneGradeInfo(m_grade);
    uint32 quality = GetTemplate()->Quality > 6 ? 0 : GetTemplate()->Quality;
    switch (statType)
    {
    case ITEM_MOD_STRENGTH:                         //modify strength
        value = levelInfo->strength;
        break;
    case ITEM_MOD_AGILITY:                          // modify agility
        value = levelInfo->agility;
        break;
    case ITEM_MOD_STAMINA:                           // modify stamina
        value = levelInfo->stamina;
        break;
    case ITEM_MOD_INTELLECT:                        //modify intellect
        value = levelInfo->intellect;
        break;
    case ITEM_MOD_SPIRIT:                           //modify spirit
        value = levelInfo->spirit;
        break;
    case ITEM_MOD_ATTACK_POWER:                          //modify attackpower
        value = levelInfo->attackpower;
        break;
    case ITEM_MOD_SPELL_POWER:                          //modify spellpower
        value = levelInfo->spellpower;
        break;
    case ITEM_MOD_HASTE_RATING:                          //modify haste
        value = levelInfo->haste;
        break;
    case ITEM_MOD_CRIT_RATING:                          //modify crit
        value = levelInfo->crit;
        break;
    case ITEM_MOD_HIT_RATING:                          //modify hit
        value = levelInfo->hit;
        break;
    default:
        break;
    }
    return static_cast<uint32>(value * gradeInfo->rate[quality]);
}

void Stone::ApplyStoneStats(bool apply, Player* forplayer/* = nullptr*/)
{
    if (apply == m_statsApplied) return; 
    if (!forplayer)
    {
        if (!GetOwner() || GetOwner()->GetTypeId() != TYPEID_PLAYER) return;
        forplayer = GetOwner();
    }
    if (!forplayer->IsInWorld()) return;

    forplayer->ApplyAddtionStats(ITEM_MOD_STRENGTH, GetStatValue(ITEM_MOD_STRENGTH), apply);
    forplayer->ApplyAddtionStats(ITEM_MOD_AGILITY, GetStatValue(ITEM_MOD_AGILITY), apply);
    forplayer->ApplyAddtionStats(ITEM_MOD_STAMINA, GetStatValue(ITEM_MOD_STAMINA), apply);
    forplayer->ApplyAddtionStats(ITEM_MOD_INTELLECT, GetStatValue(ITEM_MOD_INTELLECT), apply);
    forplayer->ApplyAddtionStats(ITEM_MOD_SPIRIT, GetStatValue(ITEM_MOD_SPIRIT), apply);
    forplayer->ApplyAddtionStats(ITEM_MOD_ATTACK_POWER, GetStatValue(ITEM_MOD_ATTACK_POWER), apply);
    forplayer->ApplyAddtionStats(ITEM_MOD_SPELL_POWER, GetStatValue(ITEM_MOD_SPELL_POWER), apply);
    forplayer->ApplyAddtionStats(ITEM_MOD_HASTE_RATING, GetStatValue(ITEM_MOD_HASTE_RATING), apply);
    forplayer->ApplyAddtionStats(ITEM_MOD_CRIT_RATING, GetStatValue(ITEM_MOD_CRIT_RATING), apply);
    forplayer->ApplyAddtionStats(ITEM_MOD_HIT_RATING, GetStatValue(ITEM_MOD_HIT_RATING), apply);
    m_statsApplied = apply;
}

bool Stone::LoadFromDB(uint32 guid, uint64 owner_guid, Field* fields, uint32 entry)
{
    if (!Item::LoadFromDB(guid, owner_guid, fields, entry))
        return false;

    SetLevel(fields[15].GetUInt32());
    SetGrade(fields[16].GetUInt32());
    //ApplyStoneStats(true); //在player那已经apply过了
    return true;
}

void Stone::SaveToDB(SQLTransaction& trans)
{
    Item::SaveToDB(trans);
}

StoneLevelInfo const* StoneMgr::GetStoneLevelInfo(uint32 level)
{
    StoneLevelInfoMap::const_iterator it = m_stoneLevelInfo.find(level);

    if (it != m_stoneLevelInfo.end())
        return &(it->second);

    if (level > 1)
        return GetStoneLevelInfo(level - 1);

    return GetStoneLevelInfo(1);
}

StoneGradeInfo const* StoneMgr::GetStoneGradeInfo(uint32 grade)
{
    StoneGradeInfoMap::const_iterator it = m_stoneGradeInfo.find(grade);

    if (it != m_stoneGradeInfo.end())
        return &(it->second);

    if (grade > 1)
        return GetStoneGradeInfo(grade - 1);

    return GetStoneGradeInfo(1);
}
