#include "GTAKeyBoardShortcutsWidget.h"
#include "ui_GTAKeyBoardShortcutsWidget.h"
#include "GTAActionPrint.h"
#include "GTAParamValidator.h"
GTAKeyBoardShortcutsWidget::GTAKeyBoardShortcutsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTAKeyBoardShortcutsWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
}

GTAKeyBoardShortcutsWidget::~GTAKeyBoardShortcutsWidget()
{
    delete ui;
}