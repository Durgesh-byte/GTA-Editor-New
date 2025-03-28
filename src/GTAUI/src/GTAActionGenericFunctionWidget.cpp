#include "GTAActionGenericFunctionWidget.h"
#include "ui_GTAActionGenericFunctionWidget.h"
#include "GTAActionManual.h"
#include <QMessageBox>
#include "GTAParamValidator.h"
#include "GTAParamValidator.h"
#include "GTAAppController.h"

#include <QDir>
#include <QDebug>


#define GFXN_XMLRPC     "xmlrpc"
#define GFXN_FUN        "function"
#define GFXN_RETURNVAL  "returnvalue"
#define GFXA_TOOLID     "tool_id"
#define GFXA_TOOLTYPE   "tool_type"
#define GFXA_NAME       "name"
#define GFXN_ARG        "argument"
#define GFXA_VAL        "value"
#define GFXA_TYPE       "type"
#define GFXN_ASSIGN     "assign"
#define GFXA_EXPR       "expr"
#define GFXA_LOC        "location"
#define GFXN_TRANS      "transition"
#define GFXA_COND       "cond"
#define GFXA_STATUS     "status"

GTAActionGenericFunctionWidget::GTAActionGenericFunctionWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionGenericFunctionWidget)
{
    ui->setupUi(this);
    connect(ui->ToolIDLE,SIGNAL(textChanged(QString)),this,SLOT(toolIDChanged(QString)));
    connect(ui->ToolTypeLE,SIGNAL(textChanged(QString)),this,SLOT(toolTypeChanged(QString)));
    connect(ui->functionNameLE,SIGNAL(textChanged(QString)),this,SLOT(funNameChanged(QString)));
    connect(ui->funReturnLE,SIGNAL(textChanged(QString)),this,SLOT(returnValueChanged(QString)));
    connect(ui->addArgumentPB,SIGNAL(clicked()),this,SLOT(onAddArgument()));
    connect(ui->editArgumentPB,SIGNAL(clicked()),this,SLOT(onEditArgument()));
    connect(ui->deleteArgumentPB,SIGNAL(clicked()),this,SLOT(onRemoveArgument()));
    connect(ui->ArgTypeCB,SIGNAL(currentIndexChanged (const QString&)),this,SLOT(argTypeChanged(const QString&)));
    connect(ui->failConditonLE,SIGNAL(textChanged(QString)),this,SLOT(onFailConditionChanged(QString)));
    connect(ui->successConditionLE,SIGNAL(textChanged(QString)),this,SLOT(onSuccessConditionChanged(QString)));
    connect(ui->editConditionCB,SIGNAL(toggled(bool)),this,SLOT(onEditCondtionChanged(bool)));
    //connect(ui->argumentLstLW,SIGNAL(clicked()),this,SLOT(onArgumentListClicked(QString)));
    connect(ui->argumentLstLW,SIGNAL(itemSelectionChanged()),this,SLOT(onArgumentListClicked()));
    ui->warningNoteLB->hide();
    initialize();
}

GTAActionGenericFunctionWidget::~GTAActionGenericFunctionWidget()
{
    //if(pManualAction != NULL)
    //{
    //    delete pManualAction;
    //    pManualAction = NULL;
    //}
    /*const QValidator* pValidator = ui->TitleLE->validator();
    if (pValidator!=NULL)
        delete pValidator;*/
    delete ui;
}
void GTAActionGenericFunctionWidget::initialize()
{
    GTAAppController* pAppController = GTAAppController::getGTAAppController();
    QString scxmlDirPath = pAppController->getSystemService()->getScxmlTemplateDir();

    QString genericTemplateFile = QDir::cleanPath( QString("%1%2%3").arg(scxmlDirPath,QDir::separator(),"Generic_Function.scxml") );

    
    QFile xmlFileObj(genericTemplateFile);
    bool rc = xmlFileObj.open(QFile::ReadOnly | QFile::Text);
    if(rc)
    {
        rc = _domDoc.setContent(&xmlFileObj);
        if(rc)
        {
            _stateNode = _domDoc.documentElement();

            QString text = _domDoc.toString(3);
            ui->previewTE->setText(text);
        }
    }


}
void  GTAActionGenericFunctionWidget::refreshPreviewSCXML()
{
    if(!_stateNode.isNull())
    {
        QDomDocument domDoc;
        QDomNode scxmlNode = domDoc.importNode(_stateNode,true);
        domDoc.appendChild(scxmlNode);

        QString text = domDoc.toString(3);
        ui->previewTE->setText(text);

    }

}
void GTAActionGenericFunctionWidget::toolIDChanged(QString iToolID)
{
    
    if (!_stateNode.isNull())
    {
        QDomNodeList xmlRPCLst = _stateNode.toElement().elementsByTagName(GFXN_XMLRPC);
        if (!xmlRPCLst.isEmpty())
        {
            xmlRPCLst.at(0).toElement().setAttribute(GFXA_TOOLID,iToolID);
        }
    }
    
    refreshPreviewSCXML();

}
void GTAActionGenericFunctionWidget::toolTypeChanged(QString iToolType)
{

    if (!_stateNode.isNull())
    {
        QDomNodeList xmlRPCLst = _stateNode.toElement().elementsByTagName(GFXN_XMLRPC);
        if (!xmlRPCLst.isEmpty())
        {
            xmlRPCLst.at(0).toElement().setAttribute(GFXA_TOOLTYPE,iToolType);
        }
    }

    refreshPreviewSCXML();

}
void GTAActionGenericFunctionWidget::funNameChanged(QString iName)
{
    if (!_stateNode.isNull())
    {
        QDomNodeList funNodeList = _stateNode.toElement().elementsByTagName(GFXN_FUN);
        if (!funNodeList.isEmpty())
        {
            funNodeList.at(0).toElement().setAttribute(GFXA_NAME,iName);
        }
    }

    refreshPreviewSCXML();
}
void GTAActionGenericFunctionWidget::returnValueChanged(QString iName)
{
    if (!_stateNode.isNull())
    {
        QDomNodeList domNodeList = _stateNode.toElement().elementsByTagName(GFXN_RETURNVAL);
        if (!domNodeList.isEmpty())
        {
            domNodeList.at(0).toElement().setAttribute(GFXA_NAME,iName);
        }

        domNodeList = _stateNode.toElement().elementsByTagName(GFXN_ASSIGN);
        if (!domNodeList.isEmpty())
        {
            QString exprValue = QString("_event.data.%1").arg(iName);
            domNodeList.at(0).toElement().setAttribute(GFXA_EXPR,exprValue);
            domNodeList.at(0).toElement().setAttribute(GFXA_LOC,iName);
        }

        domNodeList = _stateNode.toElement().elementsByTagName(GFXN_TRANS);
        if(ui->editConditionCB->isChecked()==false)
            if (!domNodeList.isEmpty())
            {
                QString exprValue = QString("%1.STATUS").arg(iName);
                domNodeList.at(0).toElement().setAttribute(GFXA_COND,exprValue);
                
            }
    }

    refreshPreviewSCXML();
}
void GTAActionGenericFunctionWidget::onAddArgument()
{

    QString argumentName    = ui->ArgNameLE->text();
    QString argumentValue   = ui->ArgValueLE->text();
    QString argumentType    = ui->ArgTypeCB->currentText();
    if(argumentType.isEmpty())
    {
        QMessageBox::information(this,"invalid input","Value Type is Empty");
        return;
    }

    if(argumentValue.isEmpty())
    {
        QMessageBox::information(this,"invalid input","Value is Empty");
        return;
    }

    if(argumentName.isEmpty())
    {
        QMessageBox::information(this,"invalid input","argument name is Empty");
        return;
    }

    /* bool isNumber;
    argumentValue.toDouble(&isNumber);
    if (isNumber==false)
    {
        if (argumentValue.startsWith("'")==false || argumentValue.endsWith("'")==false)
        {
            QMessageBox::information(this,"invalid input","argument value is alphaNumeric it should be enclosed in single quote");
            return;
        }
        else if (argumentValue.count("'")!=2)
        {

                QMessageBox::information(this,"invalid input","argument value is alphaNumeric it should be enclosed in single quote");
                return;

        }

    }*/

    /*if (argumentType.toLower() == "string" || argumentType.toLower()=="char")
    {
        if (argumentValue.startsWith("'")==false || argumentValue.endsWith("'")==false)
        {
            QMessageBox::information(this,"invalid input",QString("argument value is of type %1 it should be enclosed in single quote").arg(argumentType));
            return;
        }
        else if (argumentValue.count("'")!=2)
        {

            QMessageBox::information(this,"invalid input",QString("argument value is of type %1 it should be enclosed in single quote").arg(argumentType));
            return;

        }
    }*/
    if (!_stateNode.isNull())
    {
        QDomNodeList funNodeList = _stateNode.toElement().elementsByTagName(GFXN_FUN);
        if (!funNodeList.isEmpty())
        {
            QDomElement functionElem = funNodeList.at(0).toElement();
            if (!functionElem.isNull())
            {
                QDomElement argumentElem = _domDoc.createElement(GFXN_ARG);
                argumentElem.setAttribute(GFXA_NAME,argumentName);
                argumentElem.setAttribute(GFXA_VAL,argumentValue);
                argumentElem.setAttribute(GFXA_TYPE,argumentType);
                QDomNode returnElem = _stateNode.toElement().elementsByTagName(GFXN_RETURNVAL).at(0).toElement();
                functionElem.insertBefore(argumentElem,returnElem);
                int currentItemCnt = ui->argumentLstLW->count();
                QString argumentIDX = QString::number(++currentItemCnt);
                ui->argumentLstLW->addItem(QString("ARG%1 [name:%2, value:%3, type:%4]").arg(argumentIDX,argumentName,argumentValue,argumentType));
            }
        }
    }

    refreshPreviewSCXML();
}
void GTAActionGenericFunctionWidget::onEditArgument()
{

    QString argumentName    = ui->ArgNameLE->text();
    QString argumentValue   = ui->ArgValueLE->text();
    QString argumentType    = ui->ArgTypeCB->currentText();
    if(argumentType.isEmpty())
    {
        QMessageBox::information(this,"invalid input","Value Type is Empty");
        return;
    }

    if(argumentValue.isEmpty())
    {
        QMessageBox::information(this,"invalid input","Value is Empty");
        return;
    }

    if(argumentName.isEmpty())
    {
        QMessageBox::information(this,"invalid input","argument name is Empty");
        return;
    }

    QList<QListWidgetItem *> selectedIndexes = ui->argumentLstLW->selectedItems();
    if (selectedIndexes.isEmpty())
    {
        QMessageBox::information(this,"invalid input","Select argument to be changed");
        return;
    }

    int indexToEdit = ui->argumentLstLW->row(selectedIndexes.at(0));

    if (!_stateNode.isNull())
    {
        QDomNodeList argumentNodeList = _stateNode.toElement().elementsByTagName(GFXN_ARG);
        if (!argumentNodeList.isEmpty())
        {
            for (int i=0;i<argumentNodeList.size();i++)
            {
                if (i==indexToEdit)
                {
                    QDomElement argumentToEdit = argumentNodeList.at(i).toElement();
                    argumentToEdit.setAttribute(GFXA_NAME,argumentName);
                    argumentToEdit.setAttribute(GFXA_VAL,argumentValue);
                    argumentToEdit.setAttribute(GFXA_TYPE,argumentType);
                    QListWidgetItem* pItem =ui->argumentLstLW->item(indexToEdit);
                    QString argumentIDX = QString::number(indexToEdit);
                    pItem->setText(QString("ARG%1 [name:%2, value:%3, type:%4]").arg(argumentIDX,argumentName,argumentValue,argumentType));


                }
            }
        }

    }

    refreshPreviewSCXML();
}
void GTAActionGenericFunctionWidget::onRemoveArgument()
{


    
    QList<QListWidgetItem *> selectedIndexes = ui->argumentLstLW->selectedItems();
    if (selectedIndexes.isEmpty())
    {
        QMessageBox::information(this,"invalid input","Select argument to be deleted");
        return;
    }

    int indexToEdit = ui->argumentLstLW->row(selectedIndexes.at(0));

    //int idx=0;
    if (!_stateNode.isNull())
    {
        QDomNodeList argumentNodeList = _stateNode.toElement().elementsByTagName(GFXN_ARG);
        if (!argumentNodeList.isEmpty())
        {
            for (int i=0;i<argumentNodeList.size();i++)
            {

                QDomElement argumentToEdit = argumentNodeList.at(i).toElement();
                if (i==indexToEdit)
                {

                    QDomNode parent = argumentToEdit.parentNode();
                    parent.removeChild(argumentToEdit) ;
                    QListWidgetItem* pItem =ui->argumentLstLW->item(indexToEdit);
                    delete pItem;


                }
                
            }
        }

    }


    QString argumentName    ;
    QString argumentValue  ;
    QString argumentType    ;
    ui->argumentLstLW->clear();
    if (!_stateNode.isNull())
    {
        QDomNodeList argumentNodeList = _stateNode.toElement().elementsByTagName(GFXN_ARG);
        if (!argumentNodeList.isEmpty())
        {
            for (int i=0;i<argumentNodeList.size();i++)
            {

                QDomElement argumentToEdit = argumentNodeList.at(i).toElement();
                argumentName=argumentToEdit.attribute(GFXA_NAME);
                argumentValue=argumentToEdit.attribute(GFXA_VAL);
                argumentType=argumentToEdit.attribute(GFXA_TYPE);
				ui->argumentLstLW->item(indexToEdit);
                QString argumentIDX = QString::number(i);
                ui->argumentLstLW->addItem(QString("ARG%1 [name:%2, value:%3, type:%4]").arg(argumentIDX,argumentName,argumentValue,argumentType));



            }
        }

    }
    

    refreshPreviewSCXML();
}
void GTAActionGenericFunctionWidget::clear() 
{
    ui->ToolIDLE->clear();
    ui->functionNameLE->clear();
    ui->ArgNameLE->clear();
    ui->ToolTypeLE->clear();
    ui->ArgValueLE->clear();
    ui->ArgTypeCB->setCurrentIndex(0);
    ui->argumentLstLW->clear();
    ui->commandNameLE->clear();
    ui->funReturnLE->clear();
    initialize();


}
bool GTAActionGenericFunctionWidget::isValid() const
{
    if(ui->commandNameLE->text().isEmpty() ||
            ui->ToolIDLE->text().isEmpty() ||
            ui->functionNameLE->text().isEmpty() ||
            ui->ToolTypeLE->text().isEmpty() ||
            ui->funReturnLE->text().isEmpty() ||_domDoc.isNull())
        return false;

    if (ui->editConditionCB->isChecked())
    {
        if (ui->successConditionLE->text().isEmpty() )
        {
            return false;
        }
    }
    if (_stateNode.isNull())
    {
        return false;
    }
    else
    {
        QDomNodeList argNodes = _stateNode.toElement().elementsByTagName(GFXN_ARG);
        if(argNodes.isEmpty())
        {
            _LastError = "At least one argument should be present";
            return false;
        }

    }

    return true;
}
bool GTAActionGenericFunctionWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc=false;

    GTAActionGenericFunction* pGenFun=NULL;
    if (isValid())
    {
        
        pGenFun=new GTAActionGenericFunction;
        pGenFun->setFunctionName(ui->commandNameLE->text());
        pGenFun->setAction(ACT_GEN_FUNC);
        pGenFun->setFuncReturnName(ui->funReturnLE->text());

        QString txt = _domDoc.toString();
        pGenFun->setSCXMLText(txt);
        if (ui->editConditionCB->checkState()==Qt::Checked)
            pGenFun->setUsercondition(true);
        else
            pGenFun->setUsercondition(false);

        rc=true;
        
    }

    opCmd = pGenFun;
    return rc;
    
}
void GTAActionGenericFunctionWidget::setActionCommand(const GTAActionBase *ipActionCmd )
{
    GTAActionGenericFunction* pGenFuncCmd = dynamic_cast<GTAActionGenericFunction*>((GTAActionBase *)ipActionCmd);
    if (NULL!=pGenFuncCmd)
    {
        ui->commandNameLE->setText(pGenFuncCmd->getFunctionName());
        bool hasUserCond = pGenFuncCmd->hasUsercondiiton();
        ui->editConditionCB->setChecked(hasUserCond);
        
        QString text = pGenFuncCmd->getSCXMLText();
        QDomDocument domDoc;
        bool rc = domDoc.setContent(text.toLatin1());

        if (rc)
        {
            _domDoc = domDoc;
            text =domDoc.toString(3);
            ui->previewTE->setText(text);
            _stateNode = _domDoc.documentElement();
            refreshPreviewSCXML();

        }
        if (!_stateNode.isNull())
        {
            QDomNodeList xmlRPCLst = _stateNode.toElement().elementsByTagName(GFXN_XMLRPC);
            if (!xmlRPCLst.isEmpty())
            {
                QString sToolID = xmlRPCLst.at(0).toElement().attribute(GFXA_TOOLID);
                QString sToolType = xmlRPCLst.at(0).toElement().attribute(GFXA_TOOLTYPE);

                ui->ToolIDLE->setText(sToolID);
                ui->ToolTypeLE->setText(sToolType);
            }
            
            QDomNodeList funNodeList = _stateNode.toElement().elementsByTagName(GFXN_FUN);
            if (!funNodeList.isEmpty())
            {
                QString sFuncName = funNodeList.at(0).toElement().attribute(GFXA_NAME);
                ui->functionNameLE->setText(sFuncName);
            }
            
            QDomNodeList returnValLst = _stateNode.toElement().elementsByTagName(GFXN_RETURNVAL);
            if (!returnValLst.isEmpty())
            {
                QString returnName = returnValLst.at(0).toElement().attribute(GFXA_NAME);
                ui->funReturnLE->setText(returnName);
            }

            QDomNodeList argumentList = _stateNode.toElement().elementsByTagName(GFXN_ARG);
            ui->argumentLstLW->clear();
            for (int i=0;i<argumentList.size();i++)
            {
                QDomElement argumentElem = argumentList.at(i).toElement();
                QString argumentIDX = QString::number(i+1);
                QString argumentName=argumentElem.attribute(GFXA_NAME);
                QString argumentValue=argumentElem.attribute(GFXA_VAL);
                QString argumentType=argumentElem.attribute(GFXA_TYPE);;

                QString stm = QString("ARG%1 [name:%2, value:%3, type:%4]").arg(argumentIDX,argumentName,argumentValue,argumentType);
                ui->argumentLstLW->addItem(stm);

            }
            if(hasUserCond)
            {
                QDomNodeList transisitonNodes = _stateNode.toElement().elementsByTagName(GFXN_TRANS);
                int noOfTXNNodes = transisitonNodes.size();
                for (int i=0;i<noOfTXNNodes;i++)
                {
                    QDomElement txnElement = transisitonNodes.at(i).toElement();
                    if(txnElement.isNull()==false)
                    {
                        QString status = txnElement.attribute(GFXA_STATUS);
                        QString cond = txnElement.attribute(GFXA_COND);
                        if (status=="success")
                        {
                            ui->successConditionLE->setText(cond);
                        }
                        else if (status == "fail")
                        {
                            ui->failConditonLE->setText(cond);
                        }

                    }
                }
            }

            refreshPreviewSCXML();
        }
    }
    else
        clear();



}
void GTAActionGenericFunctionWidget::onSuccessConditionChanged(QString iSuccessCond)
{
    if (!_stateNode.isNull())
    {
        QDomNodeList funNodeList = _stateNode.toElement().elementsByTagName(GFXN_TRANS);
        if (!funNodeList.isEmpty())
        {
            int childSize = funNodeList.size();
            
            for(int i=0;i<childSize;i++)
            {
                QDomElement elem =  funNodeList.at(i).toElement();
                QString sStatus = elem.attribute(GFXA_STATUS);
                if (sStatus == "success")
                {
                    if(!iSuccessCond.isEmpty())
                    {
                        funNodeList.at(i).toElement().setAttribute(GFXA_COND,iSuccessCond);
                    }
                    else
                        funNodeList.at(i).toElement().removeAttribute(GFXA_COND);
                }
            }


        }


    }
    
    refreshPreviewSCXML();
}
void GTAActionGenericFunctionWidget::onFailConditionChanged(QString iFailCondition)
{
    if (!_stateNode.isNull())
    {
        QDomNodeList funNodeList = _stateNode.toElement().elementsByTagName(GFXN_TRANS);
        if (!funNodeList.isEmpty())
        {
            int childSize = funNodeList.size();
            if (childSize==1)
            {
                QDomElement failTXNode = funNodeList.at(0).cloneNode(true).toElement();
                _stateNode.appendChild(failTXNode);
                failTXNode.setAttribute(GFXA_STATUS,"fail");
                
            }

            childSize = funNodeList.size();
            
            for(int i=0;i<childSize;i++)
            {
                QDomElement elem =  funNodeList.at(i).toElement();
                QString sStatus = elem.attribute(GFXA_STATUS);
                if (sStatus == "fail")
                {
                    if(!iFailCondition.isEmpty())
                    {
                        funNodeList.at(i).toElement().setAttribute(GFXA_COND,iFailCondition);
                    }
                    else
                        _stateNode.removeChild(funNodeList.at(i));
                }
            }
            


        }


    }

    refreshPreviewSCXML();
}
void GTAActionGenericFunctionWidget::onArgumentListClicked()
{
    int itemsCount = ui->argumentLstLW->count();
    int selectedItem =0;
    for (int i=0;i<itemsCount;i++)
    {
        QListWidgetItem* pItem = ui->argumentLstLW->item(i);
        if (pItem->isSelected())
        {
            selectedItem = i;
            break;
        }
    }


    if (!_stateNode.isNull())
    {
        QDomNodeList argumentNodeList = _stateNode.toElement().elementsByTagName(GFXN_ARG);
        if (!argumentNodeList.isEmpty())
        {
            for (int i=0;i<argumentNodeList.size();i++)
            {
                if (i==selectedItem)
                {
                    QDomElement argumentToEdit = argumentNodeList.at(i).toElement();
                    ui->ArgNameLE->setText(argumentToEdit.attribute(GFXA_NAME));
                    ui->ArgValueLE->setText(argumentToEdit.attribute(GFXA_VAL));
                    QString argType = argumentToEdit.attribute(GFXA_TYPE);

                    int index = ui->ArgTypeCB->findText(argType);

                    if(index>=0)
                        ui->ArgTypeCB->setCurrentIndex(index);
                    break;

                }
            }
        }

    }


}

void GTAActionGenericFunctionWidget::argTypeChanged(const QString& iName)
{
    if (iName.toLower()=="string" || iName.toLower()=="char")
        setNote(iName,true);
    else
        setNote(QString(),false);
    
}
void GTAActionGenericFunctionWidget::setNote(const QString& inputString,bool iShow)
{
    ui->warningNoteLB->setVisible(iShow);
    if(iShow)
    {
        QString htmlCode = QString ( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; font-weight:600; color:#ff0000;\">Please note arg value should be enclosed in quotes since argument type is $$$</span></p></body></html>");
        htmlCode.replace("$$$",inputString);
        ui->warningNoteLB->setText(QApplication::translate("GTAActionGenericFunctionWidget",htmlCode.toLatin1()));
    }
    


}
void GTAActionGenericFunctionWidget::onEditCondtionChanged(bool checkState)
{

    QDomNodeList transisitonNodes = _stateNode.toElement().elementsByTagName(GFXN_TRANS);
    int noOfTXNNodes = transisitonNodes.size();
    for (int i=0;i<noOfTXNNodes;i++)
    {
        QDomElement txnElement = transisitonNodes.at(i).toElement();
        if(txnElement.isNull()==false)
        {
            QString status = txnElement.attribute(GFXA_STATUS);
            if(checkState==false)
            {
                if (status == "fail")
                {
                    _stateNode.removeChild(txnElement);
                }
                if (status == "success")
                {
                    QString returnVal = ui->funReturnLE->text();
                    if(!returnVal.isEmpty())
                        txnElement.setAttribute(GFXA_COND,QString("%1.STATUS").arg(returnVal));
                }
            }
            else
            {
                //change text already connected so resetting text will take care of scxml.
                QString sucessText  = ui->successConditionLE->text();
                QString failCondText = ui->failConditonLE->text();
                if(sucessText.isEmpty()==false)
                    onSuccessConditionChanged(sucessText);
                if (failCondText.isEmpty()==false)
                    onFailConditionChanged(failCondText);
                

            }



        }
    }

    refreshPreviewSCXML();

    
    

}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_GEN_FUNC,GTAActionGenericFunctionWidget,obj)
