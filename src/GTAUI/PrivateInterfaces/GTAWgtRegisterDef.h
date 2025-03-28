#include "IGTAObjCreator.hpp"
#include "GTAFactoryCreatorRegistry.hpp"
#include "GTAFactory.h"
#include "GTAObjCreator.hpp"

#define REGISTER_EQUATION_WIDGET(Name,WgtClass) GTAFactoryCreatorRegistry< GTAFactory,                      \
                                                GTAObjCreator<GTAEquationUIInterface,WgtClass> > obj(Name);


//REGNAME : should be unique across the tool
//INSTANCENAME: should be unique across one widget class.
// example REGISTER_ACTION_WIDGET(COMMAND_NAME,WIDGET_CLASS,OBJECT_NAME);
#define REGISTER_ACTION_WIDGET(REGNAME,WgtClass,INSTANCENAME) GTAFactoryCreatorRegistry< GTAFactory,                        \
    GTAObjCreator<GTAActionWidgetInterface,WgtClass> > INSTANCENAME(REGNAME);



#define REGISTER_CHECK_WIDGET(Name,WgtClass) GTAFactoryCreatorRegistry< GTAFactory,                                         \
    GTAObjCreator<GTACheckWidgetInterface,WgtClass> > obj(Name);


#define REGISTER_FAILURE_WIDGET(Name,WgtClass) GTAFactoryCreatorRegistry< GTAFactory,                                       \
    GTAObjCreator<GTAFailureProfileUIInterface,WgtClass> > obj(Name);



#define REGISTER_SERIALIZER(Name,SerializerClass) GTAFactoryCreatorRegistry< GTAFactory,                                    \
    GTAObjCreator<GTACmdSerializerInterface,SerializerClass> > obj(Name);