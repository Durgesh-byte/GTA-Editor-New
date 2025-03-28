#include "GTAElementTreeViewWidget.h"
#include "ui_GTAElementTreeViewWidget.h"


GTAElementTreeViewWidget::GTAElementTreeViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTAElementTreeViewWidget)
{
    setWindowFlags(Qt::Window);
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

   // QString stylesheet = QString(" QTableView::item { background-color: #646464;padding: 4px; border: 1px;border-style: solid;}");

   // QString stylesheet = QString("QTreeView::item {border: 0.5px solid;}");
    //ui->elementTV->setStyleSheet(stylesheet);
}
GTAElementTreeViewWidget::~GTAElementTreeViewWidget()
{
    if (nullptr != _pModel)
    {
        delete _pModel;
        _pModel=nullptr;
    }
    delete ui;
}

QString GTAElementTreeViewWidget::getName() const
{
    return QString();
}
void GTAElementTreeViewWidget::setName(const QString & iVal) 
{
    ui->elementNameLE->setText(iVal);

}
void GTAElementTreeViewWidget::setModel(QAbstractItemModel* pModel)
{
    if (NULL!=pModel)
    {
        _pModel=pModel;
        ui->elementTV->setModel(pModel);
        ui->elementTV->expandAll();
        ui->elementTV->resizeColumnToContents(0); 
        ui->elementTV->show();
    }
}
