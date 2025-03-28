#include "GTAActionAVCWidget.h"
#include "ui_GTAActionAVCWidget.h"
#include "GTAActionManual.h"
#include "GTAParamValidator.h"
#include "GTAParamValidator.h"

#pragma warning(push, 0)
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QDomDocument>
#pragma warning(pop)

GTAActionAVCWidget::GTAActionAVCWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionAVCWidget)
{
    ui->setupUi(this);

    connect(ui->browseXMLPB,SIGNAL(clicked()),this,SLOT(onBrowseXML()));
    //ui->dynamicWidgetFrame

    
   
}

GTAActionAVCWidget::~GTAActionAVCWidget()
{
    delete ui;
}

void GTAActionAVCWidget::setActionCommand(const GTAActionBase * )
{
  

}

bool GTAActionAVCWidget::getActionCommand(GTAActionBase*& opCmd)const
{
  opCmd = NULL;
  return false;
}

void GTAActionAVCWidget::onBrowseXML()
{
    QString sFilePath;
    QString defPath = ui->browseXMLLE->text();
    QFileInfo info(defPath);
    defPath = info.absolutePath();
    QFileInfo info1(defPath);
    if(defPath.isEmpty() || !info1.exists())
        defPath = ".";
    sFilePath = QFileDialog::getOpenFileName(this, ("AVC File Browser"),
        defPath,
        tr("Images (*.xml)"));

    if (!sFilePath.isEmpty())
    {
        ui->browseXMLLE->setText(QDir::cleanPath(sFilePath));
        loadDynamicWidget(sFilePath);
        
    }

   
}
bool GTAActionAVCWidget::loadDynamicWidget(const QString& iFilePath)
{
    bool rc=false;
    QFileInfo info(iFilePath);
    if (info.exists())
    {
        QDomDocument avcXmlDoc;
        rc = avcXmlDoc.setContent(iFilePath);
        if (rc)
        {
            QDomElement rootNode = avcXmlDoc.documentElement();
            if (rootNode.isNull())
               rc = false;
            else
            {
                rc = false;
                QDomNodeList childNodeList = rootNode.childNodes();
                QDomNode allAttributeNode;
                for (int i=0;i<childNodeList.size();i++)
                {
                    QDomElement elem = childNodeList.at(i).toElement();
                    if (!elem.isNull())
                    {
                        if (elem.nodeName()=="Attributes")
                        {
                            rc = true;
                            allAttributeNode = elem;
                            break;
                        }
                    }
                }


                if(!allAttributeNode.isNull())
                {

                }
            }

        }



    }



    return rc;


}
void GTAActionAVCWidget::clear()
{

}
bool GTAActionAVCWidget::isValid()const
{
    return true;

}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_AVC,GTAActionAVCWidget,obj)
