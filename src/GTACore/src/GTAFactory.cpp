#include "GTAFactory.h"
#include <QDebug>
#include <typeinfo>

GTAFactory::GTAFactory()
{
}

GTAFactory::GTAFactory(const GTAFactory& )
{

}

GTAFactory::~GTAFactory()
{
    QList<QString> keyList= _UiEquationStore.keys();
    foreach(QString sKey, keyList)
        unregisterEqnCreator(sKey);


    keyList=_UiCheckStore.keys();
    foreach(QString sKey, keyList)
        unregisterCheckCreator(sKey);

    keyList=_UiFailureStore.keys();
    foreach(QString sKey, keyList)
        unregisterFailProfileCreator(sKey);


    keyList=_UiSerializerStore.keys();
    foreach(QString sKey, keyList)
        unregisterSerializeCreator(sKey);


    keyList=_UiActionStore.keys();
    foreach(QString sKey, keyList)
        unregisterActionCreator(sKey);

   
    
}

bool GTAFactory::create(const QString &iType, GTAEquationUIInterface *&iopEquationWidget)
{
    bool rc = false;
	QString l_type = QString();

    if(iopEquationWidget != NULL)
        return rc;
	// We have to truncate the iType in order to get the real EquationName
	if (iType.contains("_"))
		l_type = iType.split("_").first();
	else
		l_type = iType;

	rc = _UiEquationStore.contains(l_type);
    if(rc)
    {
        iopEquationWidget = _UiEquationStore[l_type]->create();
    }
    return rc;
}

 
bool GTAFactory::registerCreator(const QString iType, IGTAObjCreator <GTAEquationUIInterface>* pCreator)
{
    unregisterEqnCreator(iType);
    _UiEquationStore[iType] = pCreator;
    return true;
}
bool GTAFactory::create(const QString &iType, GTAActionWidgetInterface *&iopActWdgt)
{
    bool rc = false;
    if(iopActWdgt != NULL)
        return rc;
    rc = _UiActionStore.contains(iType);
    if(rc)
    {
        iopActWdgt = _UiActionStore[iType]->create();
    }

    return rc;
}
bool GTAFactory::registerCreator(const QString iType, IGTAObjCreator <GTAActionWidgetInterface>* pCreator)
{
    unregisterActionCreator(iType);
    _UiActionStore[iType] = pCreator;
    return true;
} 
bool  GTAFactory::create(const QString& iType, GTACheckWidgetInterface *& iopCheckWdgt )
{
    bool rc = false;
    if(iopCheckWdgt != NULL)
        return rc;
    rc = _UiCheckStore.contains(iType);
    if(rc)
    {
        iopCheckWdgt = _UiCheckStore[iType]->create();
    }

    return rc;
}
bool GTAFactory::registerCreator(const QString iType, IGTAObjCreator <GTACheckWidgetInterface>* pCreator)
{
    unregisterCheckCreator(iType);
    _UiCheckStore[iType] = pCreator;
    return true;
}
bool GTAFactory::create(const QString& iType, GTAFailureProfileUIInterface *& iopFailProfileWdgt )
{
    bool rc = false;
    if(iopFailProfileWdgt != NULL)
        return rc;
    rc = _UiFailureStore.contains(iType);
    if(rc)
    {
        iopFailProfileWdgt = _UiFailureStore[iType]->create();
    }

    return rc;
}
bool GTAFactory::registerCreator(const QString iType, IGTAObjCreator <GTAFailureProfileUIInterface>* pCreator)
{
    unregisterFailProfileCreator(iType);
    _UiFailureStore[iType] = pCreator;
    return true;
}
bool GTAFactory::create(const QString& iType, GTACmdSerializerInterface *& iopCmdSerializer )
{
    bool rc = false;
    if(iopCmdSerializer != NULL)
        return rc;
    rc = _UiSerializerStore.contains(iType);
    if(rc)
    {
        iopCmdSerializer = _UiSerializerStore[iType]->create();
    }

    return rc;
}

bool GTAFactory::registerCreator(const QString iType, IGTAObjCreator <GTACmdSerializerInterface>* pCreator)
{
    unregisterSerializeCreator(iType);
    _UiSerializerStore[iType] = pCreator;
    return true;
}
bool GTAFactory::unregisterFailProfileCreator(const QString iType)
{
    bool bOk = false;
    if(_UiFailureStore.contains(iType))
    {
        delete _UiFailureStore[iType];
        _UiFailureStore.remove(iType);
        bOk = true;
    }
    return bOk;
}
bool GTAFactory::unregisterActionCreator(const QString iType)
{
    bool bOk = false;
    if(_UiActionStore.contains(iType))
    {
        delete _UiActionStore[iType];
        _UiActionStore.remove(iType);
        bOk = true;
    }
    return bOk;
}
bool GTAFactory::unregisterCheckCreator(const QString iType)
{
    bool bOk = false;
    if(_UiCheckStore.contains(iType))
    {
        delete _UiCheckStore[iType];
        _UiCheckStore.remove(iType);
        bOk = true;
    }
    return bOk;
}
bool GTAFactory::unregisterEqnCreator(const QString iType)
{
    bool bOk = false;
    if(_UiEquationStore.contains(iType))
    {
        delete _UiEquationStore[iType];
        _UiEquationStore.remove(iType);
        bOk = true;
    }
    
    return bOk;
}
bool GTAFactory::unregisterSerializeCreator(const QString iType)
{
    bool bOk = false;
    if(_UiSerializerStore.contains(iType))
    {
        delete _UiSerializerStore[iType];
        _UiSerializerStore.remove(iType);
        bOk = true;
    }

    return bOk;
}
GTAFactory& GTAFactory::getInstance()
{
    static GTAFactory _instance;
    return _instance;
}


