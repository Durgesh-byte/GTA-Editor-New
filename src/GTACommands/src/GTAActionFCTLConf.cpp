#include "GTAActionFCTLConf.h"
#include "GTAUtil.h"

GTAActionFCTLConf::GTAActionFCTLConf()
{

}
GTAActionFCTLConf::GTAActionFCTLConf(const GTAActionFCTLConf& rhs)
{
    setConfigName(rhs.getConfigName());
    setLAW(rhs.getLAW());
    setSFConf(rhs.getSFConf() );
    setGrndFlight(rhs.getGrndFlight());
    setEngines(rhs.getEngines());
    setLandingGear(rhs.getLandingGear() );
    setCG(rhs.getCG());
    setGrossWt(rhs.getGrossWt());
    setVCAS(rhs.getVCAS());
    setAltitude(rhs.getAltitude());
    setYHYD(rhs.getYHYD());
    setGHYD(rhs.getGHYD());
    setBHYD(rhs.getBHYD());
    setAlpha(rhs.getAlpha());
    setFCPC1(rhs.getFCPC1());
    setFCPC2(rhs.getFCPC2());
    setFCPC3(rhs.getFCPC3());
    setFCSC1(rhs.getFCSC1());
    setFCSC2(rhs.getFCSC2());
    setBCM(rhs.getBCM());
    setFMGEC1(rhs.getFMGEC1());
    setFMGEC2(rhs.getFMGEC2());
    setAP1(rhs.getAP1());
    setAP2(rhs.getAP2());

    setAction(rhs.getAction());
    setComplement(rhs.getComplement());
    setComment(rhs.getComment());
}


void GTAActionFCTLConf::setConfigName(const QString &iVal)
{
    _ConfigName = iVal;
}

QString GTAActionFCTLConf::getConfigName() const
{
    return _ConfigName;
}

void GTAActionFCTLConf::setLAW(const QString &iVal)
{
    _Law = iVal;
}

QString GTAActionFCTLConf::getLAW() const
{
    return _Law;
}

void GTAActionFCTLConf::setSFConf(const QString &iVal)
{
    _SFConf  = iVal;
}

QString GTAActionFCTLConf::getSFConf() const
{
    return _SFConf;
}

void GTAActionFCTLConf::setGrndFlight(const QString &iVal)
{
    _GrndFlight = iVal;
}

QString GTAActionFCTLConf::getGrndFlight() const
{
    return _GrndFlight;
}

void GTAActionFCTLConf::setEngines(const QString &iVal)
{
    _Engines = iVal;
}

QString GTAActionFCTLConf::getEngines() const
{
    return _Engines;
}

void GTAActionFCTLConf::setLandingGear(const QString &iLDG)
{
    _LandingGear = iLDG;
}

QString GTAActionFCTLConf::getLandingGear() const
{
    return _LandingGear;
}

void GTAActionFCTLConf::setCG(const QString &iCG)
{
    _CG = iCG;
}

QString GTAActionFCTLConf::getCG() const
{
    return _CG;
}

void GTAActionFCTLConf::setGrossWt(const QString &iVal)
{
    _GrossWt = iVal;
}

QString GTAActionFCTLConf::getGrossWt() const
{
    return _GrossWt;
}

void GTAActionFCTLConf::setVCAS(const QString &iVal)
{
    _VCAS = iVal;
}

QString GTAActionFCTLConf::getVCAS() const
{
    return _VCAS;
}

void GTAActionFCTLConf::setAltitude(const QString &iVal)
{
    _Altitude = iVal;
}

QString GTAActionFCTLConf::getAltitude() const
{
    return _Altitude;
}

void GTAActionFCTLConf::setYHYD(const QString &iVal)
{
    _YHYD  = iVal;
}

QString GTAActionFCTLConf::getYHYD() const
{
    return _YHYD;
}

void GTAActionFCTLConf::setGHYD(const QString &iVal)
{
    _GHYD = iVal;
}

QString GTAActionFCTLConf::getGHYD() const
{
    return _GHYD;
}

void GTAActionFCTLConf::setBHYD(const QString &iVal)
{
    _BHYD= iVal;
}

QString GTAActionFCTLConf::getBHYD() const
{
    return _BHYD;
}

void GTAActionFCTLConf::setAlpha(const QString &iVal)
{
    _Alpha = iVal;
}

QString GTAActionFCTLConf::getAlpha() const
{
    return _Alpha;
}

void GTAActionFCTLConf::setFCPC1(const QString &iVal)
{
    _FCPC1 = iVal;
}

QString GTAActionFCTLConf::getFCPC1() const
{
    return _FCPC1;
}

void GTAActionFCTLConf::setFCPC2(const QString &iVal)
{
    _FCPC2 = iVal;
}

QString GTAActionFCTLConf::getFCPC2() const
{
    return _FCPC2;
}

void GTAActionFCTLConf::setFCPC3(const QString &iVal)
{
    _FCPC3 = iVal;
}

QString GTAActionFCTLConf::getFCPC3() const
{
    return _FCPC3;
}

void GTAActionFCTLConf::setFCSC1(const QString &iVal)
{
    _FCSC1 = iVal;
}

QString GTAActionFCTLConf::getFCSC1() const
{
    return _FCSC1;
}

void GTAActionFCTLConf::setFCSC2(const QString &iVal)
{
    _FCSC2 = iVal;
}

QString GTAActionFCTLConf::getFCSC2() const
{
    return _FCSC2;
}

void GTAActionFCTLConf::setBCM(const QString &iVal)
{
    _BCM = iVal;
}

QString GTAActionFCTLConf::getBCM() const
{
    return _BCM;
}

void GTAActionFCTLConf::setFMGEC1(const QString &iVal)
{
    _FMGEC1 = iVal;
}

QString GTAActionFCTLConf::getFMGEC1() const
{
    return _FMGEC1;
}

void GTAActionFCTLConf::setFMGEC2(const QString &iVal)
{
    _FMGEC2 = iVal;
}

QString GTAActionFCTLConf::getFMGEC2() const
{
    return _FMGEC2;
}

void GTAActionFCTLConf::setAP1(const QString &iVal)
{
    _AP1 = iVal;
}

QString GTAActionFCTLConf::getAP1() const
{
    return _AP1;
}

void GTAActionFCTLConf::setAP2(const QString &iVal)
{
    _AP2 = iVal;
}

QString GTAActionFCTLConf::getAP2() const
{
    return _AP2;
}



QString GTAActionFCTLConf::getStatement() const
{
    return  QString("FCTL CONF");
    
}

QList<GTACommandBase*>& GTAActionFCTLConf::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionFCTLConf::insertChildren(GTACommandBase* pBase,const int& idPos)
{
     pBase;idPos;//for supressing warning.
    return false;
}

void  GTAActionFCTLConf::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionFCTLConf::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTAActionFCTLConf::getClone() const
{
    return  new GTAActionFCTLConf(*this);
}
bool GTAActionFCTLConf::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionFCTLConf::getVariableList() const
{
    return QStringList ();
}

void GTAActionFCTLConf:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        _ConfigName.replace(tag,iterTag.value());
        _Law.replace(tag,iterTag.value());
        _SFConf.replace(tag,iterTag.value());
        _GrndFlight.replace(tag,iterTag.value());
        _Engines.replace(tag,iterTag.value());
        _LandingGear.replace(tag,iterTag.value());
        _CG.replace(tag,iterTag.value());
        _GrossWt.replace(tag,iterTag.value());
        _VCAS.replace(tag,iterTag.value());
        _Altitude.replace(tag,iterTag.value());
        _YHYD.replace(tag,iterTag.value());
        _GHYD.replace(tag,iterTag.value());
        _BHYD.replace(tag,iterTag.value());
        _Alpha.replace(tag,iterTag.value());
        _FCPC1.replace(tag,iterTag.value());
        _FCPC2.replace(tag,iterTag.value());
        _FCPC3.replace(tag,iterTag.value());
        _FCSC1.replace(tag,iterTag.value());
        _FCSC2.replace(tag,iterTag.value());
        _BCM.replace(tag,iterTag.value());
        _FMGEC1.replace(tag,iterTag.value());
        _FMGEC2.replace(tag,iterTag.value());
        _AP1.replace(tag,iterTag.value());
        _AP2.replace(tag,iterTag.value());
    }
}

bool GTAActionFCTLConf::searchString(const QRegExp&, bool) const
{
    bool rc = false;
    return rc;
}
QString GTAActionFCTLConf::getSCXMLStateName()const
{
    return getStatement();

}
void GTAActionFCTLConf::stringReplace(const QRegExp&, const QString&)
{

}
