#ifndef GTASCXMLXMLRPC_H
#define GTASCXMLXMLRPC_H

#include "GTASCXML.h"
#include "GTASCXMLXMLRPC.h"
#include "GTASCXMLUtils.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTASCXML_SHARED_EXPORT GTASCXMLXMLRPC
{

public:

    struct funcArg
    {
        QString type;
        QString name;
        QString value;
    };
    struct funcReturn
    {
        QString name;
        QString value;
        QString type;
    };


    GTASCXMLXMLRPC();
    void setNodeType(const QString &iNodeType);
    void setToolId(const QString &iToolId);
    void setToolType(const QString &iToolType);
    void setFunctionName(const QString &iFuncName);
    void insertFunctionArgument(const QString &iArgType,const QString &iArgName,const QString &iArgValue);
    void insertFunctionReturn(const QString &iName, const QString &iType);
    bool getArgumentByName(const QString &iArgName,funcArg &oFuncArg);
    bool updateArgumentByName(const QString &iArgName,funcArg &iFuncArg);
    bool translateXMLToStruct(const QDomElement &iTransitionElem);
    bool getReturnArgument(funcReturn &oRet);
    bool getFunctionArgument(funcArg &oRet);
    bool updateReturnArgument(funcReturn &iRet);

    QDomElement getSCXML(QDomDocument & iDomDoc)  const;

    bool isNull() const {return _FunctionName.isEmpty();}

private:
    QList<funcArg> _Args;
    QList<funcReturn> _Returns;
    QString _ToolId;
    QString _ToolType;
    QString _FunctionName;
    QString _NodeType;

};

#endif // GTASCXMLXMLRPC_H
