#ifndef GTAACTIONWIDGETINTERFACE_H
#define GTAACTIONWIDGETINTERFACE_H
#include "GTACore.h"
#include "GTAActionBase.h"
#include "GTAICDParameter.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

struct ErrorMessage {
    enum class ErrorType { kError = 0, kWarning, kInfo, kNa };
    enum class MsgSource { kNone = 0, kCheckCompatibility, kFile, kDatabase, kPluginImport, kExportDoc, kSvn, kResult, kPorting, kNoSrc }; //why is there none and nosrc?
    ErrorMessage() : errorType(ErrorType::kError), source(MsgSource::kNone), description(QString()),
                     fileName(QString()), lineNumber(QString()) {}
    
    ErrorMessage(const QString& inDesc, ErrorType inType = ErrorType::kNa, MsgSource inSrc = MsgSource::kNoSrc, const QString& inFileName = "", const QString& inLineNumber = "") :
                description(inDesc), fileName(inFileName), lineNumber(inLineNumber), errorType(inType), source(inSrc) {}

    QString description;
    QString fileName;
    QString lineNumber;
    
    ErrorType errorType;
    MsgSource source;

    QString uuid;
    
};

using ErrorMessageList = QList<ErrorMessage>;

class GTACore_SHARED_EXPORT GTAActionWidgetInterface : public QWidget
{
    Q_OBJECT
protected:
    mutable QString _LastError;
public:
    enum validationFlags {  kNa          = 0x00000000,
                            NUMBER      = 0x00000001,
                            STRING      = 0x00000002,
                            NON_ZERO    = 0x00000004,
                            NON_NEGATIVE= 0x00000008,
                            MANDATORY   = 0x00000010};

    GTAActionWidgetInterface(QWidget * Parent);
    virtual ~GTAActionWidgetInterface(){}
    virtual void clear() = 0;
    virtual bool isValid()const = 0;
    virtual void setActionCommand(const GTAActionBase* ipActionCmd)=0;
    virtual bool getActionCommand(GTAActionBase*& pCmd)const=0;
    
    //TODO: These are to be made abstract
    virtual bool hasSearchItem(){return false;}
    virtual bool hasActionOnFail(){return false;}
    
    virtual bool hasPrecision(){return false;}
    virtual bool hasConfigTime(){return false;}
    virtual bool hasTimeOut(int& oValidation){oValidation=kNa;return false;}
    virtual bool hasExternalComment()const{return true;}
    virtual bool hasCallInput() { return false; }
    
    virtual bool hasDumpList(){return false;}
    virtual void processSearchInput(const QStringList &){}
    virtual void processSearchInput(const QList<GTAICDParameter> &){}
    virtual QList<QWidget*>  getWidgetsInTabOrder()const{return QList<QWidget*>(); }
    virtual int getSearchType(){return -1;}
    //this api is used so that action widget is based on complement makes its widget visible/invisible example call
    virtual void setComplement(const QString& iComplement){iComplement;}
    
    //In case where precison depends upon the search of parameter in db, if DB parameter has a precision it has 
    //to be updated
    virtual QString getUpdatedPrecison(){return _newPrecison;}

    // for command widgets having conditional/assign statements ( =,<= etc)
    virtual bool hasCondition(){return false;}

    virtual QString getLastError();

    virtual void connectExternalItemsWithLocalWidget(QWidget *pParentWidget = NULL);

    virtual bool setWidgetFields([[maybe_unused]] const QList<QPair<QString, QString>>& params, [[maybe_unused]] ErrorMessageList& errorLogs) { return false; }

signals:
    void searchObject(QLineEdit *&);
    
    //command widgets with precison requires to be changed according to condition statements.
    void conditionOperatorChanged(QString);
    void disablePrecision(bool &iStatus);

protected:
    QString _newPrecison;

};

#endif // GTACHECKUIINTERFACE_H
