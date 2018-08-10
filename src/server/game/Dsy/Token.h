#ifndef TRINITY_TOEKN_H
#define TRINITY_TOEKN_H

#include "Common.h"
#include "Player.h"

struct TokenTemplate
{
    uint32 id;
    bool charOrAccount;
    string name;
    string desc;
    uint32 minValue;
    uint32 defaultValue;
    uint32 maxValue;
};

typedef UNORDERED_MAP<uint32, TokenTemplate> TokenTemplateStore;

struct AddTokenItems
{
    uint32 entry;
    uint32 tokenType;
    uint32 addValue;
};

typedef UNORDERED_MAP<uint32, AddTokenItems> AddTokenItemsContainer;

enum TokenName
{
    DIAMOND      = 1,
    TOKEN_POINT  = 2,
    VIP_LEVEL    = 3,
    LEGEND_LEVEL = 4,
    LEGEND_EXP   = 5,
    RANK_LEVEL   = 6,
    ARENA_LEVEL  = 7,
    VIP_EXP      = 8

};

class Token
{
public:
    Token::Token(uint32 id = 0);
    void LoadFromDB(Field* fields, Player* player);
    uint64 GetPlayerGUID() const;
    Player* GetPlayer() const;
    void SetPlayerGUID(uint64 guid);
    uint32 GetId() const;
    string Name();
    string Desc();
    uint32 Get() const;
    uint32 GetMin();
    uint32 GetDefault();
    uint32 GetMax();
    bool IsAccountToken();
    void Set(uint32 value, string reason = "");
    void Add(int32 value, string reason = "");
    void Remove(uint32 value, string reason = "");
    TokenTemplate const* GetTemplate();

private:
    uint32 m_id;
    uint32 m_value;
    uint64 m_playerGuid;
};

class TokenMgr
{
public:
    friend class ACE_Singleton<TokenMgr, ACE_Null_Mutex>;
    void LoadTokenTemplate();
    void LoadItemAddTokenTable();
    TokenTemplate const* GetTokenTemplate(uint32 entry)
    {
        return &m_tokenTemplateStore[entry];
    }
    AddTokenItems const* GetAddTokenItems(uint32 entry);
    TokenTemplateStore const GetAllTokens()
    {
        return m_tokenTemplateStore;
    }

private:
    TokenTemplateStore m_tokenTemplateStore;
    AddTokenItemsContainer m_addTokenItemsStore;
};

#define sTokenMgr ACE_Singleton<TokenMgr, ACE_Null_Mutex>::instance()

#endif
