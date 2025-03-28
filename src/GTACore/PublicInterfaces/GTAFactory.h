#ifndef GTAUIFACTORY_H
#define GTAUIFACTORY_H
#include "IGTAObjCreator.hpp"
#include "GTAEquationUIInterface.h"
#include "GTAActionWidgetInterface.h"
#include "GTACheckWidgetInterface.h"
#include "GTAFailureProfileUIInterface.h"
#include "GTACmdSerializerInterface.h"
#include "GTACore.h"

#pragma warning(push, 0)
#include <QMap>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTACore_SHARED_EXPORT GTAFactory
{
private:
    GTAFactory();
    ~GTAFactory();
    GTAFactory(const GTAFactory&);
    GTAFactory& operator=(const GTAFactory&);//{return *this;}
    QMap<QString, IGTAObjCreator <GTAEquationUIInterface>*>_UiEquationStore;
    QMap<QString, IGTAObjCreator <GTACheckWidgetInterface>*>_UiCheckStore;
    QMap<QString, IGTAObjCreator <GTAActionWidgetInterface>*>_UiActionStore;
    QMap<QString, IGTAObjCreator <GTAFailureProfileUIInterface>*>_UiFailureStore;
    QMap<QString, IGTAObjCreator <GTACmdSerializerInterface>*>_UiSerializerStore;

public:

    
    static GTAFactory& getInstance();
    bool create(const QString& iType, GTAEquationUIInterface *& iopEquationWidget );
    bool create(const QString& iType, GTAActionWidgetInterface *& iopCheckWdgt );
    bool create(const QString& iType, GTACheckWidgetInterface *& iopActWdgt );
    bool create(const QString& iType, GTAFailureProfileUIInterface *& iopFailProfileWdgt );
    bool create(const QString& iType, GTACmdSerializerInterface *& iopCmdSerializer );

    bool registerCreator(const QString iType, IGTAObjCreator <GTAEquationUIInterface>* pCreator);
    bool registerCreator(const QString iType, IGTAObjCreator <GTAActionWidgetInterface>* pCreator);
    bool registerCreator(const QString iType, IGTAObjCreator <GTACheckWidgetInterface>* pCreator);
    bool registerCreator(const QString iType, IGTAObjCreator <GTAFailureProfileUIInterface>* pCreator); 
    bool registerCreator(const QString iType, IGTAObjCreator <GTACmdSerializerInterface>* pCreator);
    
    bool unregisterActionCreator(const QString iType);
    bool unregisterCheckCreator(const QString iType);
    bool unregisterEqnCreator(const QString iType);
    bool unregisterFailProfileCreator(const QString iType);
    bool unregisterSerializeCreator(const QString iType);
};

#endif // GTAUIFACTORY_H
