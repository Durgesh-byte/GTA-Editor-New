#ifndef GTASCXMLGENERICFUNCTION_H
#define GTASCXMLGENERICFUNCTION_H

#include "GTASCXMLUtils.h"
#include "GTASCXML.h"

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTASCXML_SHARED_EXPORT GTASCXMLGenericFunction
{
    struct funcAttributes
    {
        QString name;
        QString value;
    };
private:
    QString _ReturnName;
    QString _FunctionName;
    QList<funcAttributes> _Attrs;
public:
    GTASCXMLGenericFunction();
    void setFunctionName(const QString &iFunctionName);
    void setFunctionReturn(const QString &iReturnName);
    void insertFunctionAttributes(const QString &iName, const QString &iValue);
    QString getFunctionName() const;
    QString getFunctionReturnName() const;
    bool getFunctionAttributeByName(const QString &iAttrName, funcAttributes &oFuncAttrs);
    bool updateAttributeByName(const QString &iArgName,const QString &iValue);
    bool translateXMLToStruct(const QDomElement &iFunctionElem);
    QDomElement getSCXML(QDomDocument & iDomDoc)  const;

    bool isNull() const {return _FunctionName.isEmpty();}

};

#endif // GTASCXMLGENERICFUNCTION_H
