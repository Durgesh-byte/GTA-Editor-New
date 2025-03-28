#include "GTAGenericFunction.h"

GTAGenericFunction::GTAGenericFunction():_isReturnTypeSimple(false)
{
    _isAcknowledgementRequested = true;
}


QString GTAGenericFunction::getFunctionName()
{
    return _Name;
}

void GTAGenericFunction::setFunctionName(const QString iFunction)
{
    _Name = iFunction;
    if(_DisplayName.isEmpty())
        _DisplayName = _Name;
}

//Returns only visible args
QList<GTAGenericArgument> GTAGenericFunction::getFunctionArguments()const
{
    QList<GTAGenericArgument> args;
    for(int i = 0; i < _Arguments.count(); i++)
    {
        GTAGenericArgument argObj = _Arguments.at(i);
        if(argObj.isDisplayed())
            args.append(argObj);
    }
    return args;
}

void GTAGenericFunction::setFunctionArguments(const QList<GTAGenericArgument> iArguments, bool isCallerSerializer)
{
    if(!_Arguments.isEmpty())
    {
        _Arguments.clear();
        _argDependencyTable.clear();
        _colorOfArgs.clear();
        _backColorOfArgs.clear();
    }

    for (GTAGenericArgument arg : iArguments)
    {   
        // reformat strings for compatibility
        QString argValue = arg.getUserSelectedValue();
        if (arg.getArgumentType() == "string") {
            if (!arg._isStringVariable) {
                if (!(argValue.startsWith("'") && argValue.endsWith("'") || argValue.startsWith("\"") && argValue.endsWith("\""))) {
                    argValue = "'" + argValue + "'";
                    argValue.replace("\"", "'");
                }
                arg.setUserSelectedValue(argValue);
            }
            else {
                if (argValue.isEmpty()) {
                    argValue = "''";
                }
                if (argValue.startsWith("'")) {
                    argValue.remove(0, 1);
                }
                if (argValue.endsWith("'")) {
                    argValue.chop(1);
                }
                arg.setUserSelectedValue(argValue);
            }
        }
        _Arguments.append(arg);
        setColorTableArg(arg.getArgumentName(), arg.getColor(), arg.getBackColor());
    }
    fillArgDependencyTable();

    if(!isCallerSerializer)
    {
        setConstantValues(_Arguments);
        setDefaultValues(_Arguments);
    }
}


void GTAGenericFunction::fillArgDependencyTable()
{
    for(int i = 0; i < _Arguments.count(); i++)
    {
        GTAGenericArgument argument = _Arguments.at(i);
        QString condition = argument.getCondition();
        if(!condition.isEmpty())
        {
            _argDependencyTable.insert(argument.getCondition(),argument.getArgumentName());
        }
    }
}


void GTAGenericFunction::setDefaultValues(QList<GTAGenericArgument> &oArgList)
{
    bool changed;

    for(int i = 0; i < oArgList.count(); i++)
    {
        changed = false;


        GTAGenericArgument argument = oArgList.at(i);
        QString userVal = argument.getUserSelectedValue();
        if(!argument.isArgDefault() && userVal.isEmpty())
        {
            QStringList defaultList = argument.getDefaultValue();
            QStringList valueList = argument.getValues();


            if(!argument.isArgConst() && !defaultList.isEmpty())
            {

                if(argument.isConditionValListEmpty())
                {
                    if(defaultList.count() == 1)
                    {
                        QString findStr = defaultList.at(0);
                        if(valueList.isEmpty())
                        {
                            argument.setUserSelectedValue(findStr);
                            changed = true;
                        }
                        else
                        {
                            if(valueList.contains(findStr))
                            {
                                argument.setUserSelectedValue(findStr);
                                changed = true;
                            }
                            else
                            {
                                changed = false;
                            }
                        }
                        
                    }
                }

            }
            if(changed)
            {
                argument.setArgIsDefault(true);
                _colorOfArgs.remove(oArgList.at(i).getArgumentName());
                _backColorOfArgs.remove(oArgList.at(i).getArgumentName());
                oArgList.removeAt(i);
                oArgList.insert(i,argument);
                setColorTableArg(argument.getArgumentName(), argument.getColor(), argument.getBackColor());
            }
        }
    }

}

void GTAGenericFunction::setConstantValues(QList<GTAGenericArgument> &oArgList)
{
    bool changed;
    for(int i = 0; i < oArgList.count(); i++)
    {
        changed = false;


        GTAGenericArgument argument = oArgList.at(i);
        QString userVal = argument.getUserSelectedValue();
        if(!argument.isArgConst() && userVal.isEmpty())
        {
            QStringList constantList = argument.getConstantValue();
            QStringList valueList = argument.getValues();

            if(!constantList.isEmpty())
            {
                if(!argument.isConditionValListEmpty())
                {

                    changed = true;
                }
                else
                {
                    if(constantList.count() == 1)
                    {
                        QString findStr = constantList.at(0);
                        if(valueList.isEmpty())
                        {

                            argument.setUserSelectedValue(findStr);
                            changed = true;


                        }
                        else
                        {
                            if(valueList.contains(findStr))
                            {
                                argument.setUserSelectedValue(findStr);
                                changed = true;
                            }
                            else
                            {
                                changed = false;
                            }
                            
                        }

                    }
                }

            }
            if(changed)
            {

                argument.setArgIsConst(true);
                _colorOfArgs.remove(oArgList.at(i).getArgumentName());
                _backColorOfArgs.remove(oArgList.at(i).getArgumentName());
                oArgList.removeAt(i);
                oArgList.insert(i,argument);
                setColorTableArg(argument.getArgumentName(), argument.getColor(), argument.getBackColor());
            }

        }

    }
}

QHash<QString, QString>  GTAGenericFunction::getArgDependencyTable()const
{
    return _argDependencyTable;
}

GTAGenericAttribute GTAGenericFunction::getFunctionReturnType()const
{
    return _returnAttribute;
}

void GTAGenericFunction::setFunctionReturnType(const GTAGenericAttribute &iReturnParam)
{
    _returnAttribute = iReturnParam;
}


QString GTAGenericFunction::getToolType()
{
    return _ToolType;
}

void GTAGenericFunction::setToolType(const QString iToolType)
{
    _ToolType = iToolType;
}

QString GTAGenericFunction::getToolID()
{
    return _ToolID;
}

void GTAGenericFunction::setToolID(const QString iToolID)
{
    _ToolID = iToolID;
}

void GTAGenericFunction::setReturnParamName(const QString iReturnParamName)
{
    _ReturnParamName = iReturnParamName;
}

QString GTAGenericFunction::getReturnParamName()
{
    return _ReturnParamName;
}

void GTAGenericFunction::setReturnParamType(const QString iReturnParamType)
{
    _ReturnParamType = iReturnParamType;
}

QString GTAGenericFunction::getReturnParamType()
{
    return _ReturnParamType;
}

void GTAGenericFunction::setFunctionSeletced(const bool iSelected)
{
    _Selected = iSelected;
}

void GTAGenericFunction::setWaitForAcknowledgement(const bool &iAcknowledgement)
{
    _isAcknowledgementRequested = iAcknowledgement;
}

bool GTAGenericFunction::getWaitForAcknowledgement()const
{
    return _isAcknowledgementRequested;
}

bool GTAGenericFunction::isFunctionSelected()
{
    return _Selected;
}

void GTAGenericFunction::setDisplayName(const QString &iName)
{
    _DisplayName = iName;

}

QString GTAGenericFunction::getDisplayName()const
{
    return _DisplayName;
}

void GTAGenericFunction::setArgsToUseList(const QStringList &iList)
{
    _ArgsToUseList = iList;
}

QStringList GTAGenericFunction::getArgsToUseList() const
{
    return _ArgsToUseList;
}

void GTAGenericFunction::setWaitUntilStatus(bool isWaitUntilTrue)
{
    _isWaitUntilTrue = isWaitUntilTrue;
}

bool GTAGenericFunction::getWaitUntilStatus()const
{
    return _isWaitUntilTrue;
}

QList<GTAGenericArgument> GTAGenericFunction::getAllArguments()const
{
    return _AllArguments;
}

void GTAGenericFunction::setAllArguments(const QList<GTAGenericArgument> &iArgList, bool isCallerSerializer)
{
    _AllArguments = iArgList;

    if(!isCallerSerializer)
    {
        setConstantValues(_AllArguments);
        setDefaultValues(_AllArguments);
    }
}

QList<GTAGenericArgument> GTAGenericFunction::getHiddenArguments()const
{
    QList<GTAGenericArgument> HiddenArguments;


    for(int i = 0; i < _AllArguments.count(); i++)
    {
        GTAGenericArgument argObj = _AllArguments.at(i);
        if(argObj.isDisplayed() == false)
            HiddenArguments.append(argObj);
    }
    return HiddenArguments;
}

void GTAGenericFunction::setIsReturnTypeSimple(const bool &iVal)
{
    _isReturnTypeSimple = iVal;
}

void GTAGenericFunction::ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QList<GTAGenericArgument> resolvedSelectedArguments;
    QList<GTAGenericArgument> resolvedAllArguments;

    QHash <QString, QString> resolvedSelectedArgumentsColor;
    QHash <QString, QString> resolvedSelectedArgumentsBackColor;

    for(int i=0;i<_Arguments.count();i++)
    {
        GTAGenericArgument arg = _Arguments.at(i);
        arg.ReplaceTag(iTagValueMap);
        resolvedSelectedArguments.append(arg);
        resolvedSelectedArgumentsColor.insert(arg.getArgumentName(), arg.getColor());
        resolvedSelectedArgumentsBackColor.insert(arg.getArgumentName(), arg.getBackColor());
    }
    _Arguments.clear();
    _Arguments.append(resolvedSelectedArguments);
    _colorOfArgs.clear();
    _backColorOfArgs.clear();
    _colorOfArgs = resolvedSelectedArgumentsColor;
    _backColorOfArgs = resolvedSelectedArgumentsBackColor;

    for(int i=0;i<_AllArguments.count();i++)
    {
        GTAGenericArgument arg = _AllArguments.at(i);
        arg.ReplaceTag(iTagValueMap);
        resolvedAllArguments.append(arg);
    }
    _AllArguments.clear();
    _AllArguments.append(resolvedAllArguments);
}
QStringList GTAGenericFunction::getVariableList() const
{
    QStringList items;
    for(int i=0;i<_AllArguments.count();i++)
    {
        items.append(_AllArguments.at(i).getVariableList());
    }
    for(int i=0;i<_Arguments.count();i++)
    {
        items.append(_Arguments.at(i).getVariableList());
    }
    items.removeDuplicates();
    return items;
}

bool GTAGenericFunction::isReturnTypeSimple()const
{
    return _isReturnTypeSimple;
}

bool GTAGenericFunction::operator ==(GTAGenericFunction obj2)
{

    QString name = obj2.getFunctionName();
    QString toolType = obj2.getToolType();
    QString toolId = obj2.getToolID();
    QString returnName = obj2.getReturnParamName();
    QString returnType = obj2.getReturnParamType();

    // if(_Name != name || _ToolType != toolType|| _ToolID != toolId || _ReturnParamName != returnName || _ReturnParamType != returnType)
    if(_Name == name && _ReturnParamName == returnName)
        return true;
    else
        return false;

}


//bool GTAGenericFunction::operator ==(GTAGenericFunction iRhs)const
//{
//    if(_Name == iRhs.getFunctionName())
//        return true;
//    else
//        return false;
//}

bool GTAGenericFunction::operator !=(GTAGenericFunction iRhs)const
{
    bool rc = false;
    if(_Name != iRhs.getFunctionName() || _ToolType != iRhs.getToolType()
            || _ToolID != iRhs.getToolID()
            || _ReturnParamName != iRhs.getReturnParamName()
            || _ReturnParamType != iRhs.getReturnParamType()
            || _DisplayName != iRhs.getDisplayName())
    {
        return true;
    }
    else
    {

        QList<GTAGenericArgument> rhsAllArgs = iRhs.getAllArguments();
        if(_AllArguments.count() != rhsAllArgs.count())
            return true;

        for(int i = 0; i < _AllArguments.count(); i++)
        {
            GTAGenericArgument arg = _AllArguments.at(i);
            GTAGenericArgument rhsArg = rhsAllArgs.at(i);
            if(arg != rhsArg)
            {
                return true;
            }
        }
    }

    return rc;
}

void GTAGenericFunction::setColorTableArg(const QString& iNameArg, const QString& iColor, const QString& iBackColor) {
    _colorOfArgs.insert(iNameArg, iColor);
    _backColorOfArgs.insert(iNameArg, iBackColor);
}

QString GTAGenericFunction::getColorArg(QString& iNameArg) const{
    return _colorOfArgs.value(iNameArg);
}

/**
 * @brief Function to control if there is an argument with foreground color.
 *
 * @return Qstring name of color or empty.
 */
QString GTAGenericFunction::getColor() const{
    for (const auto& arg : getFunctionArguments()) {
        if (!_colorOfArgs.value(arg.getArgumentName()).isEmpty()) {
            return _colorOfArgs.value(arg.getArgumentName());
        }
    }
    return "";
}

/**
 * @brief Function to control if there is an argument with background color.
 * 
 * @return Qstring name of color or empty. 
 */
QString GTAGenericFunction::getBackColor() const {
    for (const auto& arg : getFunctionArguments()) {
        if (!_backColorOfArgs.value(arg.getArgumentName()).isEmpty()) {
            return _backColorOfArgs.value(arg.getArgumentName());
        }
    }
    return "";
}