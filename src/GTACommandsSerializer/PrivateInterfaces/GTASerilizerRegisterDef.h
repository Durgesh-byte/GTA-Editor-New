#include "IGTAObjCreator.hpp"
#include "GTAFactoryCreatorRegistry.hpp"
#include "GTAFactory.h"
#include "GTAObjCreator.hpp"


#define REGISTER_SERIALIZER(Name,SerializerClass) GTAFactoryCreatorRegistry< GTAFactory,                                    \
    GTAObjCreator<GTACmdSerializerInterface,SerializerClass> > obj(Name);

#define REGISTER_SERIALIZER_MULTI(Name,SerializerClass,ObjInstanceName) GTAFactoryCreatorRegistry< GTAFactory,                                    \
    GTAObjCreator<GTACmdSerializerInterface,SerializerClass> > ObjInstanceName(Name);