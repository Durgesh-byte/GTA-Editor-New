#ifndef GTAFAILURECONFIG_H
#define GTAFAILURECONFIG_H
#include "GTAUtil.h"
#include "GTAFailureProfileBase.h"

#pragma warning(push, 0)
#include <QString>
#include <QPair>
#include <QList>
#include <QDomDocument>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailureConfig 
{
public:
    GTAFailureConfig(); 
    GTAFailureConfig(const GTAFailureConfig& rhs);
    virtual ~GTAFailureConfig();

    QString getConfigurationName();
    
    bool getFailureProfile(const int& index , GTAFailureProfileBase*& , double& oDuration , QString& oVmacForcingType)const;
    bool appendFailure(GTAFailureProfileBase* ipProfile,const double& iduration , const QString& iVmacForcingType);
    int noOfProfiles()const;
    bool remove(const int& index);
    bool editFailureProfile(const int& index , GTAFailureProfileBase* ipProfile, const double& iduration , const QString& sVmacForcingType);
    QString getName()const{return _ConfigName;}
    void setName(const QString& iName ){_ConfigName=iName;}

    bool getProfileXNodes(QDomDocument& iDomDoc, QDomElement& oElement);

    //void setVmacForcingType(const QString& iVmacForcing){_vmacForcingType=iVmacForcing;}
    //QString getVmacForcingType()const{return _vmacForcingType;}
    QStringList getVmacForcingTypes()const{return _vmacForcingTypes;}
    void        setVmacForcingTypes(const QStringList& iVmacForcing){_vmacForcingTypes=iVmacForcing;}




private:

    QString _ConfigName;
    QList<  QPair <GTAFailureProfileBase* , double>>  _LstOfFailureProfiles;
    //QString _vmacForcingType;
    QStringList _vmacForcingTypes;
    
};

#endif // GTAFailProfileIdle_H
