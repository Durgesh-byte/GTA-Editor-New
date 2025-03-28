#include "GTAOneClickSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAOneClick.h"
#include "GTARequirementSerializer.h"
GTAOneClickSerializer::GTAOneClickSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
GTAOneClickSerializer::~GTAOneClickSerializer()
{
    _pActionCmd = NULL;
}
void GTAOneClickSerializer::setCommand(const GTACommandBase* iPCmd)
{
    _pActionCmd = dynamic_cast<GTAOneClick*>((GTACommandBase*) iPCmd);
}
bool GTAOneClickSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {   
        QString AppName = _pActionCmd->getApplicationName();
        QString ConfFile = _pActionCmd->getConfFile();
        QString ConfigName = _pActionCmd->getConfigName();
        QString TestRig = _pActionCmd->getTestRigName();
        QString TestType = _pActionCmd->getTestRigType();
        QStringList Options = _pActionCmd->getOptionNames();
        QString description = _pActionCmd->getDescription();
        QString sActiononFail = _pActionCmd->getActionOnFail();
        QString startEnv = _pActionCmd->getIsStartEnv() == true?XNODE_TRUE:XNODE_FALSE;
        QString stopAlto = _pActionCmd->getIsStopAlto() == true?XNODE_TRUE:XNODE_FALSE;
        QString stopTestConfig = _pActionCmd->getIsStopTestConfig() == true?XNODE_TRUE:XNODE_FALSE;
        QStringList sDumpList;
        if (sActiononFail==ACT_FAIL_STOP)
            sDumpList = _pActionCmd->getDumpList();
        
        oElement = iDomDoc.createElement(XNODE_ACTION);
        
     
        setActionAttributes(_pActionCmd,oElement);




        QDomElement domConfigName = iDomDoc.createElement(XNODE_ONE_CLICK);
        domConfigName.setAttribute(XNODE_NAME,ConfigName);
        domConfigName.setAttribute(XNODE_FILE,ConfFile);
        domConfigName.setAttribute(XNODE_TESTRIG,TestRig);
        domConfigName.setAttribute(XNODE_TESTRIG_TYPE,TestType);
        domConfigName.setAttribute(XNODE_ONE_CLICK_APP,AppName);
        domConfigName.setAttribute(XATTR_ONELCICK_SETENV_START_ENV,startEnv);
        domConfigName.setAttribute(XATTR_ONELCICK_SETENV_STOP_ALTO,stopAlto);
        domConfigName.setAttribute(XATTR_ONELCICK_SETENV_STOP_TESTCONFIG,stopTestConfig);
        QDomElement domDes = iDomDoc.createElement(XNODE_ONE_CLICK_DESCRIPTION);
        domDes.setAttribute(XNODE_ONE_CLICK_DESCRIPTION_VALUE,description);
        domConfigName.appendChild(domDes);
        QDomElement domOptions = iDomDoc.createElement(XNODE_ONE_CLICK_OPTIONS);
        for (auto Option : Options)
        {
            QString OptionName = Option;
            QString OptionType = _pActionCmd->getTypeofOption(Option);
            QStringList OptionValue = _pActionCmd->getValueForOption(Option);

            QString sIniFile = _pActionCmd->getIniFileNameofOption(Option);
            QHash<QString,bool> _hshFolderChkStatus= _pActionCmd->getDirChkStatusofOption(Option);

            if (!sIniFile.isEmpty()&& !_hshFolderChkStatus.isEmpty())
            {
                OptionValue = QStringList()<<sIniFile;
            }



            QDomElement domOption = iDomDoc.createElement(XNODE_ONE_CLICK_OPTION);
            domOption.setAttribute(XNODE_ONE_CLICK_OPTION_NAME,OptionName);
            domOption.setAttribute(XNODE_ONE_CLICK_OPTION_TYPE,OptionType);
            if(!OptionValue.isEmpty())
            {
                domOption.setAttribute(XNODE_ONE_CLICK_OPTION_VALUE,OptionValue.first());
            }
            domOptions.appendChild(domOption);

            for (auto sFolderName : _hshFolderChkStatus.keys())
            {
              QDomElement  folderSelcetionNode = iDomDoc.createElement(XNODE_ONE_CLICK_SEL_FOLDER);
              folderSelcetionNode.setAttribute(XATTR_NAME,sFolderName);
              folderSelcetionNode.setAttribute(XATTR_CHK_STATUS, _hshFolderChkStatus[sFolderName]?"true":"false");
              domOption.appendChild(folderSelcetionNode);

                
            }
        }
        domConfigName.appendChild(domOptions);
        oElement.appendChild(domConfigName);

        if (!sDumpList.isEmpty())
        {
            QDomElement doDumpList = iDomDoc.createElement(XNODE_DUMPLIST);
            oElement.appendChild(doDumpList);
            foreach(QString dumpParameter, sDumpList)
            {
                QDomElement doDumpParamNode = iDomDoc.createElement(XNODE_DUMP_PARAM);
                QDomText domParamValTxt = iDomDoc.createTextNode(dumpParameter);
                doDumpList.appendChild(doDumpParamNode);
                doDumpParamNode.appendChild(domParamValTxt);

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

bool GTAOneClickSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)
{
    bool rc = false;
    if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION)
    {   
        QString actionName = iElement.attribute(XNODE_NAME);
        QString sActionOnFail = iElement.attribute(XNODE_ONFAIL);
        QString complementName = iElement.attribute(XNODE_COMPLEMENT);
        
        if(actionName != ACT_ONECLICK || complementName!=COM_ONECLICK_ENV)
            return rc;
        else
            rc = true;

       
        
        GTAOneClick * pOneClickCmd = new GTAOneClick(actionName,complementName);
        getActionAttributes(iElement,pOneClickCmd);
        QDomNodeList childNodeList =iElement.childNodes();
        
        QString ConfFile;
        QString ConfigName ;
        QString TestRigName;
        QString TestRigType;
        QString AppName; 
        QStringList dumpList;
        bool startEnv = false;
        bool stopTetsconfig = false;
        bool stopalto = false;
        
        //TODO: What if node is missing? Needs fail mechanism
        for(int i = 0; i <childNodeList.count(); i++)
        {
            QDomNode childNode = childNodeList.at(i);
            if(childNode.isElement() && childNode.nodeName() == XNODE_ONE_CLICK)
            {
                QDomElement domElem = childNode.toElement();
                ConfigName = domElem.attribute(XNODE_NAME);
                ConfFile = domElem.attribute(XNODE_FILE);
                TestRigName = domElem.attribute(XNODE_TESTRIG);
                TestRigType = domElem.attribute(XNODE_TESTRIG_TYPE);
                AppName = domElem.attribute(XNODE_ONE_CLICK_APP);
                QString sStartEnv = domElem.attribute(XATTR_ONELCICK_SETENV_START_ENV);
                QString sStopAlto = domElem.attribute(XATTR_ONELCICK_SETENV_STOP_ALTO);
                QString sStopTestconfig = domElem.attribute(XATTR_ONELCICK_SETENV_STOP_TESTCONFIG);
                startEnv = (sStartEnv == XNODE_TRUE)?true:false;
                stopTetsconfig= (sStopTestconfig == XNODE_TRUE)?true:false;
                stopalto= (sStopAlto == XNODE_TRUE)?true:false;

                QDomElement  desElem = domElem.namedItem(XNODE_ONE_CLICK_DESCRIPTION).toElement();
                QString desVal = desElem.attribute(XNODE_ONE_CLICK_DESCRIPTION_VALUE);
                pOneClickCmd->setDescription(desVal);
                QDomNode options = domElem.namedItem(XNODE_ONE_CLICK_OPTIONS);
                QDomNodeList optList = options.childNodes();
                for(int j=0;j<optList.count();j++)
                {
                    QDomNode option = optList.at(j);
                    if(option.isElement() && option.nodeName() == XNODE_ONE_CLICK_OPTION)
                    {
                        QDomElement domElemOpt = option.toElement();
                        QString optName = domElemOpt.attribute(XNODE_ONE_CLICK_OPTION_NAME);
                        QString optType = domElemOpt.attribute(XNODE_ONE_CLICK_OPTION_TYPE);
                        QString optValue = domElemOpt.attribute(XNODE_ONE_CLICK_OPTION_VALUE);
                        QHash<QString,bool> hshFolderSelection;
                        
                        if(!optName.isEmpty())
                        {
                            QDomNodeList folderNodeLst = domElemOpt.childNodes();
                            if(!folderNodeLst.isEmpty() && folderNodeLst.at(0).toElement().nodeName() == XNODE_ONE_CLICK_SEL_FOLDER)
                            {
                               
                                pOneClickCmd->setIniFileNameofOption(optName,optValue);
                                for(int k=0; k<folderNodeLst.size(); k++)
                                {
                                    QDomNode folderNode = folderNodeLst.at(k);
                                    QString sFolderName = folderNode.toElement().attribute(XATTR_NAME);
                                    bool chkStatus=folderNode.toElement().attribute(XATTR_CHK_STATUS)=="true"?true:false;

                                    hshFolderSelection.insert(sFolderName,chkStatus);
                                }
                                if(!hshFolderSelection.isEmpty())
                                    pOneClickCmd->setDirChkStatusofOption(optName,hshFolderSelection);
                            }
                            else
                                pOneClickCmd->setOptionValueAndType(optName,QStringList(optValue),optType);
                        }
                    }
                }
                

                
            }
            else if (childNode.isElement() && childNode.nodeName() == XNODE_DUMPLIST)
            {
                QDomNodeList dumpParamNodes = childNode.toElement().elementsByTagName(XNODE_DUMP_PARAM);
                int dumpNodeSize = dumpParamNodes.count();

                for (int j =0 ;j<dumpNodeSize;j++)
                {
                    QDomNode dumpNode = dumpParamNodes.at(j);
                    QString sDumpParameter = dumpNode.toElement().text();
                    if (!sDumpParameter.isEmpty())
                    {
                        dumpList.append(sDumpParameter);
                    }
                }
            }
        }
        pOneClickCmd->setIsStartEnv(startEnv);
        pOneClickCmd->setIsStopAlto(stopalto);
        pOneClickCmd->setIsStopTestConfig(stopTetsconfig);
        pOneClickCmd->setConfFile(ConfFile);
        pOneClickCmd->setConfigName(ConfigName);
        pOneClickCmd->setApplicationName(AppName);
        pOneClickCmd->setTestRigName(TestRigName);
        pOneClickCmd->setTestRigType(TestRigType);
        /*pOneClickCmd->setActionOnFail(sActionOnFail);*/
        if (!dumpList.isEmpty())
            pOneClickCmd->setDumpList(dumpList);
        
        opCommand = pOneClickCmd;

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
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_ONECLICK,COM_ONECLICK_ENV),GTAOneClickSerializer)
