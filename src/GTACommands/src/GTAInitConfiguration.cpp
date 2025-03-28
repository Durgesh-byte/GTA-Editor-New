#include "GTAInitConfiguration.h"
#include "GTAUtil.h"
GTAInitConfiguration::GTAInitConfiguration(const QString iConfigName)
{
    _AltSpeed= QString();
    _AltGround= QString();
    _SlatFlap= QString();
    _AltTarget= QString();
    _SpeedTarget= QString();
    _ParkBrakes= QString();
    _ATHR= QString();
    _AutoPilot1= QString();
    _AutoPilot2= QString();
    _DISA= QString();
    _AcPowered= QString();
    _Bat= QString();
    _ExtPower= QString();
    _APU= QString();
    _APUGen= QString();
    _APUBleed= QString();
    _EngineState= QString();
    _RotarySelector= QString();
    _MasterLevel= QString();
    _ThrottleState= QString();
    setConfigName(iConfigName);

}
void GTAInitConfiguration::setConfigName(const QString &iVal)
{
    _ConfigName = iVal;
    if(iVal == INIT_CONF1 )
        setConfigType(GTAInitConfigBase::DARK_COCKPIT);
    else if(iVal == INIT_CONF2)
        setConfigType(GTAInitConfigBase::GROUNDED_ENGINE_OFF_POWERED);
    else if(iVal == INIT_CONF3)
        setConfigType(GTAInitConfigBase::GROUNDED_ENGINE_ON);
    else if(iVal == INIT_CONF4)
        setConfigType(GTAInitConfigBase::IN_FLIGHT);
}

void GTAInitConfiguration::setAltSpeed(const QString & iVal)
{
    _AltSpeed = iVal;
}

void GTAInitConfiguration::setAltGround(const QString & iVal)
{
    _AltGround = iVal;
}

void GTAInitConfiguration::setSlatFlap(const QString & iVal)
{
    _SlatFlap = iVal;
}

void GTAInitConfiguration::setAltTarget(const QString & iVal)
{
    _AltTarget = iVal;
}

void GTAInitConfiguration::setSpeedTarget(const QString & iVal)
{
    _SpeedTarget = iVal;
}

void GTAInitConfiguration::setParkBrakes(const QString & iVal)
{
    _ParkBrakes = iVal;
}

void GTAInitConfiguration::setATHR(const QString & iVal)
{
    _ATHR =iVal;
}

void GTAInitConfiguration::setAutoPilot1(const QString & iVal)
{
    _AutoPilot1 = iVal;
}

void GTAInitConfiguration::setAutoPilot2(const QString & iVal)
{
    _AutoPilot2 = iVal;
}

void GTAInitConfiguration::setDISA(const QString & iVal)
{
    _DISA = iVal;
}

void GTAInitConfiguration::setAcPowered(const QString & iVal)
{
    _AcPowered =iVal;
}

void GTAInitConfiguration::setBat(const QString & iVal)
{
    _Bat = iVal;
}

void GTAInitConfiguration::setExternalPower(const QString & iVal)
{
    _ExtPower =iVal;
}

void GTAInitConfiguration::setAPU(const QString & iVal)
{
    _APU = iVal;
}

void GTAInitConfiguration::setAPUGen(const QString & iVal)
{
    _APUGen = iVal;
}

void GTAInitConfiguration::setAPUBleed(const QString & iVal)
{
    _APUBleed = iVal;
}

void GTAInitConfiguration::setEngineSate(const QString & iVal)
{
    _EngineState = iVal;
}

void GTAInitConfiguration::setRotarySelector(const QString & iVal)
{
    _RotarySelector = iVal;
}

void GTAInitConfiguration::setMasterLevel(const QString & iVal)
{
    _MasterLevel = iVal;
}

void GTAInitConfiguration::setThrottleState(const QString & iVal)
{
    _ThrottleState = iVal;
}
QString GTAInitConfiguration::getConfigName() const
{
    return _ConfigName;
}
QString GTAInitConfiguration::getAltSpeed()const
{
    return _AltSpeed;
}
QString GTAInitConfiguration::getAltGround()const
{
    return _AltGround;
}
QString GTAInitConfiguration::getSlatFlap()const
{
    return _SlatFlap;
}
QString GTAInitConfiguration::getAltTarget()const
{
    return _AltTarget;
}
QString GTAInitConfiguration::getSpeedTarget()const
{
    return _SpeedTarget;
}
QString GTAInitConfiguration::getParkBrakes()const
{
    return _ParkBrakes;
}
QString GTAInitConfiguration::getATHR()const
{
    return _ATHR;
}
QString GTAInitConfiguration::getAutoPilot1()const
{
    return _AutoPilot1;
}
QString GTAInitConfiguration::getAutoPilot2()const
{
    return _AutoPilot2;
}
QString GTAInitConfiguration::getDISA()const
{
    return _DISA;
}

QString GTAInitConfiguration::getAcPowered()const
{
    return _AcPowered;
}
QString GTAInitConfiguration::getBat()const
{
    return _Bat;
}
QString GTAInitConfiguration::getExternalPower()const
{
    return _ExtPower;
}
QString GTAInitConfiguration::getAPU()const
{
    return _APU;
}
QString GTAInitConfiguration::getAPUGen()const
{
    return _APUGen;
}
QString GTAInitConfiguration::getAPUBleed()const
{
    return _APUBleed;
}
QString GTAInitConfiguration::getEngineSate()const
{
    return _EngineState;
}
QString GTAInitConfiguration::getRotarySelector()const
{
    return _RotarySelector;
}
QString GTAInitConfiguration::getMasterLevel()const
{
    return _MasterLevel;
}
QString GTAInitConfiguration::getThrottleState()const
{
    return _ThrottleState;
}
QString GTAInitConfiguration::getStatement() const
{
    QString oStatement;
    QString configName = getConfigName();
    QString altGround = getAltGround();
    QString disa = getDISA();
    QString altSpeed = getAltSpeed();
    QString throttle = getThrottleState();
    QString engineState = getEngineSate();
    GTAInitConfigBase::ConfigurationType confType = getConfigType();
    switch(confType)
    {
    case GTAInitConfigBase::DARK_COCKPIT: oStatement = QString("%1\nGround altitude = %2\nDISA = %3 degC").arg(configName,altGround,disa); break;
    case GTAInitConfigBase::GROUNDED_ENGINE_OFF_POWERED: oStatement = QString("%1\nGround altitude = %2\nDISA = %3 degC").arg(configName,altGround,disa); break;
    case GTAInitConfigBase::GROUNDED_ENGINE_ON:  oStatement = QString("%1\nGround altitude = %2\nAlt/Spd = %3\nThrottle 1+2 = %4\nDISA = %5 degC").arg(configName,altGround,altSpeed,throttle,disa); break;
    case GTAInitConfigBase::IN_FLIGHT: oStatement=  QString("%1\nGround altitude = %2\nEngines = %3\nDISA = %4degC").arg(configName,altGround,engineState,disa);; break;
    }

    return oStatement;
}
