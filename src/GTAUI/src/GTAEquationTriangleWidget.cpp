#include "GTAEquationTriangleWidget.h"
#include "GTAEquationTriangle.h"
#include "ui_GTAEquationTriangleWidget.h"

GTAEquationTriangleWidget::GTAEquationTriangleWidget(QWidget *parent) :
GTAEquationUIInterface(parent),
ui(new Ui::GTAEquationTriangleWidget)
{
    ui->setupUi(this);
}

GTAEquationTriangleWidget::~GTAEquationTriangleWidget()
{
    delete ui;
}

// GET Functions
QString GTAEquationTriangleWidget::getPeriod() const
{
    return ui->periodLE->text();
}
QString GTAEquationTriangleWidget::getMinValue() const
{
    return ui->minValueLE->text();
}
QString GTAEquationTriangleWidget::getMaxValue() const
{
    return ui->maxValueLE->text();
}
QString GTAEquationTriangleWidget::getStartValue() const
{
    return ui->startValueLE->text();
}
QString GTAEquationTriangleWidget::getDirection() const
{
    QString sDirection = ui->directionCB->currentText();
    return sDirection;
}

// SET Functions
void GTAEquationTriangleWidget::setPeriod(const QString & iPeriod)
{
    ui->periodLE->setText(iPeriod);
}
void GTAEquationTriangleWidget::setMinValue(const QString & iMin)
{
    ui->minValueLE->setText(iMin);
}
void GTAEquationTriangleWidget::setMaxValue(const QString & iMax)
{
    ui->maxValueLE->setText(iMax);
}
void GTAEquationTriangleWidget::setStartValue(const QString & iStart)
{
    ui->startValueLE->setText(iStart);
}
void GTAEquationTriangleWidget::setDirection(const QString & iDirection)
{
    if(ui->directionCB->findText(iDirection))
     {
         int itemCount = ui->directionCB->count();
         for (int i=0;i<itemCount;i++)
         {
             if (ui->directionCB->itemText(i) == iDirection)
             {
                 ui->directionCB->setCurrentIndex(i);
             }
         }
     }
}

bool GTAEquationTriangleWidget::isValid()const
{
    if( ui->periodLE->text().isEmpty() || ui->minValueLE->text().isEmpty()  ||
        ui->maxValueLE->text().isEmpty()  || ui->startValueLE->text().isEmpty() )
    {
        return false;
    }
    else
        return true;
}
void GTAEquationTriangleWidget::clear() const
{
    ui->periodLE->clear();
    ui->minValueLE->clear();
    ui->maxValueLE->clear();
    ui->startValueLE->clear();
    ui->directionCB->setCurrentIndex(0);
}
bool GTAEquationTriangleWidget::getEquation(GTAEquationBase*& pEqBase)const
{
    bool rc = false;
    GTAEquationTriangle* pEqnTriangle = NULL;
    if (NULL != pEqBase)
    {
        pEqnTriangle = dynamic_cast<GTAEquationTriangle*> (pEqBase);

    }
    else
    {
        pEqnTriangle = new GTAEquationTriangle();

    }
    if (NULL != pEqnTriangle)
    {
        pEqnTriangle->setPeriod(getPeriod());
        pEqnTriangle->setMinValue(getMinValue());
        pEqnTriangle->setMaxValue(getMaxValue());
        pEqnTriangle->setStartValue(getStartValue());
        pEqnTriangle->setDirection(getDirection());
        rc = true;
    }
    pEqBase = pEqnTriangle;
    return rc;
}

bool GTAEquationTriangleWidget::setEquation(GTAEquationBase*& pEqBase)
{

    GTAEquationTriangle* pEqnTriangle = NULL;
    if (NULL != pEqBase)
    {
        pEqnTriangle = dynamic_cast<GTAEquationTriangle*> (pEqBase);
        if (NULL != pEqnTriangle)
        {
            setPeriod(pEqnTriangle->getPeriod());
            setMinValue(pEqnTriangle->getMinValue());
            setMaxValue(pEqnTriangle->getMaxValue());
            setStartValue(pEqnTriangle->getStartValue());
            setDirection(pEqnTriangle->getDirection());
        }
    }
    else
    {
        clear();
    }
    return true;
}

 void GTAEquationTriangleWidget::onSolidCBToggled(bool) {}

#include "GTAWgtRegisterDef.h"
REGISTER_EQUATION_WIDGET(EQ_TRIANGLE, GTAEquationTriangleWidget)
