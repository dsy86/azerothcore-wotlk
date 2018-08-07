#ifndef TRINITY_LEGENDLEVEL_H
#define TRINITY_LEGENDLEVEL_H

#include "Common.h"
#include "Player.h"
#include "Token.h"

struct LLevelBaseStats
{
    uint32 llevel;
    float ehp;
    float edps;
};
typedef UNORDERED_MAP<uint32, LLevelBaseStats> LLevelBaseStatsContainer;

struct LLevelCreatureStats
{
    uint32 entry;
    uint32 llevel;
    uint32 mapId;
    float hpRate;
    float dpsRate;
    uint32 attackTime;
    uint32 rangeAttackTime;
    int32 rank;
};
typedef UNORDERED_MAP<uint32, LLevelCreatureStats> LLevelCreatureStatsContainer;

struct LLevelRankStats
{
    uint32 rank;
    float hpRate;
    float dpsRate;
    float meleeRate;
    float spellRate;
};
typedef UNORDERED_MAP<uint32, LLevelRankStats> LLevelRankStatsContainer;

struct LLevelMapStats
{
    uint32 id;
    uint32 map;
    uint32 difficulty;
    uint32 llevel;
};
typedef UNORDERED_MAP<uint32, LLevelMapStats> LLevelMapStatsContainer;

struct LLevelLevelStats
{
    uint32 llevel;
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
typedef UNORDERED_MAP<uint32, LLevelLevelStats> LLevelLevelStatsContainer;

class LegendLevelMgr
{
private:
    LegendLevelMgr();
    ~LegendLevelMgr();

    typedef std::vector<uint32> XPperLLevel;       // [level]
    XPperLLevel m_XPperLLevel;

    LLevelBaseStatsContainer m_llevelBaseStatsStore;
    LLevelCreatureStatsContainer m_llevelCreatureStatsStore;
    LLevelRankStatsContainer m_llevelRankStatsStore;
    LLevelMapStatsContainer m_llevelMapStatsStore;
    LLevelLevelStatsContainer m_llevelLevelStatsStore;

public:
    friend class ACE_Singleton<LegendLevelMgr, ACE_Null_Mutex>;
    uint32 GetMaxLLevel();
    void LoadXPperLegendLevelData();
    uint32 GetXPforNextLegendLevel(uint32 level)
    {
        if (level >= GetMaxLLevel()) return 0;
        return m_XPperLLevel[level];
    }
    void LoadLLevelBaseStats();
    LLevelBaseStats const* GetLLevelBaseStats(uint32 llevel);
    void LoadLLevelCreatureStats();
    LLevelCreatureStats const* GetLLevelCreatureStats(uint32 entry);
    void LoadLLevelRankStats();
    LLevelRankStats const* GetLLevelRankStats(uint32 rank);
    void LoadLLevelMapStats();
    LLevelMapStats const* GetLLevelMapStats(uint32 map, uint32 difficulty);
    void LoadLLevelLevelStats();
    LLevelLevelStats const* GetLLevelLevelStats(uint32 llevel);
    uint32 GetCustomRank(Creature* creature);
    void CalculateStats(Creature* creature, float &hp, float &dmg);
    string DifficultyName(uint32 value);
};

#define sLegendLevelMgr ACE_Singleton<LegendLevelMgr, ACE_Null_Mutex>::instance()

#endif
