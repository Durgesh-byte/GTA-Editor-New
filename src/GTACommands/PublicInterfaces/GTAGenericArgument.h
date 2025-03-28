#ifndef GTAGENERICARGUMENT_H
#define GTAGENERICARGUMENT_H

#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#include <QStringList>
#include <QHash>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTACommands_SHARED_EXPORT GTAGenericArgument
{

public:
    GTAGenericArgument();
    QString getArgumentName() const;
    void setArgumentName(const QString iName);

    QString getArgumentType() const;
    void setArgumentType(const QString iType);

    QString getCondition() const;
    void setCondition(const QString iCondition);

    QStringList getValues();
    void setValues(const QStringList iValues);

    QString getSearchType();
    void setSearchType(const QString iSearchType);

    QString getUserSelectedValue();
    void setUserSelectedValue(const QString iVal);

    QHash<int, QStringList> getConditionValList()const;
    bool isConditionValListEmpty();

    int getUserSelectedHashIndex()const;
    void setUserSelectedHashIndex(const int iIndex);

    bool operator ==(GTAGenericArgument obj2)const;
    bool operator !=(GTAGenericArgument iRhs)const;

    void setDefaultValue(const QStringList& iDefaultValue);
    QStringList getDefaultValue()const;

    void setConstantValue(const QStringList& iConstantValue);
    QStringList getConstantValue()const;

    void setHMILabel(const QString& iLabel);
    QString getHMILabel()const;

    bool isArgumentMandatory() const;
    void setMandatory(const QString& iVal);

    bool isArgConst() const;
    void setArgIsConst(const bool& iVal);

    bool isArgDefault() const;
    void setArgIsDefault(const bool& iVal);

    bool isDefaultOverriden()const;
    void setDefaultOverriden(const bool& iVal);

//    bool operator ==(GTAGenericArgument &iArg);

    void setIsDisplayed(const bool& iVal);
    bool isDisplayed()const;

    void setPath(const QString& iPath);
    QString getPath()const;

    void setId(const int &iId);
    int getId()const;

    void setUserDbName(const QString& iName);
    QString getUserDbName()const;

    void ReplaceTag(const QMap<QString,QString>& iTagValueMap);

    QStringList getVariableList() const;

    void setColor(const QString& iColor);
    bool _isStringVariable = false;
    QString getColor() const;

    void setBackColor(const QString& iBackColor);
    QString getBackColor() const;

private:

    int         _Id;
    QString     _Name;
    QString     _Type;
    QString     _Condition;
    QStringList _Values;
    QString     _SearchType;
    QString     _UserSelectedValue;
    QStringList _DefaultValue;
    QString     _HMILabel;
    QStringList _ConstantValue;
    bool        _isMandatory;
    QHash<int,QStringList> _conditionVals;
    int         _UserSelectedHashIndex;
    bool        _ConstValSet;
    bool        _isDefault;
    bool        _isDefaultOverriden;
    bool        _isDisplayed;
    QString     _Path;
    QString     _UserDbName;
    QString     _Color;
    QString     _BackColor;

    void fillConditionList();
};

#endif // GTAGENERICARGUMENT_H
