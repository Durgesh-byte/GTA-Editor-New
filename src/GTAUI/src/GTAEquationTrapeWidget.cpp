#include "GTAEquationTrapeWidget.h"
#include "ui_GTAEquationTrapeWidget.h"
#include "GTAEquationTrepeze.h"
GTAEquationTrapeWidget::GTAEquationTrapeWidget(QWidget *parent) :
    GTAEquationUIInterface(parent),
    ui(new Ui::GTAEquationTrapeWidget)
{
    ui->setupUi(this);

    _LevelWgtList.insert(1, ui->Level1LE);
    _LevelWgtList.insert(2, ui->Level2LE);
    _LevelWgtList.insert(3, ui->Level3LE);

    _LengthWgtList.insert(1, ui->Length1LE);
    _LengthWgtList.insert(2, ui->Length2LE);

    _RampWgtList.insert(1, ui->Ramp1LE);
    _RampWgtList.insert(2, ui->Ramp2LE);

    onSolidCBToggled(true);
}

GTAEquationTrapeWidget::~GTAEquationTrapeWidget()
{
    delete ui;
}

// GET Functions
int GTAEquationTrapeWidget::getRampCount()const
{
    return _RampWgtList.count();
}
int GTAEquationTrapeWidget::getLengthCount()const
{
    return _LengthWgtList.count();
}
int GTAEquationTrapeWidget::getLevelCount()const
{
    return _LevelWgtList.count();
}
QString GTAEquationTrapeWidget::getRamp(int Idx) const
{
    if(_RampWgtList.contains(Idx))
    {
        QLineEdit * pLine = (QLineEdit *)_RampWgtList.value(Idx);
        if(pLine != NULL)
        {
            return pLine->text();
        }
    }
    return QString();
}
QString GTAEquationTrapeWidget::getLevel(int Idx) const
{
    if(_LevelWgtList.contains(Idx))
    {
        QLineEdit * pLine = (QLineEdit *)_LevelWgtList.value(Idx);
        if(pLine != NULL)
        {
            return pLine->text();
        }
    }
    return QString();
}
QString GTAEquationTrapeWidget::getLength(int Idx) const
{
    if(_LengthWgtList.contains(Idx))
    {
        QLineEdit * pLine = (QLineEdit *)_LengthWgtList.value(Idx);
        if(pLine != NULL)
        {
            return pLine->text();
        }
    }
    return QString();
}

void GTAEquationTrapeWidget::insertLength(int index, const QString & iVal)
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

void GTAEquationTrapeWidget::insertLevel(int index, const QString & iVal)
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

void GTAEquationTrapeWidget::insertRamp(int index, const QString & iVal)
{
    if(_RampWgtList.contains(index))
    {
        QLineEdit * pEdit = (QLineEdit *)_RampWgtList.value(index);
        if(pEdit != NULL)
        {
            pEdit->setText(iVal);
        }
    }
}
void GTAEquationTrapeWidget::clear() const
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
    foreach(int key, _RampWgtList.keys())
    {
        QLineEdit * pItem = dynamic_cast<QLineEdit*>(_RampWgtList.value(key));
        if(pItem != NULL)
            pItem->clear();
    }
}

bool GTAEquationTrapeWidget::isValid()const
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
    foreach(int key, _RampWgtList.keys())
    {
        QLineEdit * pItem = dynamic_cast<QLineEdit*>(_RampWgtList.value(key));
        if(pItem != NULL && pItem->text().isEmpty())
            return false;
    }
    return true;
}

bool GTAEquationTrapeWidget::getEquation(GTAEquationBase*& pEqBase)const
{
    bool rc = false;
    GTAEquationTrepeze *pTrapeze = NULL;
    if (NULL != pEqBase)
    {
        pTrapeze = dynamic_cast<GTAEquationTrepeze *> (pEqBase);
    }
    else
    {
        pTrapeze = new GTAEquationTrepeze();
    }

    if (NULL != pTrapeze)
    {
        int lengthCount = this->getLengthCount();
        int levelCount = this->getLevelCount();
        int rampCount = this->getRampCount();
        for(int i = 1; i <= lengthCount; i++)
        {
            QString len = this->getLength(i);
            if(len.isEmpty())
                len = "0";

            pTrapeze->insertLength(i, len);
        }
        for(int j = 1; j <= levelCount; j++)
        {
            QString val = this->getLevel(j);
            if(val.isEmpty())
                val = "0";
            pTrapeze->insertLevel(j, val);
        }

        for(int k = 1; k <= rampCount; k++)
        {
            QString val = this->getRamp(k);
            if(val.isEmpty())
                val = "0";
            pTrapeze->insertRamp(k, val);
        }
        rc = true;
    }
    pEqBase = pTrapeze;
    return rc;
}

bool GTAEquationTrapeWidget::setEquation(GTAEquationBase*& pEqBase)
{
    bool rc = false;
    GTAEquationTrepeze * pEqn = dynamic_cast<GTAEquationTrepeze*>(pEqBase);
    if(pEqn != NULL)
    {
        int lenCount = pEqn->getLengthCount();
        int levCount = pEqn->getLevelCount();
        int rampCount = pEqn->getRampCount();
        for(int i = 1; i <= lenCount; i++)
        {
            QString val = pEqn->getLength(i);
            this->insertLength(i, val);
        }
        for(int i = 1; i <= levCount; i++)
        {
            QString val = pEqn->getLevel(i);
            this->insertLevel(i, val);
        }
        for(int i = 1; i <= rampCount; i++)
        {
            QString val = pEqn->getRamp(i);
            this->insertRamp(i, val);
        }
        rc = true;
    }
    else
        this->clear();
    return rc;
}


void GTAEquationTrapeWidget::onSolidCBToggled(bool iVal)
{
    if(iVal)
    {
        ui->Level1LE->clear();
    }
    else
    {
        ui->Level1LE->setText(EQ_CURRENT_VALUE);
    }
}

#include "GTAWgtRegisterDef.h"
REGISTER_EQUATION_WIDGET(EQ_TRAPEZE,GTAEquationTrapeWidget)
