#include "GTAGenericToolCommand.h"


GTAGenericToolCommand::GTAGenericToolCommand():_isMismatched(false),_isSCXMLModSelected(false)
{
}
GTAGenericToolCommand::GTAGenericToolCommand(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement)
{
	_isSCXMLModSelected = false;
}
GTAGenericToolCommand::GTAGenericToolCommand(const GTAGenericToolCommand& rhs):GTAActionBase(rhs)
{
    setToolName(rhs.getToolName());

    setToolDisplayName(rhs.getToolDisplayName());
    
    setFunctions(rhs.getCommand());

    setDefinitions(rhs.getDefinitions());

    setUserDbList(rhs.getUserDbList());

    setSCXMLModSelected(rhs.getSCXMLModSelected());

}



QString GTAGenericToolCommand::getStatement() const
{
    //return _ToolName;
    QStringList statement;
    for(int i=0;i<_SelectedFunctions.count();i++)
    {
        QString cmdStatement;
        GTAGenericFunction function = _SelectedFunctions.at(i);
        QString functionDisplayName = function.getDisplayName();
        QStringList items = function.getArgsToUseList();
        QList<GTAGenericArgument> argList = function.getFunctionArguments();
        QStringList argListDisplay;
        if(!items.isEmpty())
        {
            for(int j = 0;j<items.count();j++)
            {
                GTAGenericArgument arg1;
                arg1.setArgumentName(items.at(i));
                int idx = argList.indexOf(arg1);
                if(idx>=0)
                {
                    QString val = argList[idx].getUserSelectedValue();
                    QString label = argList[idx].getHMILabel();
                    if(label.isEmpty())
                        label = argList[idx].getArgumentName();
                    QString argDis = QString("%1 : %2").arg(label,val);
                    argListDisplay.append(argDis);
                }
            }
        }
        else
        {
            for(int j = 0;j<argList.count();j++)
            {

                QString val = argList[j].getUserSelectedValue();
                QString label = argList[j].getHMILabel();
                if(label.isEmpty())
                    label = argList[j].getArgumentName();
                QString argDis = QString("%1 : %2").arg(label,val);
                argListDisplay.append(argDis);

            }
        }

        QString name;
        if(_ToolDisplayName.isEmpty())
            name = _ToolName;
        else
            name = _ToolDisplayName;

        cmdStatement += QString("%1 : %2 (%3)[ExternalToolID:%4]\n").arg(name,functionDisplayName,argListDisplay.join(", "),getObjId()).trimmed();
        statement.append(cmdStatement);
    }
    return statement.join("");
}




bool GTAGenericToolCommand::hasPrecision() const
{
    return false;
}
bool GTAGenericToolCommand::hasTimeOut() const
{
    return true;
}

//void GTAGenericToolCommand::setHasTimeOut(const bool &val)
//{
//    _hasTimeOut = val;
//}

/*void GTAGenericToolCommand::setStatement(const QList<GTAGenericFunction> iFunctions)
{
    _SelectedFunctions = iFunctions;
}*/

bool GTAGenericToolCommand::isWaitUntilEnabled(const QString &iReturnParamName,const QString &iReturnType) const
{
    bool rc = false;
    GTAGenericParamData objReturnParam;
    GTAGenericAttribute objAttribute;
    bool found = false;
    for(int i = 0; i < _Definitions.count(); i++)
    {
        objReturnParam = _Definitions.at(i);
        if(objReturnParam.getParamDataName() == iReturnParamName)
        {
            found = true;
            break;
        }
    }
    if(iReturnType != "struct")
        rc = false;
    if(found)
    {
        QList<GTAGenericAttribute> lstAttributes = objReturnParam.getParamAttributeList();

        for(int i = 0; i < lstAttributes.count(); i++)
        {
            objAttribute = lstAttributes.at(i);
            if(objAttribute.getReturnCode() == RETURN_CODE_TRUE)
            {
                rc=objAttribute.hasWaitUntil();
                break;
            }
        }
    }
    return rc;
}

void GTAGenericToolCommand::insertFunction(GTAGenericFunction iFuncObj)
{
    _SelectedFunctions.append(iFuncObj);
}

void GTAGenericToolCommand::removeFunction(GTAGenericFunction iFuncObj)
{

    QListIterator <GTAGenericFunction> itr(_SelectedFunctions);
    while(itr.hasNext())
    {

        if(_SelectedFunctions.contains(iFuncObj))
        {
            int index = _SelectedFunctions.indexOf(iFuncObj);
            _SelectedFunctions.removeAt(index);
        }

    }
}


void GTAGenericToolCommand::setDefinitions(const QList<GTAGenericParamData> &iDefinitions)
{
    _Definitions = iDefinitions;
}

QList<GTAGenericParamData> GTAGenericToolCommand::getDefinitions() const
{
    return _Definitions;
}

QString GTAGenericToolCommand::getElement()const
{
    return QString("Generic Command");
}

QList<GTAGenericFunction> GTAGenericToolCommand::getCommand() const

{
    return _SelectedFunctions;
}


QList<GTACommandBase*>& GTAGenericToolCommand::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAGenericToolCommand::insertChildren(GTACommandBase*, const int&)
{
    return false;
}

void  GTAGenericToolCommand::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAGenericToolCommand::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTAGenericToolCommand::getClone() const
{
    return  new GTAGenericToolCommand(*this);
}
bool GTAGenericToolCommand::hasReference()
{
    return false;
}

bool GTAGenericToolCommand::canHaveChildren() const
{
    return false;
}

QString GTAGenericToolCommand::getCommandPath() const
{
    QString instanceName = getInstanceName();
    int idx = instanceName.indexOf("[");
    if(idx>0)
    {
        QStringList items = instanceName.split("[",QString::SkipEmptyParts);
        items.removeAt(0);
        return items.join("");
    }

    instanceName.append(getObjId());
    return instanceName;
}
QString GTAGenericToolCommand::getLTRAStatement()  const
{
    //return _ToolName;
    QStringList statement;
    for(int i=0;i<_SelectedFunctions.count();i++)
    {
        QString cmdStatement;
        GTAGenericFunction function = _SelectedFunctions.at(i);
        QString functionDisplayName = function.getDisplayName();
        QStringList items = function.getArgsToUseList();
        QList<GTAGenericArgument> argList = function.getFunctionArguments();
        QStringList argListDisplay;
        if(!items.isEmpty())
        {
            for(int j = 0;j<items.count();j++)
            {
                GTAGenericArgument arg1;
                arg1.setArgumentName(items.at(i));
                int idx = argList.indexOf(arg1);
                if(idx>=0)
                {
                    argListDisplay.append(getGenericArgumentInfos(argList, j));
                }
            }
        }
        else
        {
            for(int j = 0;j<argList.count();j++)
            {
                argListDisplay.append(getGenericArgumentInfos(argList, j));
            }
        }

        QString name;
        if(_ToolDisplayName.isEmpty())
            name = _ToolName;
        else
            name = _ToolDisplayName;
        cmdStatement += QString("#c#%1#c# : %2 (%3)\n").arg(name,functionDisplayName,argListDisplay.join(", ")).trimmed();
        statement.append(cmdStatement);

    }
    return statement.join("");
}

/**
 * .
 * @brief The goal of this function is to prepared the string statement with args of function that will pushed in cell. 
 * @param iArgList list of GenericArgument present in the function
 * @param j id of the current GenerecArgument
 * @return QString with the format could be interpreted by GTATransformeDocx to create
 */
QString GTAGenericToolCommand::getGenericArgumentInfos(QList<GTAGenericArgument>& iArgList, int j) const
{
    QString val = iArgList[j].getUserSelectedValue();
    QString label = iArgList[j].getHMILabel();
    if (label.isEmpty())
        label = iArgList[j].getArgumentName();
    QString color = iArgList[j].getColor().toLower();
    QString argDis = "";
    if (color.isEmpty())
        argDis = QString("%1 : %2").arg(label, val);
    else
        argDis = QString("%1 : #color##%3#%2#%3##color#").arg(label, val, color);
    return argDis;
}

bool GTAGenericToolCommand::getSCXMLModSelected() const
{
    return _isSCXMLModSelected;
}


void GTAGenericToolCommand::setSCXMLModSelected(const bool &iVal)
{
    _isSCXMLModSelected = iVal;
}

QStringList GTAGenericToolCommand::getVariableList() const
{
    QStringList items;
    for(int i=0;i<_SelectedFunctions.count();i++)
    {
        GTAGenericFunction func = _SelectedFunctions.at(i);
        items.append(func.getVariableList());
    }
    items.removeDuplicates();
    return items;
}
void GTAGenericToolCommand::ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QList<GTAGenericFunction> resolvedSelectedFunctions;

    for(int i=0;i<_SelectedFunctions.count();i++)
    {
        GTAGenericFunction func = _SelectedFunctions.at(i);
        func.ReplaceTag(iTagValueMap);
        resolvedSelectedFunctions.append(func);
    }
    _SelectedFunctions.clear();
    _SelectedFunctions.append(resolvedSelectedFunctions);
}
void GTAGenericToolCommand::setToolDisplayName(const QString &iToolName)
{
    _ToolDisplayName = iToolName;
}

QString GTAGenericToolCommand::getToolDisplayName()const
{
    return _ToolDisplayName;
}

void GTAGenericToolCommand::setToolName(const QString &iToolName)
{
    _ToolName = iToolName;
    //    if(_ToolDisplayName.isEmpty())
    //        _ToolDisplayName = iToolName;
}
QString GTAGenericToolCommand::getToolName()const
{
    return _ToolName;
}


bool GTAGenericToolCommand::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;
    if(ibTextSearch)
    {
        return _ToolName.contains(iRegExp);

    }

    return rc;
}

QString  GTAGenericToolCommand::getSCXMLStateName() const
{
    return QString("External Tool_%1").arg(_ToolName);
}

/*bool GTAGenericToolCommand::getSCXMLConditionStatement(const QString &iStateNameWithReturnParamName, const QString &iReturnParamName, const bool &iCondition,QString &oReturnCode) const
{
    oReturnCode = QString();
    if(!_Definitions.isEmpty())
    {

        bool rc = false;
        QString condString;
        GTAGenericParamData objReturnParam;
        bool found = false;
        for(int i = 0; i < _Definitions.count(); i++)
        {
            objReturnParam = _Definitions.at(i);
            if(objReturnParam.getParamDataName() == iReturnParamName)
            {
                found = true;
                break;
            }
        }

        if(found)
        {
            bool retCodeFound = false;
            QList<GTAGenericAttribute> lstAttributes = objReturnParam.getParamAttributeList();
            GTAGenericAttribute objAttribute;
            for(int i = 0; i < lstAttributes.count(); i++)
            {
                objAttribute = lstAttributes.at(i);
                if(objAttribute.getReturnCode() == RETURN_CODE_TRUE)
                {
                    retCodeFound = true;
                    break;
                }

            }
            if(retCodeFound)
            {
                condString = iStateNameWithReturnParamName + "." + objAttribute.getAttributeName();

                if(iCondition == true)
                {
                    QString trueCond1 = objAttribute.getTrueCondition();
                    if(!trueCond1.isEmpty())
                    {
                        QString trueCond = QString("'%1'").arg(trueCond1);
                        oReturnCode = QString("%1%2%3").arg(condString,"==",trueCond);
                    }
                    else
                    {
                        oReturnCode = QString("%1").arg(condString);
                        return false;
                    }
                }
                else
                {
                    QString falseCond1 = objAttribute.getFalseCondition();
                    if(!falseCond1.isEmpty())
                    {
                        QString falseCond = QString("'%1'").arg(falseCond1);
                        oReturnCode = QString("%1%2%3").arg(condString,"==",falseCond);
                    }
                    else
                    {
                        oReturnCode = QString("%1").arg(condString);
                        return false;
                    }
                }

                rc = true;
                return rc;
            }
        }

        return rc;
    }
}*/


bool GTAGenericToolCommand::getSCXMLConditionStatement(const QString &iStateNameWithReturnParamName, const QString &iReturnParamName, const bool &iCondition,QString &oReturnCode, const QString &iReturnType) const
{
    oReturnCode = QString();
    if(!_Definitions.isEmpty())
    {

        bool rc = false;
        bool makeCondition = false;
        bool isReturnSimple = false;
        GTAGenericParamData objReturnParam;
        GTAGenericAttribute objAttribute;
        bool found = false;
        for(int i = 0; i < _Definitions.count(); i++)
        {
            objReturnParam = _Definitions.at(i);
            if(objReturnParam.getParamDataName() == iReturnParamName)
            {
                found = true;
                break;
            }
        }
        if(iReturnType != "struct")
            found = false;
        if(found)
        {
            bool retCodeFound = false;
            QList<GTAGenericAttribute> lstAttributes = objReturnParam.getParamAttributeList();

            for(int i = 0; i < lstAttributes.count(); i++)
            {
                objAttribute = lstAttributes.at(i);
                if(objAttribute.getReturnCode() == RETURN_CODE_TRUE)
                {
                    retCodeFound = true;
                    break;
                }

            }
            if(retCodeFound)
            {
                makeCondition = true;
            }
        }
        else
        {
            if(!_SelectedFunctions.isEmpty())
            {
                GTAGenericFunction objFunc;
                for(int i = 0; i < _SelectedFunctions.count(); i++)
                {
                    objFunc = _SelectedFunctions.at(i);
                    if(objFunc.getReturnParamName() == iReturnParamName)
                    {
                        objAttribute = objFunc.getFunctionReturnType();
                        isReturnSimple = objFunc.isReturnTypeSimple();
                        makeCondition = true;
                        break;
                    }
                }

            }
        }

        if(makeCondition)
        {
            QString condString;
            if(isReturnSimple)
                condString = iStateNameWithReturnParamName;// + "." + objAttribute.getAttributeName();
            else
                condString = iStateNameWithReturnParamName + "." + objAttribute.getAttributeName();

            if(iCondition == true)
            {
                QString trueCond1 = objAttribute.getTrueCondition();
                if(!trueCond1.isEmpty())
                {
                    QString trueCond = QString("'%1'").arg(trueCond1);
                    oReturnCode = QString("%1%2%3").arg(condString,"==",trueCond);
                    return true;
                }
                else
                {
                    oReturnCode = QString("%1").arg(condString);
                    return false;
                }
            }
            else
            {
                QString falseCond1 = objAttribute.getFalseCondition();
                if(!falseCond1.isEmpty())
                {
                    QString falseCond = QString("'%1'").arg(falseCond1);
                    oReturnCode = QString("%1%2%3").arg(condString,"==",falseCond);
                    return true;
                }
                else
                {
                    oReturnCode = QString("%1").arg(condString);
                    return false;
                }
            }
        }

        return rc;
    }
	return false;
}

void GTAGenericToolCommand::setUserDbList(const QList<GTAGenericDB> &iDbList)
{
    _UserDb = iDbList;
}

QList<GTAGenericDB> GTAGenericToolCommand::getUserDbList()const
{
    return _UserDb;
}

void GTAGenericToolCommand::setMismatch(bool iVal)
{
    _isMismatched = iVal;
}

bool GTAGenericToolCommand::isMismatch()
{
    return _isMismatched;
}

void GTAGenericToolCommand::setForegroundColor(const QString& iColor){
    _Color = iColor;
}

/**
 * .
 * @brief Find if the current cell in the view procedure should apply color on his text. 
 * @return boolean, to validate if there are a color or not. 
 */
bool GTAGenericToolCommand::isForegroundColor() const {
    for (const auto& function : _SelectedFunctions) {
        if (!function.getColor().isEmpty()) {
            return true;
        }
    }
    return false;
}

/**
 * .
 * @brief If one of this function have color, we applied this color
 * @return QColor to should apply at the cell in the procedure view. 
 */
QColor GTAGenericToolCommand::getForegroundColor() const
{
    for (const auto& function : _SelectedFunctions) {
        if (!function.getColor().isEmpty()) {
            return GTAUtil::getTextColor(function.getColor());
        }
    }
    return QColor(Qt::black);
}

/**
 * .
 * @brief Apply a background color in the procedure table. 
 * @return QColor object for the background cell.
 */
QColor GTAGenericToolCommand::getBackgroundColor() const
{
    return kBackgroudColor;
}