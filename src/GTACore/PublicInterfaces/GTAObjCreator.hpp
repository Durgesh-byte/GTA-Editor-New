#ifndef GTAObjCreator_HPP_
#define GTAObjCreator_HPP_
#include "IGTAObjCreator.hpp"

template <class Intf, class Impl>
class GTAObjCreator: public IGTAObjCreator<Intf>
{
public:
    GTAObjCreator() {}
    virtual ~GTAObjCreator() {}
    virtual Intf* create()
    {
        Intf* pIntf = NULL;
        Impl* pImpl = new Impl();
        if(!! pImpl)
        {
            pIntf = dynamic_cast<Intf*> (pImpl);
            if(! pIntf)
            {
                delete pImpl;
                pImpl = NULL;
            }
        }
        return pIntf;
    }
};
#endif
