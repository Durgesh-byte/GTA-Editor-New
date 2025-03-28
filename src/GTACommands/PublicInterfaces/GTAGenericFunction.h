#ifndef GTAGENERICFUNCTION_H
#define GTAGENERICFUNCTION_H

#include "GTACommands.h"
#include "GTAGenericArgument.h"
#include "GTAGenericParamData.h"

#pragma warning(push, 0)
#include <QString>
#include <QList>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAGenericFunction
{
public:
    GTAGenericFunction();

    QString getFunctionName();
    void setFunctionName(const QString iFunction);

    QList<GTAGenericArgument> getFunctionArguments() const;
    void setFunctionArguments(const QList<GTAGenericArgument> iArguments, bool isCallerSerializer = false);

    GTAGenericAttribute getFunctionReturnType()const;
    void setFunctionReturnType(const GTAGenericAttribute &iReturnParam);

    QString getToolType();
    void setToolType(const QString iToolType);

    QString getToolID();
    void setToolID(const QString iToolID);

    void setReturnParamName(const QString iReturnParamName);
    QString getReturnParamName();

    void setReturnParamType(const QString iReturnParamType);
    QString getReturnParamType();

    void setFunctionSeletced(const bool iSelected);
    bool isFunctionSelected();

    void setWaitForAcknowledgement(const bool &iAcknowledgement);
    bool getWaitForAcknowledgement()const;

    void setDisplayName(const QString &iName);
    QString getDisplayName()const;

    void setArgsToUseList(const QStringList &iList);
    QStringList getArgsToUseList() const;

    QHash<QString, QString> getArgDependencyTable()const;

    void setWaitUntilStatus(bool isWaitUntilTrue);
    bool getWaitUntilStatus()const;

    QList<GTAGenericArgument> getAllArguments()const;
    void setAllArguments(const QList<GTAGenericArgument> &iArgList, bool isCallerSerializer = false);

    QList<GTAGenericArgument> getHiddenArguments()const;

    void setIsReturnTypeSimple(const bool &iVal);
    bool isReturnTypeSimple()const;

    QStringList getVariableList() const;
    void ReplaceTag(const QMap<QString,QString>& iTagValueMap);
    bool operator !=(GTAGenericFunction iRhs)const;
//    bool operator ==(GTAGenericFunction iRhs)const;
    bool operator ==(GTAGenericFunction obj2);

    void setColorTableArg(const QString& iNameArg, const QString& iColor, const QString& iBackColor);
    QString getColorArg(QString& iNameArg) const;
    QString getColor() const;
    QString getBackColor() const;

private:

	QString _Name;
    QString _ToolType;
    QString _ToolID;
    QList<GTAGenericArgument> _Arguments;
    QList<GTAGenericArgument> _AllArguments;
    QString _ReturnParamName;
    QString _ReturnParamType;
    bool    _Selected;
    QString _DisplayName;
    QStringList _ArgsToUseList;
    QHash<QString, QString> _argDependencyTable;
    bool _isWaitUntilTrue;
    GTAGenericAttribute _returnAttribute;
    bool _isReturnTypeSimple;
    bool _isAcknowledgementRequested;

    QHash<QString, QString> _colorOfArgs;
    QHash<QString, QString> _backColorOfArgs;
    QString _Color;
    QString _BackColor;
    //Functions
    void fillArgDependencyTable();
    void setDefaultValues(QList<GTAGenericArgument> &oArgList);
    void setConstantValues(QList<GTAGenericArgument> &oArgList);


};

#endif // GTAGENERICFUNCTION_H
