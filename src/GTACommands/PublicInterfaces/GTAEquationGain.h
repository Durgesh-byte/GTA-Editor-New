#ifndef GTAEQUATIONGAIN_H
#define GTAEQUATIONGAIN_H
#include "GTAEquationBase.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationGain : public GTAEquationBase
{
public:
    GTAEquationGain(); 
	GTAEquationGain(const GTAEquationGain& rhs);
	virtual ~GTAEquationGain();
	void setGain(const QString & iConst);
    QString getGain() const;
    virtual QString getStatement() const;
	GTAEquationBase* getClone()const;
    bool operator ==(GTAEquationBase*& pObj) const;

    void getFunctionStatement(QString& osFuncType,QString& osFunctionStatement) const;
    void getEquationArguments(QHash<QString,QString>& iSerialzationMap) const;
    void setEquationArguments(const QHash<QString,QString>& iSerialzationMap);
    QString getSimulationName() const { return EQ_GAIN; }

    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap);
    virtual void stringReplace(const QRegExp&, const QString&) {}
    virtual bool searchString(const QRegExp&, bool ) const { return false; }
private:
    QString _GainValue;
};

#endif // GTAEQUATIONGAIN_H
