#ifndef GTAICDPARAMETER_H
#define GTAICDPARAMETER_H
#include "GTADataModelInterface.h"

#pragma warning(push, 0)
#include <QString>
#include <QObject>
#include <QHash>
#pragma warning(pop)

#define BOOLEAN_TYPE  "Boolean"
#define FLOAT_TYPE  "Float"
#define INTERGER_TYPE  "Integer"
#define ENUMERATE_TYPE "Enumerate"
//#define STRING_TYPE  "String"

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
struct GTADataModel_SHARED_EXPORT GTAICDParameterValues
{
    QString key;
    QString value;

    bool operator ==(const GTAICDParameterValues & iVal)
    {
		if(value.toLower() == iVal.value.toLower())
            return true;
        else
            return false;
    }
};


class GTADataModel_SHARED_EXPORT GTAICDParameter
{	
public:
    GTAICDParameter();

    void setName(const QString & iVal);
    QString getName() const;

    void setUpperName(const QString& iVal);
    QString getUpperName() const;

    void setBit(const QString & iVal);
    QString getBit() const;

    void setIdentifier(const QString & iVal);
    QString getIdentifier() const;

    void setLabel(const QString & iVal);
    QString getLabel() const;

    void setMaxValue(const QString & iVal);
    QString getMaxValue() const;

    void setMinValue(const QString & iVal);
    QString getMinValue() const;

    void setSignalType(const QString & iVal);
    QString getSignalType() const;

    void setSourceFile(const QString & iVal);
    QString getSourceFile() const;

    void setSourceType(const QString & iVal);
    QString getSourceType() const;

    void setParameterType(const QString& iVal);
    QString getParameterType() const;

    void setUnit(const QString & iVal);
    QString getUnit() const;

    void setValueType(const QString & iVal);
    QString getValueType() const;

    void setEquipmentName(const QString &iVale);
    QString getEquipmentName() const;

    void setApplicationName(const QString & iValue);
    QString getApplicationName() const;
    //QString getSourceFileLastModifiedDate() const;

    void setBusName(const QString & iValue);
    QString getBusName() const;

	/*inline */void setSignalName(const QString & iValue);
	/*inline */QString getSignalName() const;

    inline void setMedia(const QString & iMedia){_Media = iMedia;}
    inline QString getMedia() const{
        if (_ParamType == "FIB-IPR") return "PIR";
        else return _Media;
    }

    inline void setMessageName(const QString & iMessage){_Message = iMessage;}
    inline QString getMessageName() const{return _Message;}

    inline void setParameterLocalName(const QString & iPln){_Pln = iPln;}
    inline QString getParameterLocalName() const{return _Pln;}

    bool operator ==(const QString & iVal);
    bool operator ==(const GTAICDParameter & iParam);

    inline void setFSName( const QString &iFsName){_FsName = iFsName;}
    inline QString getFSName( ) const {return _FsName;}

    inline void setSuffix(const QString &iSuffix){_Suffix = iSuffix;}
    inline QString getSuffix()const {return _Suffix;}

    inline void setPrecision(const QString &iPrecision){_Precision = iPrecision;}
    inline QString getPrecision()const {return _Precision;}

    inline void setDirection(const QString &iDirection){_Direction = iDirection;}
    inline QString getDirection()const {return _Direction;}

    inline void setTempParamName(const QString &iParm) {_TempParamName = iParm;}
    inline QString getTempParamName()const {return _TempParamName;}

    inline void setLastModifiedDate(const QString &iParam){_SourceFileLastModifiedDate = iParam;}
    inline QString getLastModifiedDate()const {return _SourceFileLastModifiedDate;}

    inline void setActorName(const QString &iActorName){_Actor = iActorName;}
    inline QString getActorName()const {return _Actor;}

    inline void setMediaType(const QString &iMediaType){_MediaType = iMediaType;}
    inline QString getMediaType()const {return _MediaType;}

    inline void setToolName(const QString &iToolName){_ToolName = iToolName;}
    inline QString getToolName()const {return _ToolName;}

    inline void setToolType(const QString &iToolType){_ToolType = iToolType;}
    inline QString getToolType()const {return _ToolType;}

    inline void setRefreshRate(const QString &iRefreshRate){_RefreshRate = iRefreshRate;}
    inline QString getRefreshRate()const {return _RefreshRate;}

    inline void setValue(const QString &iValue){_Value = iValue;}
    inline QString getValue()const
    {
        if((_Value.trimmed().isEmpty()) && (_ToolName != "NA"))
        {
            return QString("{'Type':%1,'Media':%2}").arg(getValueType(), getMedia());
        }
        return _Value;
    }

    QList<GTAICDParameterValues> getPossibleValueList();
    QString getPossibleValues();
    void setPossibleValues(QList<GTAICDParameterValues> &iPossibleValues);
    void setPossibleValues(const QString &iValues);

    void setUUID(const QString &iUUID);
    QString getUUID() const;
    QStringList getAsList();

    //function added just for gta live view
    void setSSMValue(const QString &iSSM) { _SSM = iSSM; }
    QString getSSMValue() const { return _SSM; }

	bool isOnlyValueDueToSignalType(QString & iSignalType);
	QString getMediaFromSignalType(const QString& iSignalType);

    void setColor(const QString& iColor){ _Color = iColor; }
    QString getColor() const { return _Color; }

    void setBackColor(const QString& iBackColor) { _BackColor = iBackColor; }
    QString getBackColor() const { return _BackColor; }

private:
    QString _ParamName;
    QString _ParamUpperName;
    QString _Bit;
    QString _Identifier;
    QString _Label;
    QString _MaxValue;
    QString _MinValue;
    QString _SignalType;
    QString _SourceType;
    QString _ParamType;
    QString _Unit;
    QString _ValueType;
    QString _SourceFile;
    QString _EquipmentName;
    QString _SourceFileLastModifiedDate;
    QString _ApplicationName;
    QString _BusName;
    QString _SignalName;
    QString _Media;
    QString _Message;
    QString _Pln;
    QString _FsName;
    QString _Suffix;
    QString _Precision;
    QString _TempParamName;
    QString _Direction;
    QString _Actor;
    QString _MediaType;
    QString _ToolName;
    QString _ToolType;
    QString _RefreshRate;
    QString _Value;
    QString _UUID;
    QString _SSM;
    QList<GTAICDParameterValues> _PossibleValues;
    QString _Color;
    QString _BackColor;

};


#endif // GTAICDPARAMETER_H
