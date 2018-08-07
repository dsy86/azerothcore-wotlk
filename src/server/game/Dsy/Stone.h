#ifndef TRINITY_STONE_H
#define TRINITY_STONE_H

#include "Common.h"
#include "ItemPrototype.h"
#include "Item.h"
#include "DsyMiscMgr.h"


struct StoneLevelInfo
{
    uint32 level;
    uint32 reqItem;
    uint32 reqCount;
    uint32 strength;
    uint32 agility;
    uint32 stamina;
    uint32 intellect;
    uint32 spirit;
    uint32 attackpower;
    uint32 spellpower;
    uint32 haste;
    uint32 crit;
    uint32 hit;
};
typedef UNORDERED_MAP<uint32, StoneLevelInfo> StoneLevelInfoMap;

struct StoneGradeInfo
{
    uint32 grade;
    uint32 reqCount;
    float rate[7];
};
typedef UNORDERED_MAP<uint32, StoneGradeInfo> StoneGradeInfoMap;

enum ErrorReason
{
    SUCCEED,
    MAX_LEVEL_OR_GRADE,
    REQUIRE_ITEM,
    NO_REASON
};

class Stone : public Item
{
public:
    Stone() : m_level(1), m_grade(1), m_statsApplied(false) {} 
    bool AddLevel();
    bool AddGrade();
    void SetLevel(uint32 level) 
    { 
        if (m_level == level) return;
        ApplyStoneStats(false);
        m_level = level;
        ApplyStoneStats(true);
        SetState(ITEM_CHANGED);
    }
    void SetGrade(uint32 grade) 
    { 
        if (m_grade == grade) return;
        ApplyStoneStats(false);
        m_grade = grade;
        ApplyStoneStats(true);
        SetState(ITEM_CHANGED);
    }
    const uint32 GetLevel() const { return m_level; }
    const uint32 GetGrade() const { return m_grade; }
    bool CanAddLevel();
    bool CanAddGrade();
    uint32 GetStatValue(ItemModType statType);
    void ApplyStoneStats(bool apply, Player* forplayer = nullptr);
    bool GetStatApplyStatus() const { return m_statsApplied; }

    // DB
    // overwrite virtual Item::SaveToDB
    void SaveToDB(SQLTransaction& trans);
    // overwrite virtual Item::LoadFromDB
    bool LoadFromDB(uint32 guid, uint64 owner_guid, Field* fields, uint32 entry);
    // overwrite virtual Item::DeleteFromDB

protected:
    uint32 m_level;
    uint32 m_grade;
    bool m_statsApplied;
    ErrorReason m_addLevelReason;
    ErrorReason m_addGradeReason;
};

class StoneMgr
{
public:
    friend class ACE_Singleton<StoneMgr, ACE_Null_Mutex>;
    void LoadStoneLevelInfo(bool reload);
    void LoadStoneGradeInfo(bool reload);
    StoneLevelInfo const* GetStoneLevelInfo(uint32 level);
    StoneGradeInfo const* GetStoneGradeInfo(uint32 grade);
    uint32 GetMaxLevel() const { return m_maxLevel; }
    uint32 GetMaxGrade() const { return m_maxGrade; }

private:
    StoneLevelInfoMap m_stoneLevelInfo;
    StoneGradeInfoMap m_stoneGradeInfo;
    uint32 m_maxLevel;
    uint32 m_maxGrade;
};

#define sStoneMgr ACE_Singleton<StoneMgr, ACE_Null_Mutex>::instance()

#endif
