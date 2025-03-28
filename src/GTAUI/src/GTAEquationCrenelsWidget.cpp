#include "GTAEquationCrenelsWidget.h"
#include "ui_GTAEquationCrenelsWidget.h"
#include "GTAEquationCrenels.h"

GTAEquationCrenelsWidget::GTAEquationCrenelsWidget(QWidget *parent) :
    GTAEquationUIInterface(parent),
    ui(new Ui::GTAEquationCrenelsWidget)
{
    ui->setupUi(this);

    _LevelWgtList.insert(1,ui->Level1LE);
    _LevelWgtList.insert(2,ui->Level2LE);
    _LevelWgtList.insert(3,ui->Level3LE);
    _LevelWgtList.insert(4,ui->Level4LE);
    _LevelWgtList.insert(5,ui->Level5LE);

    _LengthWgtList.insert(1,ui->Length1LE);
    _LengthWgtList.insert(2,ui->Length2LE);
    _LengthWgtList.insert(3,ui->Length3LE);
    _LengthWgtList.insert(4,ui->Length4LE);
}

GTAEquationCrenelsWidget::~GTAEquationCrenelsWidget()
{
    delete ui;
}

int GTAEquationCrenelsWidget::getLengthCount()const
{
     return _LengthWgtList.count();
}
int GTAEquationCrenelsWidget::getLevelCount()const
{
     return _LevelWgtList.count();
}
QString GTAEquationCrenelsWidget::getLength(int iIdx) const
{
    if(_LengthWgtList.contains(iIdx))
    {
        QLineEdit * pLine = (QLineEdit *)_LengthWgtList.value(iIdx);
        if(pLine != NULL)
        {
            return pLine->text();
        }
    }
    return QString();
}

QString GTAEquationCrenelsWidget::getLevel(int iIdx) const
{
    if(_LevelWgtList.contains(iIdx))
    {
        QLineEdit * pLine = (QLineEdit *)_LevelWgtList.value(iIdx);
        if(pLine != NULL)
        {
            return pLine->text();
        }
    }
    return QString();
}
void GTAEquationCrenelsWidget::insertLength(int index, const QString & iVal)
{
    if(_LengthWgtList.contains(index))
    {
        QLineEdit * pEdit = (QLineEdit *)_LengthWgtList.value(index);
        if(pEdit != NULL)
        {
            pEdit->setText(iVal);
        }
    }
}

void GTAEquationCrenelsWidget::insertLevel(int index, const QString & iVal)
{
    if(_LevelWgtList.contains(index))
    {
        QLineEdit * pEdit = (QLineEdit *)_LevelWgtList.value(index);
        if(pEdit != NULL)
        {
            pEdit->setText(iVal);
        }
    }
}

void GTAEquationCrenelsWidget::clear() const
{
    foreach(int key, _LevelWgtList.keys())
    {
        QLineEdit * pItem = dynamic_cast<QLineEdit*>(_LevelWgtList.value(key));
        if(pItem != NULL)
            pItem->clear();
    }
    foreach(int key, _LengthWgtList.keys())
    {
        QLineEdit * pItem = dynamic_cast<QLineEdit*>(_LengthWgtList.value(key));
        if(pItem != NULL)
            pItem->clear();
    }
}

bool GTAEquationCrenelsWidget::isValid() const
{
    foreach(int key, _LevelWgtList.keys())
    {
        QLineEdit * pItem = dynamic_cast<QLineEdit*>(_LevelWgtList.value(key));
        if(pItem != NULL && pItem->text().isEmpty())
            return false;
    }
    foreach(int key, _LengthWgtList.keys())
    {
        QLineEdit * pItem = dynamic_cast<QLineEdit*>(_LengthWgtList.value(key));
        if(pItem != NULL && pItem->text().isEmpty())
            return false;
    }
    return true;
}

bool GTAEquationCrenelsWidget::getEquation(GTAEquationBase*& pEqBase)const
{
    GTAEquationCrenels * pCrenels = NULL;
    bool rc = false;
    if (pEqBase != NULL)
    {
        pCrenels = dynamic_cast<GTAEquationCrenels *> (pEqBase);
    }
    else
    {
        pCrenels= new GTAEquationCrenels();
    }

    if (NULL != pCrenels)
    { 
        int levelCount = this->getLevelCount();
        int lenCount = this->getLengthCount();
        for(int i = 1; i <= lenCount; i++)
        {
            QString val = this->getLength(i);
            if(val.isEmpty())
                val = "0";

            pCrenels->insertLength(i, val);
        }
        for(int i = 1; i <= levelCount; i++)
        {
            QString val = this->getLevel(i);
            if(val.isEmpty())
                val = "0";
            pCrenels->insertLevel(i, val);
        } 
        rc = true;
    }
    pEqBase = pCrenels;
    return rc;
}

bool GTAEquationCrenelsWidget::setEquation(GTAEquationBase*& pEqBase)
{
    bool rc = false;
    GTAEquationCrenels* pEqn = dynamic_cast<GTAEquationCrenels*>(pEqBase);
    if(pEqn != NULL)
    {
        int lenCount = pEqn->getLengthCount();
        int levCount = pEqn->getLevelCount();
        for(int i = 1 ; i <= lenCount; i++)
        {
            QString val = pEqn->getLength(i);
            this->insertLength(i, val);
        }

        for(int i = 1 ; i <= levCount; i++)
        {
            QString val = pEqn->getLevel(i);
            this->insertLevel(i, val);
        }
        rc = true;
    }
    else
        this->clear();
    return rc;
}


void GTAEquationCrenelsWidget::onSolidCBToggled(bool)
{

}

#include "GTAWgtRegisterDef.h"
REGISTER_EQUATION_WIDGET(EQ_CRENELS,GTAEquationCrenelsWidget)
