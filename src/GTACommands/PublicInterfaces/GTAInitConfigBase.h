#ifndef GTAINITCONFIGBASE_H
#define GTAINITCONFIGBASE_H
#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAInitConfigBase
{
public:
    enum ConfigurationType{IN_FLIGHT,GROUNDED_ENGINE_ON,GROUNDED_ENGINE_OFF_POWERED,DARK_COCKPIT};
    GTAInitConfigBase();

    inline void setConfigType(ConfigurationType iType) {_Config = iType;}
    inline ConfigurationType getConfigType() const {return _Config;}

    virtual QString getStatement() const = 0;
private:
    ConfigurationType _Config;
};

#endif // GTAINITCONFIGBASE_H
