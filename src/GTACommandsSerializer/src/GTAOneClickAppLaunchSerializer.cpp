#include "GTAOneClickAppLaunchSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAOneClickLaunchApplication.h"
#include "GTARequirementSerializer.h"
GTAOneClickAppLaunchSerializer::GTAOneClickAppLaunchSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
GTAOneClickAppLaunchSerializer::~GTAOneClickAppLaunchSerializer()
{
    _pActionCmd = NULL;
}
void GTAOneClickAppLaunchSerializer::setCommand(const GTACommandBase* iPCmd)
{
    _pActionCmd = dynamic_cast<GTAOneClickLaunchApplication*>((GTACommandBase*) iPCmd);
}
bool GTAOneClickAppLaunchSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {

		oElement = iDomDoc.createElement(XNODE_ACTION);


          setActionAttributes(_pActionCmd,oElement);


        
        QList<GTAOneClickLaunchApplicationItemStruct *> lAppStructs = _pActionCmd->getLaStruct();
        int totalStruc = lAppStructs.size();
        for (int i=0;i<totalStruc;i++)
        {

            GTAOneClickLaunchApplicationItemStruct * pStruct = lAppStructs.at(i);
            if (pStruct!=NULL)
            {

                QString TestRig=pStruct->getTestRig();
                QString ToolId=pStruct->getToolId() ;
                QString ToolType=pStruct->getToolType();
                QString AppName=pStruct->getAppName() ;
                QString ConfFile=pStruct->getConfigFile();
                bool isKillMode = pStruct->getKillMode();

                QString killModeVal = (isKillMode== true)?XNODE_TRUE:XNODE_FALSE;
                //QString AppItem=pStruct->getAppItem();
                QStringList Options=pStruct->getOptions();

                QDomElement optionsNodeDom = iDomDoc.createElement(XNODE_ONE_CLICK_LAUNCHAPP_OPTS);
                optionsNodeDom.setAttribute(XATTR_ONE_CLICK_LAUNCHAPP_TESTRIG,TestRig);
                optionsNodeDom.setAttribute(XATTR_ONE_CLICK_LAUNCHAPP_TOOLID,ToolId);
                optionsNodeDom.setAttribute(XATTR_ONE_CLICK_LAUNCHAPP_TOOLTYPE,ToolType);
                optionsNodeDom.setAttribute(XATTR_ONE_CLICK_LAUNCHAPP_APPNAME,AppName);
                optionsNodeDom.setAttribute(XATTR_ONE_CLICK_LAUNCHAPP_CONFFILE,ConfFile);
                optionsNodeDom.setAttribute(XATTR_ONE_CLICK_LAUNCHAPP_KILL_MODE,killModeVal);
                oElement.appendChild(optionsNodeDom);
                
                foreach(QString sOptionName, Options)
                {
                   QDomElement domOptionNode = iDomDoc.createElement(XNODE_ONE_CLICK_LAUNCHAPP_OPT);
                   domOptionNode.setAttribute(XATTR_ONE_CLICK_LAUNCHAPP_OPTIONNAME, sOptionName);
                   QString sOptionVal = pStruct->getValue(sOptionName);
                   domOptionNode.setAttribute(XATTR_ONE_CLICK_LAUNCHAPP_OPTIONVAL, sOptionVal);
                   

                   QDomElement domArgumentNode = iDomDoc.createElement(XNODE_ONE_CLICK_LAUNCHAPP_ARG);
                   QString sArguments = pStruct->getArgs(sOptionName);
                   QDomText argTxtNode = iDomDoc.createTextNode(sArguments);
                   domArgumentNode.appendChild(argTxtNode);
                   
                   domOptionNode.appendChild(domArgumentNode);
                   optionsNodeDom.appendChild(domOptionNode);

                   
                  


                }
            }
        }
        
     
        //comment node
        GTACommentSerializer commentSerializer(_pActionCmd);
        QDomElement domComment;
        rc = commentSerializer.serialize(iDomDoc,domComment);
        if(rc && (!domComment.isNull()))
        {
            oElement.appendChild(domComment);
        }

        GTARequirementSerializer requirementSerializer(_pActionCmd);
        QDomElement domRequirement;
        rc = requirementSerializer.serialize(iDomDoc,domRequirement);
        if( rc && (!domRequirement.isNull()))
        {
                oElement.appendChild(domRequirement);
        }

        rc = true;
    }
    return rc;
}

bool GTAOneClickAppLaunchSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)
{
    bool rc = false;
    if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION)
    {   
        
        

        GTAOneClickLaunchApplication* pOneClickApp = new GTAOneClickLaunchApplication;
        getActionAttributes(iElement,pOneClickApp);
        QDomNodeList childNodeList = iElement.childNodes();
        for(int i = 0; i <childNodeList.count(); i++)
        {
            QDomNode childNode = childNodeList.at(i);
            if(childNode.isElement() && childNode.nodeName() == XNODE_ONE_CLICK_LAUNCHAPP_OPTS)
            {
                GTAOneClickLaunchApplicationItemStruct* pStruct = new GTAOneClickLaunchApplicationItemStruct;


                QDomElement domOptionsNode = childNode.toElement();
                QString sTestRig    = domOptionsNode.attribute(XATTR_ONE_CLICK_LAUNCHAPP_TESTRIG);
                QString sConfigFile = domOptionsNode.attribute(XATTR_ONE_CLICK_LAUNCHAPP_CONFFILE);
                QString sToolID     = domOptionsNode.attribute(XATTR_ONE_CLICK_LAUNCHAPP_TOOLID);
                QString sToolType   = domOptionsNode.attribute(XATTR_ONE_CLICK_LAUNCHAPP_TOOLTYPE);
                QString sAppName    = domOptionsNode.attribute(XATTR_ONE_CLICK_LAUNCHAPP_APPNAME);
                QString killModeVal = domOptionsNode.attribute(XATTR_ONE_CLICK_LAUNCHAPP_KILL_MODE);
                bool isKillMode  = (killModeVal==XNODE_TRUE)?true:false;
                pStruct->setTestRig(sTestRig);
                pStruct->setConfigFile(sConfigFile);
                pStruct->setToolId(sToolID);
                pStruct->setToolType(sToolType);
                pStruct->setAppName(sAppName);
                pStruct->setKillMode(isKillMode);
                QDomNodeList optionNodeDomLst = domOptionsNode.childNodes();
                // int optionNodeListSize = optionNodeDomLst.size();

                if(!optionNodeDomLst.isEmpty())
                {
                    for(int j =0;j<optionNodeDomLst.size();j++)
                    {
                        QDomElement optionElemNode = optionNodeDomLst.at(j).toElement();
                        QString optionName = optionElemNode.attribute(XATTR_ONE_CLICK_LAUNCHAPP_OPTIONNAME);
                        QString optionVal  = optionElemNode.attribute(XATTR_ONE_CLICK_LAUNCHAPP_OPTIONVAL);
                        QString sArgument;
                        QDomNodeList argumentNodes = optionElemNode.elementsByTagName(XNODE_ONE_CLICK_LAUNCHAPP_ARG);
                        if(!argumentNodes.isEmpty())
                        {
                            sArgument = argumentNodes.at(0).toElement().text();

                        }

                        pStruct->insertOption(optionName,optionVal);
                        pStruct->insertValArgs(optionName,sArgument);
                        //pStruct->insertOptionType();
                    }
                }

                pOneClickApp->insertLAStruct(pStruct);


                
            }

        }
       
        opCommand = pOneClickApp;

        //comment node
        GTACommentSerializer commentSerializer(NULL);
        commentSerializer.deserialize(iElement,opCommand);


        //requiremrnt node
        GTARequirementSerializer requirementSerializer(NULL);
        requirementSerializer.deserialize(iElement,opCommand);
        rc = true;
    }
    return rc;
}
#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_ONECLICK,COM_ONECLICK_APP),GTAOneClickAppLaunchSerializer)

