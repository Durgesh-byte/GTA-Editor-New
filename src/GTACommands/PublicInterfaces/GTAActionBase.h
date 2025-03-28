#ifndef GTAACTIONBASE_H
#define GTAACTIONBASE_H
#include "GTACommands.h"
#include "GTACommandBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAActionBase : public GTACommandBase
{
public:
    enum ActionType{CALL,CONDITION,MANUAL,PRINT,RELEASE,SET,WAIT,CALL_EXPAND};
    enum ComplementType{OBJECT,FUNCTION,PROCEDURE,IF,WHILE,MESSAGE,PARAMETER,SOLIDCONTINOUS,LIQUIDCONTINOUS,FOR,UNTIL};
    GTAActionBase();
    virtual ~GTAActionBase();
    GTAActionBase(const QString &iAction, const QString & iComplement);
    GTAActionBase(const GTAActionBase& rhs);

    void setAction(const QString & iAction);
    void setComplement(const QString & iComplement);
    void setTimeOut(const QString & iTimeOut , const QString& iTimeUnit);
    void setPrecision(const QString & iPrecisionOut , const QString& iPrecisionUnit);
    void setActionOnFail(const QString & iActionOnFail);
    void setConfCheckTime(const QString & iTimeOut , const QString& iTimeUnit);

    QString getAction() const;
    QString getComplement() const;
    void getTimeOut(QString & oTimeOut , QString& oTimeUnit) const;
    void getConfCheckTime(QString & oTime , QString& oTimeUnit) const;
    void getPrecision(QString & iPrecision , QString& oPrecisionUnit) const;
    QString getActionOnFail() const;


    virtual QString getStatement() const = 0;
    virtual bool hasPrecision() const=0;
    virtual bool hasTimeOut() const=0;
    virtual bool hasConfirmatiomTime() {return false;}


    double getTimeOutInMs() const;

    GTACommandBase::CommandType getCommandType() const {return GTACommandBase::ACTION;}
    QString getCommandTypeForDisplay() const {return QString("Action");}

    //Visual Property
    virtual QColor getForegroundColor() const;
    void setForegroundColor(const QString& iColor);
    virtual bool isForegroundColor() const { return false; };

    /**
      * If the Command is not valid then after compatibility check
      * the background color of the Command changes.
      * it gets reset once the command becomes valid.
      */
    virtual QColor getBackgroundColor() const;
    virtual QFont getFont() const;
    bool searchText(const QRegExp& iRegExp);

    void setPrecisonforAllChildren(const double& dOffset);
    void setTimeOutsforAllChildren(const double& dOffset);

    void addTimeOut(const double& idTimeOut, const QString& iUnit);
	
	bool isManuallyAdded();
	void setIsManuallyAdded(bool val);

    /**
     * @brief Sets auto addition of external action after set
     * @param actionOnOk Action will launch on OK if true, on KO if false
     * @param addElse Will automatically add else after original if
     * @param autoOpenWindow Will automatically open action selector window 
    */
    void setAutoAddExternalAction(bool addExternal, bool actionOnOk, bool addElse, bool autoOpenWindow) {
        _autoAddExternalAction = addExternal;
        _actionOnOK = actionOnOk;
        _addElse = addElse;
        _autoOpenExternalActionWindow = autoOpenWindow;
    }

    bool isAutoExternalAction() {
        return _autoAddExternalAction;
    }

    bool isExternalActionOnOk() {
        return _actionOnOK;
    }

    bool isAutoOpenWindow() {
        return _autoOpenExternalActionWindow;
    }

    bool isAutoAddElse() {
        return _addElse;
    }
  
protected:
	bool _isManuallyAdded;


private:
    QString     _Action;
    QString     _Complement;
    QString     _TimeOut;
    QString     _Precision;
    QString		_TimeUnit;
    QString		_PrecisionUnit;
    QString     _OnFail;
    QString     _ConfTime;
    QString     _ConfTimeUnit;
    QColor      _Color;

    bool _autoAddExternalAction = false;
    bool _actionOnOK = false;
    bool _addElse = false;
    bool _autoOpenExternalActionWindow = false;
};

#endif // GTAACTIONBASE_H
