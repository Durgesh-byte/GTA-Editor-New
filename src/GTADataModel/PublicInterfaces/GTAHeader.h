#ifndef GTAHEADER_H
#define GTAHEADER_H
#include "GTADataModel.h"

#pragma warning(push, 0)
#include <QHash>
#include <QString>
#include <QtXml/QDomDocument>
#include <QDateTime>
#include <QStringList>
#include <QObject>
#pragma warning(pop)

#define AIRCRAFT_TYPE "Aircraft"
#define EQUIPMENT_NAME "Equipment"
#define SUPPLIER_NAME "Supplier"
#define HEADER_OBJECT_NAME			"Object name"
#define HEADER_OBJECT_NAME_ALIAS	"Name(Proc, Object,Seq,Func²*^)"
#define	HEADER_CREATION_DATE		"Creation date"
#define	HEADER_TEST_DESIGNER		"Test designer"
#define	HEADER_KEY_WORDS			"Keywords"
#define	HEADER_TEST_DESCRIPTION	"Test description"
#define	HEADER_AIRCRAFT			"Aircraft"
#define	HEADER_EQUIPMENT			"Equipment"
#define	HEADER_EQP_SUPPLIER		"Equipment's supplier"
#define	HEADER_TEST_FACILITY		"Test facility"
#define	HEADER_VALUE_SEPERATOR		":"
#define	HEADER_STANDARD		"Standard"
#define HEADER_VALUE_EMPTY  "Empty"
#define HEADER_NAME			"Header Name"
//"²"
//"*"
//"^"



#define VALUE_POS_ATTR "ValuePos"
#define HEADER_ROOT_TAG "Header"
#define HEADER_ELEM_TAG "HeaderTag"
#define NAME_ATTR "Name"
#define VAL_ATTR "Value"
#define NAME_POS_ATTR "NamePos"



#define NAME_ATTRIBUTE "NAME"
#define VAL_ATTRIBUTE "VALUE"
#define ID_ATTRIBUTE "ID"
#define HEADER_TAG "HEADER"
#define MANDATORY_ATTR "MANDATORY"

#define HEADER_TAG "HEADER"
#define HEADERDETAILS_TAG "HEADERDETAILS"
#define DESIGNER_TAG "DESIGNER"
#define KEYWORDS_TAG "KEYWORDS"
#define DESCRIPTION_TAG "DESCRIPTION"
#define CONFIG_TAG "CONFIG"
#define SUPPLIER_TAG "SUPPLIER"
#define STANDARDS_TAG "STANDARDS"
#define STANDARDGRP_TAG "STANDARDGRP"
#define STANDARD_TAG "STANDARD"
#define TEST_TAG "TEST"

#define SUPERSCRIPT1 "²"
#define SUPERSCRIPT2 "*"
#define SUPERSCRIPT3 "^"
#define SUPERSCRIPT4 "°"

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
struct GTADataModel_SHARED_EXPORT GTAFILEInfo
{
    QString _filename;
    QString _filePath;
    QString _fileType;
    QString _fileDescription;
    QString _Mandatory;
    QStringList _Application;
    QString _Equipment;

};


class GTADataModel_SHARED_EXPORT GTAHeader//:public GTABaseModel
{

private:

    struct GTAheaderNode
    {
    public:
        QString _Name;
        bool _isMandatory;
        QString _Value;
        QString _Description;
        bool _showInLTRA;
    };



    long _ProgramConfigID;

    QString _Name;
    QString _DesignerName;
    QString _Description;
    QString _EquipmentSupplier;
    QString _CreationDate;
    QString _KeyWords;
    QString _Program;
    QString _Equipment;
    QString _Standard;

    //QList <QString> _lstheaderTags;


    QString _ElemName;
    QDomDocument* _pHeaderDom;

    QList<GTAheaderNode> _lstOfFields;
    QDomNode _rootNode;
    int _maxID;
    int _maxChildID;

    QHash<QString , QString> _hshOtherData;

    bool _editMode;
    QString _LastError;
    QList<int> _mandatoryFieldUnfilled;
	QString _GTAVersion;


public:
    explicit GTAHeader(QDomDocument* ipDomDoc, QObject* iParent=0);
    explicit GTAHeader();
    explicit GTAHeader(const GTAHeader &iRhs);
    ~GTAHeader();

    void setName(const QString & iName);
    //void setDesignerName(const QString & iDesignerName);
    void setDescription(const QString & iDescription);
    /*   void setEquipmentSupplier(const QString & iEquipmentSupplier);
 void setCreationDate(const QString & iDate);
    void setKeyWords(const QString & iKeywords);
    void setProgram(const QString& iProgram);
    void setEquipment(const QString& iEquipment);
    void setStandard(const QString& iStandard);*/


    QString getName()const{return  _Name;}
    ///QString getDesignerName(void){return _DesignerName ;}
    QString getDescription()const{return _Description ;}
    /* QString getEquipmentSupplier(void){return _EquipmentSupplier ;}
 QString getCreationDate(void){return _CreationDate ;}
 QString getKeyWords(void){return _KeyWords ;}
 QString getPogram(void) {return _Program;}
 QString getEquipment(void) {return _Equipment;}
 QString getStandard(void) {return _Standard;}*/




    void setElemName(const QString &iElemName){_ElemName = iElemName;}
    QString getElemName() const {return _ElemName;}
    QString getValue(const int& dParentId, const int& dChildId,bool& ioEditSuccess) const;
    QString getTagName(const int& dId);
    bool setValue(const int& dParentId, const int& dChildId,const QString& isValue);

    void setMaxID(const int& isID);
    void setMaxChildrenID(const int& isID);
    int getMaxID()const;
    int getMaxChildID()const;


    QDomDocument* getHeaderDom()const {return _pHeaderDom;}
    void dumpDomNOde();
    bool mandatoryAttributeFilled(QStringList& oEmptyManField);
    bool inEditMode()const;
    void setEditMode(bool val){_editMode=val;}

    bool addHeaderItem(const QString& iFieldName, bool isMandatory,QString fieldValue = QString(),QString iDescription =QString(), bool showInLTRA = true /*,QHash<QString, QString>& otherData =QHash<QString , QString>()*/ );
    bool isNodeMandatory(const QString& oFieldName) const;
    bool isDefaultMandatoryField(QString strFieldName) const;
    int  getIndex(const QString& iFiledName)const ;
    bool getDomElement(QDomDocument iContextDoc, QDomElement& oHdrRootNode)const;
    bool editField(const QString& FieldName,const QString& FieldValue);
    bool UpdateShowLTRAFlag(const QString& FieldName, const bool& value);
    int itemSize()const;
    QString getFieldName(const int& iOrder);
    QString getFieldValue(const int& iOrder);
    QString getFieldDescription(const int& iOrder);
    bool getShowInLTRA(const int& iOrder);

    QString getLastError()const{return _LastError;}
    void setLastError(const QString &iError){_LastError = iError;}
    int getFieldNodeSize(){return _lstOfFields.size();}
    bool editHeaderItem(const int& iRow,const QString& iFieldName, bool isMandatory,const QString& iFieldValue,const QString& iDescription, bool showInLTRA);
    bool removeRows(const int& rows, const int& noOfRows);
    bool insertEmptyNodes(const int& row, const int& noOfRows);
    QList<int> getUnfilledMandatoryFieldRows()const{return _mandatoryFieldUnfilled;}
    void setUnfilledMandatoryFieldRows(const  QList<int> &iList){_mandatoryFieldUnfilled = iList;}

    void insertDefaultHeaderNodes();
    QList<GTAheaderNode> getLstOfFields()const{return _lstOfFields;}
    void setLstOfFields(const QList<GTAheaderNode> &iList);
	
	/**
	* This function sets the version of GTA provided by user. Added for Requirement #357031 (Marine)
	* @iVersion: the version to be updated in QString format
	*/
	void setGTAVersion(const QString & iVersion, bool iOverwriteExisting = false);
	/**
	* This function returns the version of GTA available with the header. 
	* This entirely depends on what was set by the user or the function returns a blank QString
	* @return: the version to be updated in QString format
	*/
	QString getGTAVersion()const { return  _GTAVersion; }

};

#endif //GTAHEADER_H

/*
GTAHeader
GTAHEADER
*/
