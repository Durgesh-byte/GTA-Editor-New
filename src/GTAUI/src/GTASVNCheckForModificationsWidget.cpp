#include "GTASVNCheckForModificationsWidget.h"
#include "ui_GTASVNCheckForModificationsWidget.h"
#include<QDebug>
#include<QTableWidget>

GTASVNCheckForModificationsWidget::GTASVNCheckForModificationsWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::GTASVNCheckForModificationsWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint);
    setWindowModality(Qt::ApplicationModal);
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(close()));
    ui->svnCheckForModificationsTW->horizontalHeader()->setStretchLastSection(true);
}

GTASVNCheckForModificationsWidget::~GTASVNCheckForModificationsWidget()
{
    delete ui;
}

void GTASVNCheckForModificationsWidget::updateItemStyle(QTableWidgetItem *&oitem, QString &text)
{
    bool ignored = false;
    QFont font;
    if(text.contains("<isIgnoredInSCXML>"))
    {
        ignored = true;
        font.setStrikeOut(true);

        text.remove("<isIgnoredInSCXML>");
    }
    if(text.contains("<CHECK>"))
    {
        oitem->setForeground(QBrush(QColor(0,128,0,200)));
        text.remove("<CHECK>");
    }
    if(text.contains("<TITLE>"))
    {
        oitem->setForeground(QBrush(TITLE_EDITOR_FORE_COLOR));
        oitem->setBackgroundColor(TITLE_EDITOR_BACK_COLOR);
        font.setBold(true);
        font.setWeight(QFont::Bold);
        text.remove("<TITLE>");
    }
    if(text.contains("<ACT_PRINT>"))
    {
        text.remove("<ACT_PRINT>");
        oitem->setForeground(QBrush(QColor(0,0,250)));
        font.setBold(true);
    }
    if(text.contains("<COLORED>"))
    {
        text.remove("<COLORED>");
        oitem->setBackgroundColor(QColor(251,130,89));
    }

    oitem->setFont(font);

}

void GTASVNCheckForModificationsWidget::displayList(QStringList iOutputDiff)
{
    ui->svnCheckForModificationsTW->setColumnCount(4);
    QStringList header;
    header<<"Line"<<"Previous version"<<"Line"<<"Working Copy";
    ui->svnCheckForModificationsTW->setHorizontalHeaderLabels(header);
    ui->svnCheckForModificationsTW->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ui->svnCheckForModificationsTW->setColumnWidth(1,800);
    ui->svnCheckForModificationsTW->setColumnWidth(3,800);
    ui->svnCheckForModificationsTW->setColumnWidth(0,50);
    ui->svnCheckForModificationsTW->setColumnWidth(2,50);
    // ui->svnCheckForModificationsTW->verticalHeader()->setVisible(false);
    ui->svnCheckForModificationsTW->setSelectionMode(QAbstractItemView::NoSelection);
    ui->svnCheckForModificationsTW->setRowCount(0);
    ui->svnCheckForModificationsTW->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->svnCheckForModificationsTW->verticalHeader()->setForegroundRole(ui->svnCheckForModificationsTW->verticalHeader()->backgroundRole());
    int left = 0;
    int right = 0;

    double leftstart = 0;
    double rightstart = 0;
    int leftcount = 0;
    int rightcount = 0;
    bool nodiff =false;
    bool diff;

    foreach(QString str, iOutputDiff)
    {
        if(str.startsWith("---") || str.startsWith("+++"))
        {
            //Ignore
        }
        else if(str.startsWith("@@ "))
        {

            if(nodiff)
            {
                left++;
                ui->svnCheckForModificationsTW->setRowCount((left>=right)?left:right);
                ui->svnCheckForModificationsTW->setSpan((left-1),0,1,4);
                QTableWidgetItem *item = new QTableWidgetItem("--- Next Difference ---");
                item->setBackground(QColor(0,0,0));
                item->setForeground(QBrush(QColor(255,255,255)));
                QFont font;
                font.setBold(true);
                item->setFont(font);
                ui->svnCheckForModificationsTW->setItem((left-1),0,item);
                /*item = new QTableWidgetItem("");*//*
                ui->svnCheckForModificationsTW->setItem((left-1),0,item);
                ui->svnCheckForModificationsTW->setItem((left-1),2,item);
                ui->svnCheckForModificationsTW->setItem((left-1),3,item);*/

                right++;
            }
            diff = false;
            nodiff = true;
            rightcount= 0;
            leftcount = 0;
            str.remove("@@ -");
            QString leftstartstr = str.split(",").at(0);
            leftstart = leftstartstr.toDouble();
            QString temp = str.split("+").last();
            QString rightstartstr;
            if(temp.contains(","))
            {
                rightstartstr = temp.split(",").at(0);
            }
            else
                rightstartstr = temp.split(" @@").at(0);
            rightstart = rightstartstr.toDouble();
        }
        else if(str.startsWith(" "))
        {
            str.remove(" ").at(0);
            while(leftcount != rightcount)
            {
                if(leftcount < rightcount)
                {
                    leftcount++;
                    //left.append("");
                    left++;
                    ui->svnCheckForModificationsTW->setRowCount((left >= right) ? left : right);
                    QTableWidgetItem *item = new QTableWidgetItem("");
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                    ui->svnCheckForModificationsTW->setItem((left-1),1,item);

                }
                else
                {
                    rightcount++;
                    // right.append("");
                    right++;
                    ui->svnCheckForModificationsTW->setRowCount((left >= right) ? left : right);
                    QTableWidgetItem *item = new QTableWidgetItem("");
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                    ui->svnCheckForModificationsTW->setItem((right-1),3,item);
                }
            }

            //left.append(QString::number(leftstart++) + "<LINE>" +str);
            //right.append(QString::number(rightstart++) + "<LINE>" +str);
            left++;
            //bool ignored =false;

            QString temp = str;
            ui->svnCheckForModificationsTW->setRowCount((left >= right) ? left : right);
            QTableWidgetItem *num = new QTableWidgetItem(QString::number(leftstart-1));
            num->setTextAlignment(Qt::AlignCenter);
            if(str.contains("<TITLE>"))
            {
                num->setForeground(QBrush(QColor(255,255,255)));
                num->setBackgroundColor(TITLE_EDITOR_BACK_COLOR);
                QFont font;
                font.setBold(true);
                font.setWeight(QFont::Bold);
                num->setFont(font);

            }
            leftstart++;
            ui->svnCheckForModificationsTW->setItem((left-1),0,num);
            QTableWidgetItem *item = new QTableWidgetItem();
            updateItemStyle(item,str);
            str.replace("<NEXTLINE>","\n");
            item->setText(str);

            ui->svnCheckForModificationsTW->setItem((left-1),1,item);

            right++;
            ui->svnCheckForModificationsTW->setRowCount((left >= right) ? left : right);
            QTableWidgetItem *number = new QTableWidgetItem(QString::number(rightstart-1));
            number->setTextAlignment(Qt::AlignCenter);
            if(temp.contains("<TITLE>"))
            {
                number->setForeground(QBrush(QColor(255,255,255)));
                number->setBackgroundColor(TITLE_EDITOR_BACK_COLOR);
                QFont font;
                font.setBold(true);
                font.setWeight(QFont::Bold);
                number->setFont(font);

            }
            rightstart++;
            ui->svnCheckForModificationsTW->setItem((right-1),2,number);
            QTableWidgetItem *itm = new QTableWidgetItem();
            updateItemStyle(itm,temp);
            temp.replace("<NEXTLINE>","\n");
            itm->setText(temp);
            ui->svnCheckForModificationsTW->setItem((right-1),3,itm);


        }
        else if(str.startsWith("+"))
        {
            rightcount++;
            //right.append(QString::number(rightstart++) +"<LINE> <colored>"+str.remove("+"));
            right++;
            ui->svnCheckForModificationsTW->setRowCount((left >= right) ? left : right);
            QTableWidgetItem *num = new QTableWidgetItem(QString::number(rightstart-1));
            num->setTextAlignment(Qt::AlignCenter);
            rightstart++;
            if(str.contains("<TITLE>"))
            {
                num->setForeground(QBrush(QColor(255,255,255)));
                num->setBackgroundColor(TITLE_EDITOR_BACK_COLOR);
                QFont font;
                font.setBold(true);
                font.setWeight(QFont::Bold);
                num->setFont(font);

            }
            num->setBackgroundColor(QColor(251,130,89));
            ui->svnCheckForModificationsTW->setItem((right-1),2,num);
            str.append("<COLORED>");
            QTableWidgetItem *item = new QTableWidgetItem();

            updateItemStyle(item,str.remove("+"));
            str.replace("<NEXTLINE>","\n");
            item->setText(str);
            ui->svnCheckForModificationsTW->setItem((right-1),3,item);

        }
        else if(str.startsWith("-"))
        {
            leftcount++;
            // left.append(QString::number(leftstart++) + "<LINE> <colored>"+str.remove("-"));
            left++;
            //bool ignored =false;
            ui->svnCheckForModificationsTW->setRowCount((left >= right) ? left : right);
            QTableWidgetItem *num = new QTableWidgetItem(QString::number(leftstart-1));
            num->setTextAlignment(Qt::AlignCenter);
            leftstart++;
            if(str.contains("<TITLE>"))
            {
                num->setForeground(QBrush(QColor(255,255,255)));
                num->setBackgroundColor(TITLE_EDITOR_BACK_COLOR);
                QFont font;
                font.setBold(true);
                font.setWeight(QFont::Bold);
                num->setFont(font);

            }
            num->setBackgroundColor(QColor(251,130,89));
            ui->svnCheckForModificationsTW->setItem((left-1),0,num);
            str.append("<COLORED>");
            QTableWidgetItem *itm = new QTableWidgetItem();

            updateItemStyle(itm,str.remove("-"));
            str.replace("<NEXTLINE>","\n");
            itm->setText(str);
            ui->svnCheckForModificationsTW->setItem((left-1),1,itm);
        }

    }

    while(leftcount != rightcount)
    {
        if(leftcount < rightcount)
        {
            leftcount++;
            //left.append("");
            left++;
            ui->svnCheckForModificationsTW->setRowCount((left >= right) ? left : right);
            QTableWidgetItem *item = new QTableWidgetItem("");
            ui->svnCheckForModificationsTW->setItem((left-1),1,item);

        }
        else
        {
            rightcount++;
            // right.append("");
            right++;
            ui->svnCheckForModificationsTW->setRowCount((left >= right) ? left : right);
            QTableWidgetItem *item = new QTableWidgetItem("");
            ui->svnCheckForModificationsTW->setItem((right-1),3,item);
        }
    }

}
