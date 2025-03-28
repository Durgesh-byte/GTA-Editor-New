#include "GTAFailureConfigSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAFailureProfileSerializer.h"
GTAFailureConfigSerializer::GTAFailureConfigSerializer(GTAFailureConfig *pActionCmd) 
{
    _pConfig = pActionCmd;
}

bool GTAFailureConfigSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pConfig != NULL && iDomDoc.isNull()==false )
    {


        rc=true;
        
        int noOfProfiles = _pConfig->noOfProfiles();
        QDomElement configNode = iDomDoc.createElement(XNODE_CONFIG);
        configNode.toElement().setAttribute(XATTR_NAME,_pConfig->getConfigurationName());
        //configNode.toElement().setAttribute(XATTR_VMACFORCETYPE,_pConfig->getVmacForcingType());

        for (int i=0;i<noOfProfiles;i++)
        {
            GTAFailureProfileBase* profileBase = NULL;
            double dDuration=0;
            QString sVamcForcingType;
           _pConfig->getFailureProfile(i,profileBase,dDuration,sVamcForcingType);

           QDomElement profileElem ;
           GTAFailureProfileSerializer serializer(profileBase);
           serializer.serialize(iDomDoc,profileElem);
           if (profileElem.isNull()==false)
           {
               profileElem.setAttribute(XATTR_DURATION,dDuration); 
               profileElem.setAttribute(XATTR_VMACFORCETYPE,sVamcForcingType);
               configNode.appendChild(profileElem);
           }
          
        }
  

    oElement=configNode;

       
    }
    return rc;
}

bool GTAFailureConfigSerializer::deserialize(const QDomElement &iElement, GTAFailureConfig *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        opCommand= new GTAFailureConfig;
        opCommand->setName(iElement.attribute(XATTR_NAME));
       

        QDomNodeList configChildNodes = iElement.childNodes();
        for (int i=0;i<configChildNodes.size();i++)
        {
            QDomNode chidNode = configChildNodes.at(i);
            if (chidNode.isNull()==false)
            {
                QDomElement childElement = chidNode.toElement();
                if (chidNode.nodeName()==XNODE_FAILURE_PROFILE)
                {
                    QString sDuration  = childElement.attribute(XATTR_DURATION);
                    QString sVmacForcingType = childElement.attribute(XATTR_VMACFORCETYPE);
                    GTAFailureProfileBase* pBase = NULL;
                    GTAFailureProfileSerializer failProfileDS;
                    failProfileDS.deserialize(childElement,pBase);
                    if (pBase!=NULL)
                    {
                        opCommand->appendFailure(pBase,sDuration.toDouble(),sVmacForcingType);
                    }
                }
            }
            
        }

        rc = true;
    }
    return rc;

}
