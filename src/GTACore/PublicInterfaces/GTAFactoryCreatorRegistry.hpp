#ifndef GTAFactoryCreatorRegistry_HPP_
#define GTAFactoryCreatorRegistry_HPP_

#include <QString>


template <class Factory, class Creator>
class GTAFactoryCreatorRegistry
{
public:
    GTAFactoryCreatorRegistry(const QString& iType)
    {
        Creator* pCreator = new Creator();
        if(!! pCreator)
        {
            Factory::getInstance().registerCreator(iType, pCreator);
        }
    }

    ~GTAFactoryCreatorRegistry(){}
};


#endif
