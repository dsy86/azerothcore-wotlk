#include "DsyMiscMgr.h"

std::string DsyMiscMgr::GetItemLink(uint32 entry, bool noColor/* = false*/)
{
    const ItemTemplate* pItem = sObjectMgr->GetItemTemplate(entry);
    if (!pItem)
        return "";
    std::string name = pItem->Name1;
    if (ItemLocale const *il = sObjectMgr->GetItemLocale(entry))
        ObjectMgr::GetLocaleString(il->Name, DEFAULT_LOCALE, name);

    std::ostringstream oss;
    if (noColor)
        oss  << "|Hitem:" << entry << ":0:" << "0:0:0:0|h[" << name << "]|h";
    else
        oss << "|c" << std::hex << ItemQualityColors[pItem->Quality] << std::dec << "|Hitem:" << entry << ":0:" << "0:0:0:0|h[" << name << "]|h|r";
    return oss.str();
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
