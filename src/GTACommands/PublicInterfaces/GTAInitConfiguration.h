#ifndef GTAINITCONFIGURATION_H
#define GTAINITCONFIGURATION_H
#include "GTACommands.h"
#include "GTAInitConfigBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAInitConfiguration : public GTAInitConfigBase
{
public:
    GTAInitConfiguration(const QString iConfigName = QString());
    void setConfigName(const QString & iVal);
    void setAltSpeed(const QString & iVal);
    void setAltGround(const QString & iVal);
    void setSlatFlap(const QString & iVal);
    void setAltTarget(const QString & iVal);
    void setSpeedTarget(const QString & iVal);
    void setParkBrakes(const QString & iVal);
    void setATHR(const QString & iVal);
    void setAutoPilot1(const QString & iVal);
    void setAutoPilot2(const QString & iVal);
    void setDISA(const QString & iVal);

    void setAcPowered(const QString & iVal);
    void setBat(const QString & iVal);
    void setExternalPower(const QString & iVal);
    void setAPU(const QString & iVal);
    void setAPUGen(const QString & iVal);
    void setAPUBleed(const QString & iVal);
    void setEngineSate(const QString & iVal);
    void setRotarySelector(const QString & iVal);
    void setMasterLevel(const QString & iVal);
    void setThrottleState(const QString & iVal);


    QString getConfigName() const;
    QString getAltSpeed()const;
    QString getAltGround()const;
    QString getSlatFlap()const;
    QString getAltTarget()const;
    QString getSpeedTarget()const;
    QString getParkBrakes()const;
    QString getATHR()const;
    QString getAutoPilot1()const;
    QString getAutoPilot2()const;
    QString getDISA()const;

    QString getAcPowered()const;
    QString getBat()const;
    QString getExternalPower()const;
    QString getAPU()const;
    QString getAPUGen()const;
    QString getAPUBleed()const;
    QString getEngineSate()const;
    QString getRotarySelector()const;
    QString getMasterLevel()const;
    QString getThrottleState()const;

    virtual QString getStatement() const;
private:
    QString _ConfigName;
    QString _AltSpeed;
    QString _AltGround;
    QString _SlatFlap;
    QString _AltTarget;
    QString _SpeedTarget;
    QString _ParkBrakes;
    QString _ATHR;
    QString _AutoPilot1;
    QString _AutoPilot2;
    QString _DISA;
    QString _AcPowered;
    QString _Bat;
    QString _ExtPower;
    QString _APU;
    QString _APUGen;
    QString _APUBleed;
    QString _EngineState;
    QString _RotarySelector;
    QString _MasterLevel;
    QString _ThrottleState;

};

#endif // GTAINITCONFIGURATION_H
