#include "GTAActionMathsWidget.h"
#include "ui_GTAActionMathsWidget.h"
#include "GTAUtil.h"
#include "GTAMathExpression.h"
#include "GTATokenOperator.h"
#include "GTATokenFunction.h"
#include "GTATokenStartBracket.h"
#include "GTATokenConst.h"
#include "GTATokenFunSeperator.h"
#include "GTATokenVariable.h"
#include "GTATokenEndBracket.h"
#include "GTAActionMaths.h"
#include "GTAParamValidator.h"
#include <QMessageBox>
#include <QValidator>
#include <QTimer>
#include "GTAAppController.h"
#include <QCompleter>
#include "GTATokenCommaSeparator.h"

class GTAExpressionValidator: public QValidator
{
public:
    GTAExpressionValidator(QObject * parent = 0):QValidator(parent){}
    virtual ~GTAExpressionValidator(){};
    //QList<QString> getInvalidChars(QString &iText) const;
    State	validate ( QString & input, int & pos ) const
    {

        //QRegExp rx("([A-Za-z0-9.?,\\\\@#$_:\\;<>\"\\s]+)");
        QRegExp rx("([A-Za-z0-9.?,\\\\@_:\\;\"\\s]+)");
        QRegExpValidator validator(rx, 0);
        QValidator::State rc =  validator.validate(input,pos);
        return rc;



    }
};


GTAActionMathsWidget::GTAActionMathsWidget(QWidget *parent) :GTAActionWidgetInterface(parent),
ui(new Ui::GTAActionMathsWidget)
{
    ui->setupUi(this);

    setButtonConnections();

    _pExpression = new GTAMathExpression;

    GTAExpressionValidator* pValidator = new GTAExpressionValidator(ui->expressionLE);
    ui->expressionLE->setValidator(pValidator);
    GTAParamValidator* pValidator1 = new GTAParamValidator(ui->ParameterLE);
    ui->ParameterLE->setValidator(pValidator1);

    ui->invalidLB->hide();
    _lastTimer=NULL;


    GTAAppController* pController = GTAAppController::getGTAAppController();
    if (NULL!=pController)
    {
        QStringList wordList = pController->getVariablesForAutoComplete();
        QCompleter *completer = new QCompleter(wordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        ui->ParameterLE->setCompleter(completer);
    }

}

GTAActionMathsWidget::~GTAActionMathsWidget()
{
    delete ui;
    if (NULL!=_pExpression)
        delete _pExpression;
}
void GTAActionMathsWidget::clear()
{
    if (NULL!=_pExpression)
    { 
        delete _pExpression;
        _pExpression = new GTAMathExpression;
        clearAllStacks();
    }

    ui->ParameterLE->clear();
    ui->expressionLE->clear();
    ui->expressionTE->clear();
}

void GTAActionMathsWidget::  setButtonConnections()
{
    connect(ui->searchParamPB_2,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->searchParamPB1,SIGNAL(clicked()),this,SLOT(onSearchPBClicked1()));



    connect(ui->addPB,SIGNAL(clicked()),this,SLOT(onAddPBClicked()));
    connect(ui->subtractPB,SIGNAL(clicked()),this,SLOT(onMinusPBClicked()));
    connect(ui->dividePB,SIGNAL(clicked()),this,SLOT(onDividePBClicked()));
    connect(ui->multiplyPB,SIGNAL(clicked()),this,SLOT(onMultiplyPBClicked()));
    connect(ui->addToExpressionPB,SIGNAL(clicked()),this,SLOT(onAddExpPBClicked()));
    connect(ui->closeBracketPB,SIGNAL(clicked()),this,SLOT(onCloseBracesPBClicked()));
    connect(ui->openBracketPB,SIGNAL(clicked()),this,SLOT(onOpenBracesPBClicked()));

    connect(ui->sinPB,SIGNAL(clicked()),this,SLOT(onSinPBClicked()));
    connect(ui->cosPB,SIGNAL(clicked()),this,SLOT(onCosPBClicked()));
    connect(ui->tanPB,SIGNAL(clicked()),this,SLOT(onTanPBClicked()));

    connect(ui->asinPB,SIGNAL(clicked()),this,SLOT(onASinPBClicked()));
    connect(ui->acosPB,SIGNAL(clicked()),this,SLOT(onACosPBClicked()));
    connect(ui->atanPB,SIGNAL(clicked()),this,SLOT(onATanPBClicked()));

    connect(ui->sinhPB,SIGNAL(clicked()),this,SLOT(onSinHPBClicked()));
    connect(ui->coshPB,SIGNAL(clicked()),this,SLOT(onCosHPBClicked()));
    connect(ui->tanhPB,SIGNAL(clicked()),this,SLOT(onTanHPBClicked()));

    connect(ui->log10PB,SIGNAL(clicked()),this,SLOT(onLog10PBClicked()));
    connect(ui->log2PB,SIGNAL(clicked()),this,SLOT(onLog2PBClicked()));
    connect(ui->lnPB,SIGNAL(clicked()),this,SLOT(onLogePBClicked()));
    connect(ui->logipPB,SIGNAL(clicked()),this,SLOT(onLog1pPBClicked()));

    connect(ui->sqrtPB,SIGNAL(clicked()),this,SLOT(onSqrtPBClicked()));
    connect(ui->absPB,SIGNAL(clicked()),this,SLOT(onAbsPBPBClicked()));
    connect(ui->expPB,SIGNAL(clicked()),this,SLOT(onExpPBClicked()));

    connect(ui->ceilPB,SIGNAL(clicked()),this,SLOT(  onCeilPBClicked()));
    connect(ui->randPB,SIGNAL(clicked()),this,SLOT(  onRandPBClicked()));
    connect(ui->cbrtPB,SIGNAL(clicked()),this,SLOT(  onCbrtPBClicked()));
    connect(ui->floorPB,SIGNAL(clicked()),this,SLOT( onFloorPBClicked()));
    connect(ui->roundPB,SIGNAL(clicked()),this,SLOT( onRoundPBClicked()));
    connect(ui->piPB,SIGNAL(clicked()),this,SLOT( onPIPBClicked()));
    connect(ui->expressionTE,SIGNAL(textChanged()),this,SLOT( onExpressionChange()));
    connect(ui->pow2PB,SIGNAL(clicked()),this,SLOT(onPow2PBClicked()));
    connect(ui->pow3PB,SIGNAL(clicked()),this,SLOT(onPow3PBClicked()));
    connect(ui->powY_PB,SIGNAL(clicked()),this,SLOT(onPowYPBClicked()));
    connect(ui->minPB,SIGNAL(clicked()),this,SLOT(onMinPBClicked()));
    connect(ui->maxPB,SIGNAL(clicked()),this,SLOT(onMaxPBClicked()));



}
bool GTAActionMathsWidget::isValid()const
{
    bool expressionValidStatus = _pExpression->isValid();
    if(expressionValidStatus==false)
        _LastError = "Invalid expression";
    else
        _LastError.clear();
    if ((ui->ParameterLE->text().isEmpty() == true )|| (expressionValidStatus==false) || (_pExpression->isEmpty()==true))
        return false;
    else
        return true;
}


bool GTAActionMathsWidget::getActionCommand(GTAActionBase *& pCmd)const
{
    ui->invalidLB->hide();
   
    if(!isValid())
    {
        pCmd = NULL;
        return false;
    }
    else
    {
        GTAActionMaths* pMath = new GTAActionMaths;
        pMath->setExpression(new GTAMathExpression(_pExpression));
        pMath->setExpressionTxt(ui->expressionTE->toPlainText());
        pCmd= pMath;
        pMath->setFirstVariable(ui->ParameterLE->text());
        return true;
    }


}
void GTAActionMathsWidget::setActionCommand(const GTAActionBase * pCmd)
{
    ui->invalidLB->hide();

    GTAActionBase* pBase = (GTAActionBase *)pCmd;
    GTAActionMaths* pMath = dynamic_cast<GTAActionMaths*> (pBase);
    clear();
    if (NULL!=pMath)
    {

        GTAMathExpression* pExpression = pMath->getExpression();
        if(pExpression!=NULL)
        {
            QList<GTAIToken*> flatItems;
            if (!pExpression->isEmpty())
            { 
                _pExpression = new GTAMathExpression(pExpression);

            }

            updateExpression();
        }
        ui->expressionTE->setText( pMath->getExpressionTxt() );
        ui->ParameterLE->setText(pMath->getFirstVariable());
        //evaluateAndBuildMathsExpression();



    }


}
void GTAActionMathsWidget::onSearchPBClicked()
{
    emit searchObject(ui->expressionLE);
}
void GTAActionMathsWidget::onSearchPBClicked1()
{
    emit searchObject(ui->ParameterLE);
}

void GTAActionMathsWidget::onAddPBClicked()
{
    //GTATokenOperator* pOperator = new GTATokenOperator("+");
    ///* _pExpression->pushToken(pOperator);
    //updateExpression();*/

    //insertTokenItem(pOperator);
    insertExpression("+");
}
void GTAActionMathsWidget::onMinusPBClicked()
{
    /* GTATokenOperator* pOperator = new GTATokenOperator("-");
    insertTokenItem(pOperator);*/
    /* _pExpression->pushToken(pOperator);
    updateExpression();*/
    insertExpression("-");
}

void GTAActionMathsWidget::onMultiplyPBClicked()
{
    /* GTATokenOperator* pOperator = new GTATokenOperator("*");
    insertTokenItem(pOperator);*/
    /*_pExpression->pushToken(pOperator);
    updateExpression();*/
    insertExpression("*");
}
void GTAActionMathsWidget::onDividePBClicked()
{
    /* GTATokenOperator* pOperator = new GTATokenOperator("/");
    insertTokenItem(pOperator);*/
    /*_pExpression->pushToken(pOperator);
    updateExpression();*/
    insertExpression("/");
}

QString GTAActionMathsWidget::getExpression()
{
    return _pExpression->getExpression();

}
void GTAActionMathsWidget::updateExpression()
{
    //bool expStatus = evaluateAndBuildMathsExpression();
    //setFormatedText(getExpression());

}

void GTAActionMathsWidget::onExpPBClicked()
{
    /*GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_EXP,1);
    insertFunction(pFunction); */  

    insertExpression(QString("%1(").arg(MATH_FUN_EXP));

}
void GTAActionMathsWidget::onAbsPBPBClicked()
{
    /* GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_ABS,1);
    insertFunction(pFunction);   */
    insertExpression(QString("%1(").arg(MATH_FUN_ABS));

}
void GTAActionMathsWidget::onSqrtPBClicked()
{
    /*GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_SQRT,1);
    insertFunction(pFunction);   */
    insertExpression(QString("%1(").arg(MATH_FUN_SQRT));

}
void GTAActionMathsWidget::onSinPBClicked()
{
    /*GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_SIN,1);
    insertFunction(pFunction);   */
    insertExpression(QString("%1(").arg(MATH_FUN_SIN));

}
void GTAActionMathsWidget::onCosPBClicked()
{
    /* GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_COS,1);
    insertFunction(pFunction);   */ 
    insertExpression(QString("%1(").arg(MATH_FUN_COS));

}
void GTAActionMathsWidget::onTanPBClicked()
{
    /*    GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_TAN,1);
    insertFunction(pFunction);*/  
    insertExpression(QString("%1(").arg(MATH_FUN_TAN));

}
void GTAActionMathsWidget::onASinPBClicked()
{
    /*  GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_ASIN,1);
    insertFunction(pFunction);   */
    insertExpression(QString("%1(").arg(MATH_FUN_ASIN));

}
void GTAActionMathsWidget::onACosPBClicked()
{
    /* GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_ACOS,1);
    insertFunction(pFunction);  */ 
    insertExpression(QString("%1(").arg(MATH_FUN_ACOS));

}
void GTAActionMathsWidget::onATanPBClicked()
{
    /*GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_ATAN,1);
    insertFunction(pFunction);   */
    insertExpression(QString("%1(").arg(MATH_FUN_ATAN));

}
void GTAActionMathsWidget::onSinHPBClicked()
{
    /* GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_SINH,1);
    insertFunction(pFunction);   */
    insertExpression(QString("%1(").arg(MATH_FUN_SINH));

}
void GTAActionMathsWidget::onCosHPBClicked()
{
    /*GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_COSH,1);
    insertFunction(pFunction);   */
    insertExpression(QString("%1(").arg(MATH_FUN_COSH));

}
void GTAActionMathsWidget::onTanHPBClicked()
{
    /*GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_TANH,1);
    insertFunction(pFunction);  */ 
    insertExpression(QString("%1(").arg(MATH_FUN_TANH));

}
//void GTAActionMathsWidget::insertFunction(GTATokenFunction* ipFunction)
//{
//    if(NULL!=ipFunction)
//    {
//
//        if(_pExpression->pushToken(ipFunction) == false)
//        {
//            //QMessageBox::information(this,"invalid input","Invalid input");
//            showInvalidInputMessage();
//            //ui->invalidLB->show();
//            return;
//        }
//        else
//            ui->invalidLB->hide();
//
//
//        GTATokenStartBracket* pStartBraces = new GTATokenStartBracket("(");
//        _pExpression->pushToken(pStartBraces);
//        while (!_bracketStack.isEmpty())
//        {
//            _functionStack.push(_bracketStack.pop());
//        }
//
//        _functionStack.push(ipFunction);
//        _bracketStack.push(pStartBraces);
//    }
//    updateExpression();
//
//}

void GTAActionMathsWidget::insertFunction(GTATokenFunction* ipFunction)
{
    if(NULL!=ipFunction)
    {

        ui->invalidLB->hide();


        _pExpression->pushToken(ipFunction);
        /* GTATokenStartBracket* pStartBraces = new GTATokenStartBracket("(");
        ipFunction->pushToken(pStartBraces);*/

    }
    updateExpression();

}
void GTAActionMathsWidget::insertTokenItem(GTAIToken* pToken)
{


    if (_pExpression->pushToken(pToken)==false)
    {
        //QMessageBox::information(this,"invalid input","Invalid input");
        showInvalidInputMessage();
    }
    else
        ui->invalidLB->hide();

}
void GTAActionMathsWidget::onAddExpPBClicked()
{

    QString userExpression = ui->expressionLE->text();
    QStringList arguments = userExpression.split(",",QString::SkipEmptyParts);
    addTextSilentlyToExpression(userExpression);
    if(sender()==ui->addToExpressionPB)
         ui->expressionTE->moveCursor(QTextCursor::End);
    for (int i=0;i<arguments.size();i++)
    {
        QString currentStr = arguments.at(i);
        currentStr=currentStr.trimmed();
        bool isNum = false;
        bool isComma = currentStr == QString(",");
        currentStr.toDouble(&isNum);

        if(isNum)
        {
            GTATokenConst* pConst = new GTATokenConst(currentStr);
            insertTokenItem(pConst);

        }
        else if (isComma)
        {
            GTATokenFunSeperator* pSep = new GTATokenFunSeperator(currentStr);
            insertTokenItem(pSep);
        }
        else
        {
            bool validStr = validateVariable(currentStr);
            if(validStr==false)
            {
                showInvalidInputMessage();
                return;
            }
            GTATokenVariable* pVar = new GTATokenVariable(currentStr);
            insertTokenItem(pVar);
        }
    }


}
void GTAActionMathsWidget::onCloseBracesPBClicked()
{
    insertExpression(")");
}
//void GTAActionMathsWidget::onCloseBracesPBClicked()
//{
//
//    GTAIToken* pLastToken = _pExpression->getLastItem();
//
//    /*
//    if last token is a function it implies that function is not succeeded by open bracket 
//    */
//
//    if(pLastToken==NULL)
//    {
//
//        showUnbalancedMessage();
//        return;
//    }
//    else
//    {
//        if( pLastToken->isFunction())
//        {
//            //QMessageBox::information(this,"invalid input","Invalid input");
//            showUnbalancedMessage();
//            return;
//        }
//
//        if( pLastToken->isStartBracket())
//        {
//            // QMessageBox::information(this,"invalid input","Invalid input");
//            showInvalidInputMessage();
//            return;
//        }
//
//
//        if( pLastToken->isEndBracket() )
//        {
//            if(_pExpression->isBracesBalanced())
//            {
//                //QMessageBox::information(this,"invalid input","Unbalanced close bracket encountered.");
//                showUnbalancedMessage();
//                return;
//            }
//
//        }
//    }
//
//
//    ui->invalidLB->hide();
//
//    GTATokenEndBracket* pBracket = new GTATokenEndBracket(")");
//    if (_pExpression->pushToken(pBracket)==false)
//    {
//        showUnbalancedMessage();
//        delete pBracket;
//
//    }
//
//    
//    updateExpression();
//}
bool GTAActionMathsWidget::insertClosedBraces()
{

    GTAIToken* pLastToken = _pExpression->getLastItem();

    /*
    if last token is a function it implies that function is not succeeded by open bracket 
    */

    if(pLastToken==NULL)
    {

        showUnbalancedMessage();
        return false;
    }
    else
    {
        if( pLastToken->isFunction())
        {
            //QMessageBox::information(this,"invalid input","Invalid input");
            showUnbalancedMessage();
            return false;
        }

        if( pLastToken->isStartBracket())
        {
            // QMessageBox::information(this,"invalid input","Invalid input");
            showInvalidInputMessage();
            return false;
        }


        if( pLastToken->isEndBracket() )
        {
            if(_pExpression->isBracesBalanced())
            {
                //QMessageBox::information(this,"invalid input","Unbalanced close bracket encountered.");
                showUnbalancedMessage();
                return false;
            }

        }
    }


    ui->invalidLB->hide();

    GTATokenEndBracket* pBracket = new GTATokenEndBracket(")");
    if (_pExpression->pushToken(pBracket)==false)
    {
        showUnbalancedMessage();
        delete pBracket;
        return false;

    }


    return true;
}
//void GTAActionMathsWidget::onCloseBracesPBClicked()
//{
//
//    GTAIToken* pLastToken = _pExpression->getLastItem();
//
//    /*
//        if last token is a function it implies that function is not succeeded by open bracket 
//    */
//
//    if(pLastToken==NULL)
//    {
//       
//        showUnbalancedMessage();
//        return;
//    }
//    else
//    {
//        if( pLastToken->isFunction())
//        {
//            //QMessageBox::information(this,"invalid input","Invalid input");
//             showUnbalancedMessage();
//            return;
//        }
//
//        if( pLastToken->isStartBracket())
//        {
//            // QMessageBox::information(this,"invalid input","Invalid input");
//             showInvalidInputMessage();
//            return;
//        }
//
//
//        if( pLastToken->isEndBracket() )
//        {
//            if(_bracketStack.isEmpty())
//            {
//                //QMessageBox::information(this,"invalid input","Unbalanced close bracket encountered.");
//                showUnbalancedMessage();
//                return;
//            }
//
//        }
//    }
//    
//
//    ui->invalidLB->hide();
//
//    GTATokenEndBracket* pBracket = new GTATokenEndBracket(")");
//    bool setFunctionClosed = false;
//
//    if(!_bracketStack.isEmpty())
//        _backSpaceStack.push(_bracketStack.pop());
//
//    if (!_functionStack.isEmpty())
//    {
//        GTAIToken* pTopItem = _functionStack.top();
//
//        if(_bracketStack.isEmpty())
//        {
//            GTAIToken* pToken = _functionStack.pop();
//            _backSpaceStack.push(pToken);
//            if(pToken->isFunction() &&_functionStack.isEmpty()==false)
//            {
//                do
//                {
//                    pToken = _functionStack.top();
//                    if(pToken->isStartBracket())
//                    {
//                        if(!_functionStack.isEmpty())
//                            _bracketStack.push(_functionStack.pop());
//                        if(_functionStack.isEmpty())
//                            break;
//                    }
//                } while (pToken->isFunction()==false  );
//            }
//
//            setFunctionClosed=true;
//        }
//
//
//
//        if (pTopItem->isFunction() )
//        {
//            pTopItem->pushToken(pBracket);
//
//            GTATokenFunction* pFunction = dynamic_cast<GTATokenFunction*>(pTopItem);
//            if(pFunction!=NULL && setFunctionClosed)
//            {
//                pFunction->setClosed(true);
//            }
//        }
//        else if (pTopItem->isStartBracket())
//        {
//
//            _backSpaceStack.push(_functionStack.pop());
//        }
//
//
//
//    }
//    else
//        _pExpression->pushToken(pBracket);
//    updateExpression();
//}
void GTAActionMathsWidget::onOpenBracesPBClicked()
{

    /*  ui->invalidLB->hide();
    GTATokenStartBracket* pBracket = new GTATokenStartBracket("(");
    _pExpression->pushToken(pBracket);
    updateExpression();*/
    insertExpression("(");
}
//void GTAActionMathsWidget::onOpenBracesPBClicked()
//{
//
//    ui->invalidLB->hide();
//    GTATokenStartBracket* pBracket = new GTATokenStartBracket("(");
//    _bracketStack.push(pBracket);
//    if(!_functionStack.isEmpty())
//    {
//
//        GTAIToken* pTopItem = _functionStack.top();
//        if (pTopItem!= NULL && pTopItem->isFunction())
//        {
//            pTopItem->pushToken(pBracket);
//        }
//    }
//    else
//        _pExpression->pushToken(pBracket);
//    updateExpression();
//}
void GTAActionMathsWidget::clearAllStacks()
{
    _backSpaceStack.clear();
    _functionStack.clear();
    _bracketStack.clear();
}
void GTAActionMathsWidget::onBackSpaceClicked()
{
    ui->invalidLB->hide();
    _pExpression->popToken();
    updateExpression();
}

//void GTAActionMathsWidget::onBackSpaceClicked()
//{
//    ui->invalidLB->hide();
//    if (_pExpression->isEmpty())
//    {
//        clearAllStacks();
//        return;
//    }
//    GTAIToken* pLastToken = _pExpression->getLastItem();
//    if(pLastToken!= NULL && pLastToken->isEndBracket())
//    {
//
//
//        if(!_backSpaceStack.isEmpty())
//        {
//            GTAIToken* pToken = _backSpaceStack.top();
//
//            if(pToken!= NULL && pToken->isFunction())
//            {
//                while(_bracketStack.isEmpty()==false)
//                    _functionStack.push(_bracketStack.pop());
//
//                _functionStack.push(_backSpaceStack.pop());
//                pToken=_backSpaceStack.top();
//
//
//
//            }
//
//            if(pToken->isStartBracket())
//                _bracketStack.push(_backSpaceStack.pop());
//        }
//    }
//    else if(pLastToken->isFunction())
//    {
//        if (_functionStack.isEmpty()==false)
//        {
//            _functionStack.pop();
//            if (_functionStack.isEmpty()==false)
//            {
//                GTAIToken* pTopFToken = _functionStack.top();
//                while (pTopFToken!=NULL && pTopFToken->isFunction()==false)
//                {
//                    
//
//                    _bracketStack.push(_functionStack.pop());
//                    if(!_functionStack.isEmpty())
//                        pTopFToken = _functionStack.top();
//                    else
//                        break;
//
//                }
//            }
//        }
//    }
//    else if (pLastToken!=NULL && pLastToken->isStartBracket())
//    {
//        if(!_bracketStack.isEmpty())
//        {
//            _bracketStack.pop();
//        }
//    }
//
//    _pExpression->popToken();
//    updateExpression();
//}
void GTAActionMathsWidget::onLog10PBClicked()
{
    /*    GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_LOG10,1);
    insertFunction(pFunction);*/ 
    insertExpression(QString("%1(").arg(MATH_FUN_LOG10));
}
void GTAActionMathsWidget::onLog2PBClicked()
{
    /*    GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_LOG2,1);
    insertFunction(pFunction); */ 
    insertExpression(QString("%1(").arg(MATH_FUN_LOG2));
}
void GTAActionMathsWidget::onLogePBClicked()
{
    //GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_LOGE,1);
    //insertFunction(pFunction);
    insertExpression(QString("%1(").arg(MATH_FUN_LOGE));
}
void GTAActionMathsWidget::onLog1pPBClicked()
{
    /*    GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_LOG1P,1);
    insertFunction(pFunction); */  
    insertExpression(QString("%1(").arg(MATH_FUN_LOG1P));
}

void GTAActionMathsWidget::onCeilPBClicked()
{
    /*    GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_CEIL,1);
    insertFunction(pFunction); */  
    insertExpression(QString("%1(").arg(MATH_FUN_CEIL));
}
void GTAActionMathsWidget::onRandPBClicked()
{
    /*    GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_RAND,1);
    insertFunction(pFunction);*/ 
    insertExpression(QString("%1()").arg(MATH_FUN_RAND));
}
void GTAActionMathsWidget::onCbrtPBClicked()
{
    /*    GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_CBRT,1);
    insertFunction(pFunction);  */ 
    insertExpression(QString("%1(").arg(MATH_FUN_CBRT));
}
void GTAActionMathsWidget::onFloorPBClicked()
{
    /*    GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_FLOOR,1);
    insertFunction(pFunction);*/   
    insertExpression(QString("%1(").arg(MATH_FUN_FLOOR));
}
void GTAActionMathsWidget::onRoundPBClicked()
{
    /*    GTATokenFunction* pFunction = new GTATokenFunction(MATH_FUN_ROUND,1);
    insertFunction(pFunction); */  
    insertExpression(QString("%1(").arg(MATH_FUN_ROUND));
}

void GTAActionMathsWidget::onPIPBClicked()
{
    /*    GTATokenConst* pPi = new GTATokenConst("3.142");
    insertTokenItem(pPi);*/   
    insertExpression("3.142");
}

void GTAActionMathsWidget::onPow2PBClicked()
{
    insertExpression(QString("%1(,2)").arg(MATH_FUN_POW));
}

void GTAActionMathsWidget::onPow3PBClicked()
{
    insertExpression(QString("%1(,3)").arg(MATH_FUN_POW));
}

void GTAActionMathsWidget::onPowYPBClicked()
{
    insertExpression(QString("%1(,)").arg(MATH_FUN_POW));
}

void GTAActionMathsWidget::onMinPBClicked()
{
    insertExpression(QString("%1(,)").arg(MATH_FUN_MIN));
}

void GTAActionMathsWidget::onMaxPBClicked()
{
    insertExpression(QString("%1(,)").arg(MATH_FUN_MAX));
}

void GTAActionMathsWidget::showUnbalancedMessage()
{

    ui->invalidLB->setText(QApplication::translate("GTAActionMathsWidget", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
		"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; font-weight:600; color:#aa0000;\">. . . Unbalanced braces encountered</span></p></body></html>", 0));
    ui->invalidLB->show();

    if(_lastTimer)
    {
        _lastTimer->stop();
        disconnect(_lastTimer, SIGNAL(timeout()),  this, SLOT(hideMessage()));
        delete _lastTimer;
        _lastTimer=NULL;

    }

    ui->invalidLB->show();
    QTimer *timer = new QTimer(0);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()),  this, SLOT(hideMessage()));
    timer->start(2500);
    _lastTimer = timer;

}
void GTAActionMathsWidget::showInvalidInputMessage()
{
    ui->invalidLB->setText("WARNING : Expression is not correct!");
	ui->invalidLB->setStyleSheet("QLabel {color : red; font-size : 10pt; font-weight : 600;}");
    // QLabel is hidden when the expression become correct again
	ui->invalidLB->show();
}
void GTAActionMathsWidget::hideMessage()
{
    ui->invalidLB->hide();
}
bool GTAActionMathsWidget::validateVariable(const QString& iString)
{
    if(iString.isEmpty())
        return false;

    // check if it starts with number
    QString numchk = QString(iString.at(0));
    bool isNum;
    numchk.toInt(&isNum);
    if (isNum)
    {
        return false;
    }

    //Check if it has more than two '@'

    int noOfAt = iString.count("@");
    if(noOfAt )
    {
        if(noOfAt==2)
        {
            if(iString.startsWith("@") == false || iString.endsWith("@") == false)
                return false;
            else
            {
                if(iString.size()<=2)
                    return false;
                else
                {
                    //@123@ is invalid , @123q@ is also invalid
                    QString currStr = iString;
                    currStr.remove("@");
                    bool isNum;
                    currStr = currStr.at(0);
                    currStr.toInt(&isNum);
                    if (isNum)
                        return false;


                }
            }
        }
        else 
            return false;

    }

    if (iString.startsWith("$"))
        return false;

    if (iString.contains(" "))
        return false;

    return true;

}
void GTAActionMathsWidget::keyPressEvent ( QKeyEvent * event )
{
    /*if (event->key()==Qt::Key_Backspace )
    {
    onBackSpaceClicked();
    }*/
    QWidget::keyPressEvent(event);
}

void GTAActionMathsWidget::setFormatedText(QString iExpression)
{

    QStringList lstOfFun = QStringList()<<MATH_FUN_EXP<<MATH_FUN_ABS<<MATH_FUN_SQRT<<MATH_FUN_ASIN<<MATH_FUN_ACOS<<MATH_FUN_ATAN<<MATH_FUN_SINH<<MATH_FUN_COSH<<MATH_FUN_TANH<<MATH_FUN_LOG10<<MATH_FUN_LOG2<<MATH_FUN_LOGE<<MATH_FUN_LOG1P<<MATH_FUN_CEIL<<MATH_FUN_RAND<<MATH_FUN_CBRT<<MATH_FUN_FLOOR<<MATH_FUN_ROUND<<MATH_FUN_SIN<<MATH_FUN_COS<<MATH_FUN_TAN<<MATH_FUN_POW<<MATH_FUN_MAX<<MATH_FUN_MIN;

    QString htmlCode = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">$$$</span></p></body></html>";

    //get current cursor cursorPosition
    int cursorPosition = ui->expressionTE->textCursor().position();


    foreach(QString funName,lstOfFun)
    {
        QStringList startingCharLst = QStringList()<<"+"<<"-"<<" "<<"*"<<"/";

        // variableNamesin should not be replaced
        foreach(QString startingChar,startingCharLst)
        {
            iExpression.replace( QString("%1%2 ").arg(startingChar,funName),QString("%1<span style=\" font-size:10pt; font-weight:600; color:#5555ff;\">%2</span> ").arg(startingChar,funName) );
            iExpression.replace( QString("%1%2(").arg(startingChar,funName),QString("%1<span style=\" font-size:10pt; font-weight:600; color:#5555ff;\">%2</span>(").arg(startingChar,funName) );
        }
        if (iExpression.startsWith(QString("%1 ").arg(funName)))
        {
            iExpression.replace(0,funName.size()+1,QString("<span style=\" font-size:10pt; font-weight:600; color:#5555ff;\">%1</span> ").arg(funName));
        }
        if (iExpression.startsWith(QString("%1(").arg(funName)))
        {
            iExpression.replace(0,funName.size()+1,QString("<span style=\" font-size:10pt; font-weight:600; color:#5555ff;\">%1</span>(").arg(funName));
        }
    }

    disconnect(ui->expressionTE,SIGNAL(textChanged()),this,SLOT( onExpressionChange()));

    ui->expressionTE->setHtml(htmlCode.replace("$$$",iExpression));
    QTextCursor currentCursor = ui->expressionTE->textCursor();
    currentCursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,cursorPosition);
    ui->expressionTE->setTextCursor(currentCursor);
    connect(ui->expressionTE,SIGNAL(textChanged()),this,SLOT( onExpressionChange()));



}
void GTAActionMathsWidget::onExpressionChange()
{
    QString text = ui->expressionTE->toPlainText ();
   
    bool expressionStatus = evaluateAndBuildMathsExpression();
    if(expressionStatus==false)
        showInvalidInputMessage();
    else
        ui->invalidLB->hide();
    if(text.isEmpty() == false )
    {

        if(isValidCharachter()==false)
            text=text.mid(0,text.size()-1);
        setFormatedText(text);
    }



}
bool GTAActionMathsWidget::isValidCharachter()
{
    QString text = ui->expressionTE->toPlainText ();
    if(text.contains(">")) // to plain text does not return properly when ">" is entered
        return false;
    QChar lastCharachter = text.at(text.size()-1);
    if(lastCharachter=='\t')
        return false;
    QRegExp rx("([A-Za-z0-9*/\\+\\(\\)\\-,.@_\\s]+)");
    QString str(lastCharachter);
    bool status = str.contains(rx);
    return status;
}
QString GTAActionMathsWidget::getProcessedStringForEvaluation(QString iExp)
{

    if(iExp.isEmpty()==false)
    {
        //replace all new lines
        iExp.replace("\n"," ");

        //replace all double spaces
        bool hasEndSpace = false;
        if(iExp.endsWith(" "))
            hasEndSpace=true;
        iExp = iExp.simplified();

        //replace all unwanted spaces
        QStringList symbols = QStringList()<<"*"<<"+"<<"-"<<"/"<<"("<<")";
        foreach(QString symbol, symbols)
        {
            iExp.replace(QString(" %1").arg(symbol),symbol);
            iExp.replace(QString("%1 ").arg(symbol),symbol);
        }

        if (hasEndSpace)
            iExp.append(" ");
    }
    return iExp;

}
bool GTAActionMathsWidget::evaluateAndBuildMathsExpression()
{
    bool rc = true;
    bool ignoreComma = false;
    //bool commaFunction = false;

    QString expressionInTE = getProcessedStringForEvaluation(ui->expressionTE->toPlainText());



    if(expressionInTE.isEmpty() == false)
    {

        QString stackStr;  

        ItemType currentStkType = kNa;
        ItemType charType = kNa;
        ItemType previousStkType = kNa;

        _pExpression->clearAll();

        int startVal=0;

        if(expressionInTE.startsWith("-"))
        {
            stackStr.append("-");
            if(expressionInTE.size()>=2)
            {

                currentStkType=getCharType(expressionInTE.at(1));
                startVal++;
            }

        }

        for(int i=startVal;i<expressionInTE.size();i++)
        {
            QChar ch = expressionInTE.at(i);
            //for debug , remove after implementation
            QString dbg(ch);          

            charType = getCharType(ch);
            if(stackStr.isEmpty())
            {
                stackStr.append(ch);
                currentStkType=charType;
                ui->expressionLE->setText(stackStr);
                continue;
            }

//            if(commaFunction)
//            {
//                if(currentStkType == SEPERATOR && charType != NUMBER)
//                    return false;
//            }


            if(currentStkType==NUMBER)
            {
                if(charType==NUMBER || ch =='e' || ch =='E' || ch=='.')
                {

                    if(stackStr.endsWith("E") || stackStr.endsWith(".") || stackStr.endsWith("e"))
                    {
                        stackStr.append(ch);
                        bool bIsNum;
                        stackStr.toDouble(&bIsNum);
                        if(bIsNum==false)
                            return false;
                    }
                    else
                        stackStr.append(ch);

                    ui->expressionLE->setText(stackStr);
                    continue;
                }
                else if (charType==VARIABLE)
                {
                    return false;
                }
                else if ( charType == OPERATOR)
                {
                    if(stackStr.endsWith("E") || stackStr.endsWith(".") || stackStr.endsWith("e"))
                    {
                        if(ch=='-')
                        {
                            stackStr.append(ch);
                            ui->expressionLE->setText(stackStr);
                            continue;
                        }
                        else
                            return false;
                    }
                    if (stackStr.endsWith("-"))
                    {
                        return false;
                    }

                    onAddExpPBClicked();
                    stackStr.clear();

                    previousStkType=NUMBER;
                    currentStkType = charType;
                    stackStr.append(ch);
                    continue;

                }
                else if(charType==SEPERATOR)
                {
                    if(stackStr.endsWith("E") || stackStr.endsWith(".") || stackStr.endsWith("e"))
                        return false;
                    if(_pExpression->getExpression().contains("pow") || _pExpression->getExpression().contains("min") || _pExpression->getExpression().contains("max"))
                        ignoreComma = true;
                    if(!ignoreComma && (ch == '(' || ch == ',')) //swarup
					{
						return false;
					}

					else 
                    {
                        onAddExpPBClicked();
                        stackStr.clear();
                        currentStkType = SEPERATOR;
                        previousStkType=NUMBER;
                        ui->expressionLE->setText(stackStr);
						
						if (_pExpression->getExpression().contains("pow") ||
							_pExpression->getExpression().contains("max") ||
							_pExpression->getExpression().contains("min"))//swarup
						{
							if (ch == '(')
								return false;
						}

                        if(ch == ',')
                        {
                            if(_pExpression->getExpression().contains("pow") && !_pExpression->getExpression().contains(','))
                            {
                                stackStr.append(ch);
                            }
                            else if(_pExpression->getExpression().contains("max") || _pExpression->getExpression().contains("min"))
                            {
                                stackStr.append(ch);
                            }
                            else
                                return false;
                        }

                        if(ch == ')')
                        {
                            if(insertClosedBraces() == false)
                                return false;
                        }
                        continue;
                    }
                }
            }

            else if(currentStkType==OPERATOR)
            {
                if(charType==NUMBER|| charType==VARIABLE|| charType==SEPERATOR)
                {
                    if(stackStr=="-" && (charType == NUMBER||charType==VARIABLE) && (previousStkType == OPERATOR || previousStkType == SEPERATOR)) // FIX for - inside ()
                    {
                        stackStr.append(ch);
                        currentStkType=charType;
                    }
                    else
                    {
                        GTATokenOperator* pOperator = new GTATokenOperator(stackStr);
                        insertTokenItem(pOperator);
                        stackStr.clear();
                        previousStkType=OPERATOR;
                        currentStkType=charType;
                        if(ch!=' ')
                            stackStr.append(ch);

                    }

                    //stkIsNumber=bIsNum;stkIsVar=bIsVarChar;stkIsFunc=false;=bIsSeperator;
                    if(charType!=SEPERATOR)
                        ui->expressionLE->setText(stackStr);

                    continue;
                }
                else if (charType==OPERATOR )
                {
                    if(ch == '-' )
                    {  
                        GTATokenOperator* pOperator = new GTATokenOperator(stackStr);
                        insertTokenItem(pOperator);
                        stackStr.clear();
                        stackStr.append(ch);
                        currentStkType=NUMBER;
                        ui->expressionLE->setText(stackStr);
                        previousStkType=OPERATOR;
                        continue;
                    }
                    else
                        return false;
                }

            }

            else if (currentStkType==SEPERATOR)
            {
                if (stackStr == "(" )
                {
                    GTATokenStartBracket* pBracket = new GTATokenStartBracket("(");
                    _pExpression->pushToken(pBracket);
                    stackStr.clear();


                }

                else if(stackStr == ",")//Swarup
                {
                    GTATokenCommaSeparator* pCommaSeparator = new GTATokenCommaSeparator(",");
                    _pExpression->pushToken(pCommaSeparator);
                    stackStr.clear();
                }

                else if (stackStr == ")")
                {
                    GTATokenEndBracket* pBracket = new GTATokenEndBracket(")");
                    if(! _pExpression->pushToken(pBracket))
                    {

                        delete pBracket;
                        return false;
                    }
                    //commaFunction = false;
                }
                stackStr.clear();   
                stackStr.append(ch);
                ui->expressionLE->setText(stackStr);
                currentStkType=charType;
                previousStkType=SEPERATOR;
                continue;

            }
            else if (currentStkType==VARIABLE)
            {
                if (charType==VARIABLE || charType==NUMBER)
                {
                    stackStr.append(ch);
                    ui->expressionLE->setText(stackStr);
                }
                else if (charType==SEPERATOR)
                {
                    int Arg =1;
                    if (isFunction(stackStr,Arg))
                    {
//                        if(stackStr.contains("min")||stackStr.contains("pow"))
//                            commaFunction = true;

                        previousStkType=FUNCTION;

                        GTATokenFunction* pFun = new GTATokenFunction(stackStr,Arg);
                        insertFunction(pFun);
                        stackStr.clear();
                        currentStkType = charType;
                        ui->expressionLE->clear();
                        stackStr.append(ch);
                        continue;


                    }
                    else
                    {
                        if(ch=='(' )
                            return false;

                        currentStkType=SEPERATOR;
                        previousStkType =VARIABLE;
                        onAddExpPBClicked();
                        stackStr.clear();
                        ui->expressionLE->clear();
                        stackStr.append(ch);
                        continue;
                    }
                }
                else if ( charType == OPERATOR)
                {

                    if (stackStr.endsWith("-"))
                    {
                        return false;
                    }

                    onAddExpPBClicked();
                    stackStr.clear();
                    ui->expressionLE->clear();
                    previousStkType=VARIABLE;
                    currentStkType = charType;
                    stackStr.append(ch);
                    continue;
                }


            }
            else if (currentStkType==kNa)
            {
                if(previousStkType == FUNCTION)
                {
                    if (charType==SEPERATOR)
                    {
                        if (ch == '(')
                        {
                            stackStr.clear();
                            _pExpression->pushToken(new GTATokenStartBracket("("));

                        }
                        else
                            return false;

                    }
                    else
                        return false;
                }
            }

        }// each charachter ////////////////////////////////////////////////




        //finalise with last charachter.

        if (currentStkType==NUMBER || currentStkType==VARIABLE)
        {
            onAddExpPBClicked();
            /*  stackStr.clear();
            ui->expressionLE->clear();*/
        }
        else if (currentStkType == OPERATOR)
        {

            if(previousStkType == OPERATOR && stackStr!="-")
                return false;
            GTATokenOperator* pOperator = new GTATokenOperator(stackStr);
            insertTokenItem(pOperator);
            stackStr.clear();
            ui->expressionLE->clear();
        }
        else if (currentStkType == SEPERATOR)
        {
            if(stackStr.trimmed().isEmpty()==false)
            {
                if (stackStr == "(" )
                {
                    GTATokenStartBracket* pBracket = new GTATokenStartBracket("(");
                    _pExpression->pushToken(pBracket);
                    stackStr.clear();

                }
                else if (stackStr == ")")
                {
                    GTATokenEndBracket* pBracket = new GTATokenEndBracket(")");
                    if(! _pExpression->pushToken(pBracket))
                    {

                        delete pBracket;
                        return false;
                    }
                }
            }
        }

    }
    else
    {
        ui->expressionLE->clear();
    }


    return rc;

}
GTAActionMathsWidget::ItemType GTAActionMathsWidget::getCharType(const QChar& ch)const
{
    ItemType type = kNa;
    if(isNumber(ch))
        type = NUMBER;
    else if (isVarChar(ch) )
    {
        type = VARIABLE;
    }
    else if (isSeperator(ch))
    {
        type = SEPERATOR;
    }
    else if (isOperator(ch))
    {
        type = OPERATOR;
    }

    return type;
}
bool GTAActionMathsWidget::isNumber(const QChar& ch)const
{
    bool rc = false;
    QString num(ch);
    num.toDouble(&rc);
    return rc;

}
bool GTAActionMathsWidget::isVarChar(const QChar& ch)const
{
    QRegExp rx("([A-Za-z.@_]+)");
    QString str(ch);
    bool status = str.contains(rx);
    return status;
}
bool GTAActionMathsWidget::isSeperator(const QChar& ch)const
{
    QRegExp rx("([\\s,()]+)");
    QString str(ch);
    bool status = str.contains(rx);
    return status;
}
bool GTAActionMathsWidget::isOperator(const QChar& ch)const
{
    QRegExp rx("([\\+-/*]+)");
    QString str(ch);
    bool status = str.contains(rx);
    return status;
}
bool GTAActionMathsWidget::isFunction(const QString& fn, int& oArguments)const
{
    QStringList lstOfFun = QStringList()<<MATH_FUN_EXP<<MATH_FUN_ABS<<MATH_FUN_SQRT<<MATH_FUN_ASIN<<MATH_FUN_ACOS<<MATH_FUN_ATAN<<MATH_FUN_SINH<<MATH_FUN_COSH<<MATH_FUN_TANH<<MATH_FUN_LOG10<<MATH_FUN_LOG2<<MATH_FUN_LOGE<<MATH_FUN_LOG1P<<MATH_FUN_CEIL<<MATH_FUN_RAND<<MATH_FUN_CBRT<<MATH_FUN_FLOOR<<MATH_FUN_ROUND<<MATH_FUN_SIN<<MATH_FUN_COS<<MATH_FUN_TAN<<MATH_FUN_POW<<MATH_FUN_MAX<<MATH_FUN_MIN;
    oArguments=1;
    return lstOfFun.contains(fn);
}
void GTAActionMathsWidget::addTextSilentlyToExpression(const QString& iTxt)
{
    disconnect(ui->expressionTE,SIGNAL(textChanged()),this,SLOT( onExpressionChange()));
    int cursorPosition = ui->expressionTE->textCursor().position();

    QString currTxt = ui->expressionTE->toPlainText();
    ui->expressionTE->setText(currTxt.append(iTxt));

    QTextCursor currentCursor = ui->expressionTE->textCursor();
    currentCursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,cursorPosition);
    ui->expressionTE->setTextCursor(currentCursor);

    connect(ui->expressionTE,SIGNAL(textChanged()),this,SLOT( onExpressionChange()));

}
void GTAActionMathsWidget::insertExpression(const QString& iExp)
{
    if(iExp.isEmpty()==false)
    {
        QString currentExpression = ui->expressionTE->toPlainText();
        QString sExp = currentExpression;
        sExp=sExp.trimmed();

        if(sExp.isEmpty())
        {
            currentExpression.append(QString("%1 ").arg(iExp));

        }      
        else
            currentExpression.append(QString(" %1 ").arg(iExp));
        ui->expressionTE->setText(currentExpression);
        ui->expressionTE->moveCursor(QTextCursor::End);
    }


}
#include "GTAGenSearchWidget.h"
int GTAActionMathsWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::PARAMETER;
    return retVal;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_MATHS,GTAActionMathsWidget,obj)

//
//void GTAActionMathsWidget::onCloseBracesPBClicked()
//{
//
//    GTAIToken* pLastToken = _pExpression->getLastItem();
//
//    /*
//    if last token is a function it implies that function is not preceeded by open bracket 
//    */
//    if(pLastToken->isFunction())
//    {
//        QMessageBox::information(this,"invalid input","Invalid input");
//        return;
//    }
//
//    if((pLastToken != NULL) &&  pLastToken->isEndBracket() )
//    {
//        if(_bracketStack.isEmpty())
//        {
//            QMessageBox::information(this,"invalid input","Unbalanced close bracket encountered.");
//            return;
//        }
//    }
//
//
//    GTATokenEndBracket* pBracket = new GTATokenEndBracket(")");
//    bool setFunctionClosed = false;
//
//    if(!_bracketStack.isEmpty())
//        _backSpaceStack.push(_bracketStack.pop());
//
//    if (!_functionStack.isEmpty())
//    {
//        GTAIToken* pTopItem = _functionStack.top();
//
//        if(_bracketStack.isEmpty())
//        {
//            GTAIToken* pToken = _functionStack.pop();
//            _backSpaceStack.push(pToken);
//            if(pToken->isFunction() &&_functionStack.isEmpty()==false)
//            {
//                do
//                {
//                    pToken = _functionStack.top();
//                    if(pToken->isStartBracket())
//                    {
//                        if(!_functionStack.isEmpty())
//                            _bracketStack.push(_functionStack.pop());
//                        if(_functionStack.isEmpty())
//                            break;
//                    }
//                } while (pToken->isFunction()==false  );
//
//                /*            GTATokenFunction* pFunction = dynamic_cast<GTATokenFunction*>(pToken);
//                if(pFunction)
//                { 
//                pFunction->pushToken(pBracket);
//                pFunction->setClosed(true);pushed=true;
//                }*/
//            }
//            /* else if (pToken ->isFunction())
//            {
//
//            }*/
//            setFunctionClosed=true;
//        }
//
//
//
//        if (pTopItem->isFunction() )
//        {
//            pTopItem->pushToken(pBracket);
//
//            GTATokenFunction* pFunction = dynamic_cast<GTATokenFunction*>(pTopItem);
//            if(pFunction!=NULL && setFunctionClosed)
//            {
//                pFunction->setClosed(true);
//            }
//        }
//        else if (pTopItem->isStartBracket())
//        {
//
//            _backSpaceStack.push(_functionStack.pop());
//        }
//
//
//
//    }
//    else
//        _pExpression->pushToken(pBracket);
//    updateExpression();
//}

