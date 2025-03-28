#ifndef GTACHECKBASE_H
#define GTACHECKBASE_H
#include "GTACommands.h"
#include "GTACommandBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTACheckBase : public GTACommandBase
{
public:
    enum CheckType {VALUE, FWC_WARNING, FWC_PROCEDURE, FWC_DISPLAY,BITE_MESSAGE,ECAM_DISPLAY,AUDIO_ALARM,VISUAL_DISPLAY,MANUAL_CHECK,REFRESH};
    GTACheckBase(CheckType iType);
    virtual ~GTACheckBase();
    GTACheckBase(const GTACheckBase&);
    CheckType getCheckType() const;
    QString getCommandTypeForDisplay() const {return QString("Check");} 

    void setTimeOut(const QString & iTimeOut , const QString& iTimeUnit);
    void setPrecision(const QString & iPrecisionOut , const QString& iPrecisionUnit);
    void setActionOnFail(const QString & iActionOnFail);
    void setConfCheckTime(const QString & iTime , const QString& iUnit);

    void getTimeOut(QString & oTimeOut , QString& oTimeUnit) const;
    void getPrecision(QString & iPrecision , QString& oPrecisionUnit) const;
    void getConfCheckTime( QString & oTime, QString& oUnit) const;
    QString getActionOnFail() const;

    virtual QString getStatement() const = 0;
    CommandType getCommandType() const {return GTACommandBase::CHECK;}

    //Visual Property
    virtual QColor getForegroundColor() const;
    virtual bool isForegroundColor() const {return false;};
    bool hasConfirmatiomTime() const{return true;}
    /**
       * If the Command is not valid then after compatibility check
       * the background color of the Command changes.
       * it gets reset once the command becomes valid.
       */
    virtual QColor getBackgroundColor() const;
    virtual QFont getFont() const;
    bool canHaveChildren() const;

    virtual QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    bool searchText(const QRegExp& iRegExp);

    QString getCheckName()const;
    void addTimeOut(const double& idTimeOut, const QString& iUnit);
    double getTimeOutInMs() const;
    /**
 * @brief Sets auto addition of external action after check
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
    inline void setCheckType(CheckType iType){_Check = iType;}

private:
    CheckType	_Check;
    QString     _TimeOut;
    QString	    _TimeUnit;
    QString     _Precision;
    QString	    _PrecisionUnit;
    QString     _OnFail;
    QString     _ConfTime;
    QString     _ConfTimeUnit;
    bool _autoAddExternalAction = false;
    bool _actionOnOK = false;
    bool _addElse = false;
    bool _autoOpenExternalActionWindow = false;
};

#endif // GTACHECKBASE_H
