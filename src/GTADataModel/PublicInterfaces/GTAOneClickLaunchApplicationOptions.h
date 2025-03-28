#ifndef GTAOneClickLaunchApplicationOptions_H
#define GTAOneClickLaunchApplicationOptions_H

#include "GTADataModel.h"

#pragma warning(push, 0)
#include <QString>
#include <QStringList>
#include <QHash>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTADataModel_SHARED_EXPORT GTAOneClickLaunchApplicationArgs
{
private:
    QString _ArgsName;
    QString _ArgsType;
    QString _ArgsValue;

public:
    GTAOneClickLaunchApplicationArgs(){}
    ~GTAOneClickLaunchApplicationArgs(){}

    void setArgType(const QString &iArgType) {_ArgsType = iArgType;}
    void setArgName(const QString &iArgName){_ArgsName = iArgName;}
    void setArgValue(const QString &iArgValue){_ArgsValue = iArgValue;}

    QString  getArgsType() const{return _ArgsType;}
    QString  getArgsValue() const{return _ArgsValue;}
    QString getArgName() const {return _ArgsName;}
};

class GTADataModel_SHARED_EXPORT GTAOneClickLaunchApplicationValue
{
private:
    GTAOneClickLaunchApplicationArgs _Args;
    QString _ValueName;
public:
    GTAOneClickLaunchApplicationValue(){}
    ~GTAOneClickLaunchApplicationValue(){}

    void insertArg(const GTAOneClickLaunchApplicationArgs &iArg) {_Args = iArg;}
    void setValueName(const QString &iName){_ValueName = iName;}

    GTAOneClickLaunchApplicationArgs getArgs() const{return _Args;}
    QString getValueName() const {return _ValueName;}

};

class GTADataModel_SHARED_EXPORT GTAOneClickLaunchApplicationOptions
{
private:
    QList<GTAOneClickLaunchApplicationValue> _Values;
    QString _OptionName;
    QString _OptionType;
    QString _OptionDefault;
public:
    GTAOneClickLaunchApplicationOptions();
    ~GTAOneClickLaunchApplicationOptions();
    void insertValue(const GTAOneClickLaunchApplicationValue &iVal){_Values<<iVal;}
    void setOptionName(const QString &iOptionName){_OptionName = iOptionName;}
    void setOptionType(const QString &iOptionType){_OptionType = iOptionType;}
    void setOptionDefault(const QString &iOptionDefault){_OptionDefault = iOptionDefault;}

    QList<GTAOneClickLaunchApplicationValue> getValues() const{return _Values;}
    QString getOptionName() const {return _OptionName;}
    QString getOptionType() const {return _OptionType;}
    QString getOptionDeafault() const{return _OptionDefault;}

};


#endif // GTAOneClickLaunchApplicationOptions_H
