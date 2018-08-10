#include "Token.h"

Token::Token(uint32 id) :m_id(id), m_value(0), m_playerGuid(0)
{
    m_id = id;
    if (id <= 0)
        return;

    const TokenTemplate *tokenTemplate = sTokenMgr->GetTokenTemplate(id);
    if (tokenTemplate == nullptr)
        return;

    m_id = id;
    m_value = tokenTemplate->defaultValue;
    m_playerGuid = 0;
};

void Token::LoadFromDB(Field* fields, Player* player)
{

    const TokenTemplate *tokenTempalte = sTokenMgr->GetTokenTemplate(fields[1].GetUInt32());
    if (tokenTempalte == nullptr)
        return;
    m_id            = fields[1].GetUInt32();
    m_value         = fields[2].GetUInt32();
    m_playerGuid    = player->GetGUID();
    if (IsAccountToken())
        m_playerGuid = player->GetSession()->GetAccountId();
}
void Token::SetPlayerGUID(uint64 guid)
{
    m_playerGuid = guid;
}
uint64 Token::GetPlayerGUID() const
{
    return m_playerGuid;
}
Player* Token::GetPlayer() const
{
    return ObjectAccessor::FindPlayer(GetPlayerGUID());
}
uint32 Token::GetId() const
{
    return m_id;
}
string Token::Name()
{
    if (GetTemplate())
        return GetTemplate()->name;
    return "";
}
string Token::Desc()
{
    if (GetTemplate())
        return GetTemplate()->desc;
    return "";
}
uint32 Token::Get() const
{
    return m_value;
}
uint32 Token::GetMin()
{
    if (GetTemplate())
        return GetTemplate()->minValue;
    return 0;
}
uint32 Token::GetDefault()
{
    if (GetTemplate())
        return GetTemplate()->defaultValue;
    return 0;
}
uint32 Token::GetMax()
{
    if (GetTemplate())
        return GetTemplate()->maxValue;
    return 0;
}
void Token::Set(uint32 value, string reason)
{
    uint32 sourceValue = m_value;
    m_value = value;
    SQLTransaction trans = LoginDatabase.BeginTransaction();
    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_TOKEN_LOG);
    stmt->setUInt32(0, GUID_LOPART(m_playerGuid));
    stmt->setUInt32(1, m_id);
    stmt->setInt32(2, (int32)value - (int32)sourceValue);
    stmt->setUInt32(3, sourceValue);
    stmt->setString(4, reason);
    trans->Append(stmt);
    LoginDatabase.CommitTransaction(trans);
}
void Token::Add(int32 value, string reason)
{
    if (value == 0) return;
    if ((value + int32(m_value)) < int32(GetMin()))
        value = int32(GetMin()) - int32(m_value);
    if (GetMax() > 0 && (m_value + uint32(value)) > GetMax())
        value = GetMax() - m_value;
    if (value == 0) return;
    Set(uint32(m_value + value), reason);
}

void Token::Remove(uint32 value, string reason)
{
    Add(-int32(value), reason);
}

bool Token::IsAccountToken()
{
    if (GetTemplate())
        return GetTemplate()->charOrAccount;
    return false;
}

TokenTemplate const* Token::GetTemplate()
{
    return sTokenMgr->GetTokenTemplate(m_id);
}

void TokenMgr::LoadTokenTemplate()
{
    uint32 oldMSTime = getMSTime();
    //                                                0    1               2      3      4         5              6
    QueryResult result = WorldDatabase.Query("SELECT `id`,`charOrAccount`,`name`,`desc`,`minValue`,`defaultValue`,`maxValue` FROM _token_template");
    if (!result)
    {
        sLog->outString(">> Loaded 0 token templates. DB table `_token_template` is empty.");
        sLog->outString();
        return;
    }
    m_tokenTemplateStore.rehash(result->GetRowCount());
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();

        TokenTemplate& data = m_tokenTemplateStore[id];
        data.id             = id;
        data.charOrAccount  = fields[1].GetBool();
        data.name           = fields[2].GetString();
        data.desc           = fields[3].GetString();
        data.minValue       = fields[4].GetUInt32();
        data.defaultValue   = fields[5].GetUInt32();
        data.maxValue       = fields[6].GetUInt32();
        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u token templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

void TokenMgr::LoadItemAddTokenTable()
{
    uint32 oldMSTime = getMSTime();
    //                                                0    1               2      3      4             5
    QueryResult result = WorldDatabase.Query("SELECT `entry`,`tokenType`,`addValue` FROM _item_add_token");
    if (!result)
    {
        sLog->outString(">> Loaded 0 token templates. DB table `_item_add_token` is empty.");
        sLog->outString();
        return;
    }
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        AddTokenItems data;
        uint32 entry = fields[0].GetUInt32();
        data.entry = entry;
        data.tokenType = fields[1].GetUInt32();
        data.addValue = fields[2].GetUInt32();
        m_addTokenItemsStore[entry] = data;
        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u add token items in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

AddTokenItems const* TokenMgr::GetAddTokenItems(uint32 entry)
{
    AddTokenItemsContainer::const_iterator it = m_addTokenItemsStore.find(entry);
    if (it != m_addTokenItemsStore.end())
        return &(it->second);
    return nullptr;
}

void Player::LoadToken()
{
    TokenTemplateStore allToken = sTokenMgr->GetAllTokens();
    for (TokenTemplateStore::iterator itr = allToken.begin(); itr != allToken.end(); ++itr)
    {
        m_tokens[itr->second.id] = Token(itr->second.id);
    }
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_TOKEN);
    stmt->setUInt32(0, GetGUIDLow());
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        do {
            Field* fields = result->Fetch();
            uint32 id = fields[1].GetUInt32();
            const TokenTemplate* data = sTokenMgr->GetTokenTemplate(id);
            if (data)
                (&m_tokens[fields[1].GetUInt32()])->LoadFromDB(fields, this);
        }
        while (result->NextRow());
    }

    PreparedStatement* stmt2 = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_TOKEN);
    stmt2->setUInt32(0, GetSession()->GetAccountId());
    PreparedQueryResult result2 = LoginDatabase.Query(stmt2);
    if (result2)
    {
        do
        {
            Field* fields = result2->Fetch();
            uint32 id = fields[1].GetUInt32();
            const TokenTemplate* data = sTokenMgr->GetTokenTemplate(id);
            if (data)
                (&m_tokens[fields[1].GetUInt32()])->LoadFromDB(fields, this);
        } while (result2->NextRow());
    }
}

void Player::SaveToken(SQLTransaction& trans)
{
    if (m_tokens.empty())
        return;
    for (TokenMap::iterator itr = m_tokens.begin(); itr != m_tokens.end(); ++itr)
    {
        Token* token = &(itr->second);
        if (token->GetId() > 0 && token->GetTemplate())
        {
            if (token->IsAccountToken())
            {
                PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_REP_ACCOUNT_TOKEN);
                stmt->setUInt32(0, GetSession()->GetAccountId());
                stmt->setUInt32(1, token->GetId());
                stmt->setUInt32(2, token->Get());
                LoginDatabase.Execute(stmt);
            }
            else
            {
                PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_CHARACTER_TOKEN);
                stmt->setUInt32(0, GetGUIDLow());
                stmt->setUInt32(1, token->GetId());
                stmt->setUInt32(2, token->Get());
                trans->Append(stmt);
            }
        }

    }
}

void Player::AddToken(uint32 id, int32 value, string reason)
{
    const TokenTemplate *tokenTempalte = sTokenMgr->GetTokenTemplate(id);
    TokenMap::iterator itr = m_tokens.find(id);
    if (tokenTempalte != nullptr && itr != m_tokens.end())
    {
        (&m_tokens[id])->Add(value, reason);
    }
}

void Player::RemoveToken(uint32 id, uint32 value, string reason)
{
    const TokenTemplate *tokenTempalte = sTokenMgr->GetTokenTemplate(id);
    TokenMap::iterator itr = m_tokens.find(id);
    if (tokenTempalte != nullptr && itr != m_tokens.end())
    {
        (&m_tokens[id])->Remove(value, reason);
    }
}

void Player::SetToken(uint32 id, uint32 value, string reason)
{
    const TokenTemplate *tokenTempalte = sTokenMgr->GetTokenTemplate(id);
    TokenMap::iterator itr = m_tokens.find(id);
    if (tokenTempalte != nullptr && itr != m_tokens.end())
    {
        (&m_tokens[id])->Set(value, reason);
    }
}

uint32 Player::GetToken(uint32 id)
{
    const TokenTemplate *tokenTempalte = sTokenMgr->GetTokenTemplate(id);
    TokenMap::iterator itr = m_tokens.find(id);
    if (tokenTempalte != nullptr && itr != m_tokens.end())
    {
        return (&m_tokens[id])->Get();
    }
    return 0;
}
