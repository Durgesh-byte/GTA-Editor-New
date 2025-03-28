#ifndef IGTAObjCreator_HPP_
#define IGTAObjCreator_HPP_
template <class Intf>
struct IGTAObjCreator
{
    virtual ~IGTAObjCreator() {}
    virtual Intf* create() = 0;
};
#endif