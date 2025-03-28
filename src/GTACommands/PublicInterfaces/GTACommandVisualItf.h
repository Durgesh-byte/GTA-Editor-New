#ifndef GTACommandVisualItf_H
#define GTACommandVisualItf_H
#include "GTACommands.h"
#include "GTAScxmlLogMessage.h"

#pragma warning(push, 0)
#include <Qlist>
#include <QColor>
#include <QFont>
#include <QMap>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTACommandVisualItf
{
public:
    
    

    GTACommandVisualItf(){ _searchHighlight=false;};
    virtual ~GTACommandVisualItf(){};

    
    //Visual Property
    virtual QColor getForegroundColor() const = 0;
    virtual QColor getBackgroundColor() const = 0;
    virtual bool isForegroundColor() const = 0;
    virtual QFont getFont() const = 0;

    void setSearchHighlight(bool iHighLight)
    {
        _searchHighlight=iHighLight;
    }
    bool getSearchHighlight()const
    {
        return _searchHighlight;
    }

    void setCompatibilityHighLight(bool iHighlight)
    {
        _compatibilityHighlight = iHighlight;
    }

    bool getCompatibilityHighLight()const
    {
        return _compatibilityHighlight;
    }
   
private:

    bool _compatibilityHighlight;
    bool _searchHighlight;
    
   
   
};

#endif // GTACommandVisualItf_H

