#include "GTAActionOneClickTitle.h"

GTAActionOneClickTitle::GTAActionOneClickTitle(const QString &iAction,const QString& iComplement) : GTAActionTitle(iAction)
{
    setComplement(iComplement);
}
GTAActionOneClickTitle::GTAActionOneClickTitle(const GTAActionOneClickTitle& iObjTitle): GTAActionTitle(iObjTitle)
{
   
}
GTAActionOneClickTitle::~GTAActionOneClickTitle()
{
  
}
QString GTAActionOneClickTitle::getStatement() const
{
    QString oStatement =  QString("[OneClick Title]%1").arg(getTitle());
    return oStatement;
}

GTACommandBase* GTAActionOneClickTitle::getClone()const
{
    return (new GTAActionOneClickTitle(*this));
}