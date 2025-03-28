#ifndef GTAONECLICKOPTIONS_H
#define GTAONECLICKOPTIONS_H

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
class GTADataModel_SHARED_EXPORT GTAOneClickOption
{

private:
    QString _OptionName;
    QString _OptionDefaultValue;
    QString _OptionType;
    QString _OptionCondition;
    QStringList _OptionValues;
    bool _HasValues;
    QString _OptionConfigPath;
    QString _OptionDefIniPath;

public:
    void setOptionName(const QString & iOptionName);
    void setOptionType(const QString & iOptionType);
    void setOptionDefaultvalue(const QString & iOptionDefaultValue);
    void setOptionCondition(const QString & iOptionCondition);
    void setOptionValues(const QStringList & iOptionValues);
    void setOptionHasValues(bool ihasValues);
    QString getOptionName() const;
    QString getOptionType() const;
    QString getOptionDefaultValue() const;
    QString getOptionCondition() const;
    QStringList getOptionValues() const;
    bool optionHasValues() const;
    void setOptionConfigPath(const QString &iOptionConfigPath);
    void setOptionDefIniPath(const QString &iOptionDefIniPath);
    QString getOptionConfigPath() const;
    QString getOptionDefIniPath() const;
    GTAOneClickOption();
    ~GTAOneClickOption();
};
class GTADataModel_SHARED_EXPORT GTAOneClickOptions
{
private:
    QHash<QString,GTAOneClickOption> _DefOptionMap;
    QHash<QString,QList<GTAOneClickOption> > _CondOptionMap;
    QHash<QString,QString> _OptionTypeMap;
    QStringList _OptionList;


public:
    GTAOneClickOptions();
    ~GTAOneClickOptions();
    void setDefOption(QString &iOptionName,GTAOneClickOption &iOption);
    void setCondOptions(QString &iOptionName,GTAOneClickOption &iOption);
    void setOptionType(QString &iOptionName,QString &iOptionType);
    void addOptionToList(QString &iOption);



    GTAOneClickOption getOptionDefMap(const QString &iOptionName) const {return _DefOptionMap.value(iOptionName); }
    QList<GTAOneClickOption> getOptionCondMap(const QString &iOptionName)const {return _CondOptionMap.value(iOptionName); }
    QString getOptionTypeMap(const QString &iOptionName)const {return _OptionTypeMap.value(iOptionName); }


    GTAOneClickOption getDefOption(QString &iOptionName);
    QString getOptionType(QString &iOptionName)const ;

    QStringList getAllOptions() const ;
    GTAOneClickOption getOptionforCond(QString &iOptionName, QString &iCondName,QString  &CondVal);

};

#endif // GTAONECLICKOPTIONS_H
