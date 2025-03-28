#ifndef GTAACTIONFCTLCONF_H
#define GTAACTIONFCTLCONF_H

#include "GTAActionBase.h"

#pragma warning(push, 0)
#include <QMap>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAActionFCTLConf:public GTAActionBase
{
public:
    GTAActionFCTLConf();
    GTAActionFCTLConf(const GTAActionFCTLConf& rhs);


    void setConfigName(const QString &iVal);
    QString getConfigName() const;

    void setLAW(const QString &iVal);
    QString getLAW() const;

    void setSFConf(const QString &iVal);
    QString getSFConf() const;

    void setGrndFlight(const QString &iVal);
    QString getGrndFlight() const;

    void setEngines(const QString &iVal);
    QString getEngines() const;

    void setLandingGear(const QString &iLDG);
    QString getLandingGear() const;

    void setCG(const QString &iCG);
    QString getCG() const;

    void setGrossWt(const QString &iVal);
    QString getGrossWt() const;

    void setVCAS(const QString &iVal);
    QString getVCAS() const;

    void setAltitude(const QString &iVal);
    QString getAltitude() const;

    void setYHYD(const QString &iVal);
    QString getYHYD() const;

    void setGHYD(const QString &iVal);
    QString getGHYD() const;

    void setBHYD(const QString &iVal);
    QString getBHYD() const;

    void setAlpha(const QString &iVal);
    QString getAlpha() const;

    void setFCPC1(const QString &iVal);
    QString getFCPC1() const;

    void setFCPC2(const QString &iVal);
    QString getFCPC2() const;

    void setFCPC3(const QString &iVal);
    QString getFCPC3() const;

    void setFCSC1(const QString &iVal);
    QString getFCSC1() const;

    void setFCSC2(const QString &iVal);
    QString getFCSC2() const;

    void setBCM(const QString &iVal);
    QString getBCM() const;

    void setFMGEC1(const QString &iVal);
    QString getFMGEC1() const;

    void setFMGEC2(const QString &iVal);
    QString getFMGEC2() const;

    void setAP1(const QString &iVal);
    QString getAP1() const;

    void setAP2(const QString &iVal);
    QString getAP2() const;



    QList<GTACommandBase*>& getChildren(void) const;
    bool insertChildren(GTACommandBase* pBase,const int& idPos);
    void setParent(GTACommandBase* iParent);
    GTACommandBase* getParent(void) const;

    GTACommandBase*getClone() const;
    bool canHaveChildren() const;
    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    QString getSCXMLStateName()const;

    QString getStatement() const;
    bool hasPrecision() const{return false ;}
    bool hasTimeOut() const{return false;}
     bool hasChannelInControl()const {return false;}

private:
    QString _ConfigName;
    QString _Law;
    QString _SFConf;
    QString _GrndFlight;
    QString _Engines;
    QString _LandingGear;
    QString _CG;
    QString _GrossWt;
    QString _VCAS;
    QString _Altitude;
    QString _YHYD;
    QString _GHYD;
    QString _BHYD;
    QString _Alpha;
    QString _FCPC1;
    QString _FCPC2;
    QString _FCPC3;
    QString _FCSC1;
    QString _FCSC2;
    QString _BCM;
    QString _FMGEC1;
    QString _FMGEC2;
    QString _AP1;
    QString _AP2;
};

#endif // GTAACTIONFCTLCONF_H
