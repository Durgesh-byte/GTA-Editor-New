#include "GTAFailureConfig.h"


GTAFailureConfig::GTAFailureConfig()
{
_LstOfFailureProfiles.clear();
}
GTAFailureConfig::GTAFailureConfig(const GTAFailureConfig& rhs)
{
    bool rc;
    for (int i=0;i<rhs.noOfProfiles();i++)
    {
        GTAFailureProfileBase* profile = NULL;
        double duration;
        QString vmacForcingType;
        rc=rhs.getFailureProfile(i,profile,duration,vmacForcingType);
        if (rc && profile!=NULL)
        {
            appendFailure(profile->getClone(),duration,vmacForcingType);
            
        }
    }
    _ConfigName = rhs.getName();
    _vmacForcingTypes = rhs.getVmacForcingTypes();

}
GTAFailureConfig:: ~GTAFailureConfig()
{
    if(_LstOfFailureProfiles.isEmpty()==false)
    {
        do
        {

            QPair <GTAFailureProfileBase* , double> profDurationPair = _LstOfFailureProfiles.takeAt(0);
            if(profDurationPair.first !=NULL)
                delete profDurationPair.first;
        }while (_LstOfFailureProfiles.isEmpty()==false);
    }

}

QString GTAFailureConfig::getConfigurationName()
{
    return _ConfigName;

}

bool GTAFailureConfig:: getFailureProfile(const int& index , GTAFailureProfileBase*& opProfile, double& oDuration, QString& oVmacForcingType)const
{
   if (index>=_LstOfFailureProfiles.size() || index<0)
        return false;

    QPair <GTAFailureProfileBase* , double> profDurationPair = _LstOfFailureProfiles.at(index);
    opProfile = profDurationPair.first;
    oDuration = profDurationPair.second;
    if(index<_vmacForcingTypes.size()&& index>=0)
        oVmacForcingType=_vmacForcingTypes.at(index);

    return true;
}
bool GTAFailureConfig:: appendFailure(GTAFailureProfileBase* ipProfile,const double& iduration , const QString& iVmacForcingType)
{
    bool rc = false;
    _LstOfFailureProfiles.append(qMakePair(ipProfile, iduration));
    _vmacForcingTypes.append(iVmacForcingType);
    return rc;

}
int GTAFailureConfig::noOfProfiles()const
{
    return _LstOfFailureProfiles.size();
}
bool GTAFailureConfig::remove(const int& index)
{
    if (index>=_LstOfFailureProfiles.size() || index<0)
        return false;
    
    QPair <GTAFailureProfileBase* , double> profDurationPair = _LstOfFailureProfiles.takeAt(index);
    if(profDurationPair.first !=NULL)
        delete profDurationPair.first;
    if (index<_vmacForcingTypes.size() && index>0)
        _vmacForcingTypes.takeAt(index);
    
    return true;
}
bool GTAFailureConfig:: editFailureProfile(const int& index , GTAFailureProfileBase* ipProfile, const double& iduration,const QString& isVmacForcingType )
{
    if (index>=_LstOfFailureProfiles.size() || index<0)
        return false;

    QPair <GTAFailureProfileBase* , double> profDurationPair = _LstOfFailureProfiles.takeAt(index);
    if(profDurationPair.first !=NULL)
        delete profDurationPair.first;

    _LstOfFailureProfiles.insert(index,qMakePair(ipProfile, iduration));

    if (index<_vmacForcingTypes.size() && index>=0)
    {
        _vmacForcingTypes.takeAt(index);
        _vmacForcingTypes.insert(index,isVmacForcingType);
    }


    return true;
}

bool GTAFailureConfig:: getProfileXNodes(QDomDocument& iDomDoc, QDomElement& oElement)
{

    bool rc = false;
    if (iDomDoc.isNull() )
    {
        rc=true;
        QDomElement configNode = iDomDoc.createElement(XNODE_CONFIG);
        configNode.toElement().setAttribute(XATTR_NAME,_ConfigName);

        for (int i=0;i<_LstOfFailureProfiles.size();i++)
        {
            QPair <GTAFailureProfileBase* , double> currentPair = _LstOfFailureProfiles.at(i);


            QDomElement failureProfileElem = iDomDoc.createElement(XNODE_FAILURE_PROFILE);
            failureProfileElem.setAttribute(XATTR_DURATION,currentPair.second);



        }
        
        
        oElement = configNode;

    }
    return rc;
}