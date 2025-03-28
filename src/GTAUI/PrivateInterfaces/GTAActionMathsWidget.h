#ifndef GTAACTIONMATHSWIDGET_H
#define GTAACTIONMATHSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QStack>
#include <QKeyEvent>
#include "GTAActionWidgetInterface.h"
#include "GTAActionMath.h"

class GTAMathExpression;
class GTAIToken;
class GTATokenFunction;
namespace Ui {
    class GTAActionMathsWidget;
}

class GTAActionMathsWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionMathsWidget(QWidget *parent = 0);
    ~GTAActionMathsWidget();
    //void setComplement(const QString &compVal);

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;

    bool hasSearchItem(){return true;}

    int getSearchType();
    void insertExpression(const QString& iExp);
    
    

    
private slots:
    void onSearchPBClicked();
    void onSearchPBClicked1();
    void onBackSpaceClicked();
    
    void onAddPBClicked();
    void onMinusPBClicked();
    void onMultiplyPBClicked();
    void onDividePBClicked();
    
    void onASinPBClicked();
    void onACosPBClicked();
    void onATanPBClicked();
    
    void onSinPBClicked();
    void onCosPBClicked();
    void onTanPBClicked();

    void onSinHPBClicked();
    void onCosHPBClicked();
    void onTanHPBClicked();

    void onLog10PBClicked();
    void onLog2PBClicked();
    void onLogePBClicked(); 
    void onLog1pPBClicked();
    
    void onAddExpPBClicked();
    void onCloseBracesPBClicked();
    void onOpenBracesPBClicked();
    void onSqrtPBClicked();
    void onAbsPBPBClicked();
    void onExpPBClicked();

    void onCeilPBClicked();
    void onRandPBClicked();
    void onCbrtPBClicked();
    void onFloorPBClicked();
    void onRoundPBClicked();
    void onPIPBClicked();

    void onPow2PBClicked();
    void onPow3PBClicked();
    void onPowYPBClicked();

    void onMinPBClicked();
    void onMaxPBClicked();

    void hideMessage();
    void onExpressionChange();
    

private:

    enum ItemType {kNa,NUMBER,VARIABLE,FUNCTION,OPERATOR,SEPERATOR};


    Ui::GTAActionMathsWidget *ui;
    GTAMathExpression* _pExpression;

    QString getExpression();
    void updateExpression();
    void setButtonConnections();
    void insertFunction(GTATokenFunction* ipFunction);
    void insertTokenItem(GTAIToken* pToken);
    void clearAllStacks();
    void showUnbalancedMessage();
    void showInvalidInputMessage();
    bool validateVariable(const QString& iString);
    void setFormatedText(QString iExpression);
    bool insertClosedBraces();
    
    bool isValidCharachter();
    ItemType getCharType(const QChar& ch)const;
    bool isNumber(const QChar& ch)const;
    bool isVarChar(const QChar& ch)const;
    bool isSeperator(const QChar& ch)const;
    bool isOperator(const QChar& ch)const;
    bool isFunction(const QString& fn , int& oArgs)const;

    bool evaluateAndBuildMathsExpression();
    void addTextSilentlyToExpression(const QString& iTxt);
    QString getProcessedStringForEvaluation(QString iExp);

    QStack<GTAIToken*> _functionStack;
    QStack<GTAIToken*> _bracketStack;
    QStack<GTAIToken*> _backSpaceStack;

    QTimer* _lastTimer;

protected:
    void keyPressEvent ( QKeyEvent * event );
    
    
};

#endif // GTAActionMathWidget_H
