#ifndef GTACHECKECAMDISPLAY_H
#define GTACHECKECAMDISPLAY_H
#include "GTACheckBase.h"
#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTACheckEcamDisplay : public GTACheckBase
{
public:
    //enum DisplayType{PAGE_CHANGE = 0, VISUAL_INFO};
    //GTACheckEcamDisplay(DisplayType iType = PAGE_CHANGE);
	GTACheckEcamDisplay();
    GTACheckEcamDisplay(const QString & iType );
	virtual ~GTACheckEcamDisplay();
    void setDisplayType(const QString & iType);
    void setValue(const QString & iValue);

    QString getDisplayType() const;
    QString getValue() const;

    virtual QString getStatement() const;

	virtual QList<GTACommandBase*>& getChildren(void) const;
	virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
	virtual void setParent(GTACommandBase* iParent);
	virtual GTACommandBase* getParent(void) const;
    GTACommandBase*getClone() const;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool hasChannelInControl()const {return false;}
	
	
private:
    //DisplayType _DispType;
    QString _DispType;
    QString _Value;

};

#endif // GTACHECKECAMDISPLAY_H
