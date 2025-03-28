#ifndef GTASEARCHELEMENTBASE_H
#define GTASEARCHELEMENTBASE_H

#include "GTAUtil.h"
class GTASearchElementBase
{
public:

    GTASearchElementBase();
    void setSearchType(GTAUtil::ElementType iElementType);
    void setFirstSearchString(const QString & iStr);
    void setSeconSearchString(const QString & iStr);
    void setOperator(const QString & iOper);

    GTAUtil::ElementType getSearchType() const;

private:


};

#endif // GTASEARCHELEMENTBASE_H
