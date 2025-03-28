#ifndef GTADOCUMENTCONTROLLER_H
#define GTADOCUMENTCONTROLLER_H
#include "GTAControllers.h"
#include "GTACommandBase.h"
#include "GTAActionBase.h"
#include "GTAActionManual.h"
#include "GTAElement.h"
#include "GTAInitConfigBase.h"
#include "GTAInitConfiguration.h"
#include "GTAHeader.h"
#include "GTAActionCondition.h"
#include "GTAGenericFunction.h"
#include "GTAGenericToolCommand.h"

#pragma warning(push, 0)
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QList>
#pragma warning(pop)

//TODO: This is a warning against assigning Rvalues to references, which is done in default arguments for many methods in this file,
//      This will be fixed in the future, as this refactor will take some time (there are a lot od default arguments which will be broken by removing default argument from some of the refs
#pragma warning (disable : 4239)

class GTAControllers_SHARED_EXPORT GTADocumentController
{
public:
    GTADocumentController();

    bool saveDocument(const GTAElement * ipElement, const QString& sHeaderName,const QString & iDocFilePath , bool iOverwrite = false);

    bool saveDocument(const GTAHeader * ipHeader, const QString & iDocFilePath, bool iOverwrite = false);

    bool createCommands(const QList<GTACommandBase*> &iCmdList,QDomDocument & iDomDoc,QDomElement & iParentElem, int & GlobalPosition);

    bool createCommand(const GTACommandBase * pCmd , QDomDocument & iDomDoc, QDomElement & oElement, int & iGlobalPosition);

    bool createAction(const GTAActionBase * pActionCmd , QDomDocument & iDomDoc , QDomElement & oElement, int & iGlobalPosition);
    bool createCheck(const GTACheckBase * pChkCmd , QDomDocument & iDomDoc , QDomElement & oElement, int & iGlobalPosition);

    /**
      * This function builds the tag variable DomElement
      * @iDomDoc: root Domdocument
      * @iParentElem: tag variable domelement
      * @ipElement : GTAElement to read tagvariablelist
      */
    bool createTagVariables(QDomDocument & iDomDoc,QDomElement & iParentElem, GTAElement *& ipElement);

    //    bool getCommand(const GTACommandBase * pCmd , QDomDocument & iDomDoc , int & iOrder, QDomElement & oElement);
    //    /**
    //      * This function return true if xml node is successfully created by provide command
    //      * otherwise return false
    //      * for futher detail on error use getLastError() methode immediate after this method excution
    //      */
    //    bool getAction(const GTAActionBase * pActionCmd , QDomDocument & iDomDoc , int & iOrder, QDomElement & oElement);


    /**
      * This function return the last error occured during any of method of this class
      */
    inline QString getLastError() const {return _LastError;}

    /**
      * This function build GTAElement object from the document saved through editor
      * return false if fail to load, otherwise true
      */
    bool openDocument(const QString & iDocFilePath, GTAElement *& opElement , GTAHeader*& opHeader,bool ioOnlyModel=true);
    bool isDocumentValid(const QString & iDocFilePath);

    bool loadCommand(const QDomElement & iCmdNode, GTAElement *& iopElement);

    bool loadAction(const QDomElement & iActionElem, GTAElement *& iopElement, int & ioPosition,GTACommandBase *& ipParent, int& ioOrderChange);
    bool loadCheck(const QDomElement &iActionElem, GTAElement *&iopElement, int &ioPosition, GTACommandBase *&ipParent,int& ioOrderChange);
    void SyncReturnInExternalFunction(GTAGenericToolCommand *& pGenToolCmd);

    bool BuildElement(const QDomNodeList & iCmdNodeList, GTAElement *& iopElement, GTACommandBase *& pParent,int& ioOrderChange);

    bool checkForMismatch(const QString &iToolName, const QList<GTAGenericFunction> &iFunctionList);
    /**
      *This function gets document info.
      * @iDocFileFullPath: Takes input of document full path
	  * @oUUID: Get UUID of the input file path
      * @oCreatedDate : Takes created date of the document
	  * @oModifiedDate: Get last modified time from the document
      * @oAuthName: Get author name from the document
	  * @oValidatorName: Get validator name from the document
	  * @oValidationStatus: Get validation status from the document
	  * @oValidationTime: Get validation time from the document
	  * @oCreatedDate: Get file creation time from the document
      * @oGtaVersion : GTA creation version of the document
      * @oMaxTimeEstimated : Maximum Time Estimated of the document
      * @oInputCsv : Input Csv File of the document (if applied)
      * @oOutputCsv : Output Csv File of the document (if applied)
    */
    void getDocumentInfo(const QString & iDocFileFullPath, 
        QString &oUUID,
        QString &oModifiedDate = QString(),
        QString &oAuthName = QString(),
        QString &oValidatorName = QString(),
        QString &oValidationStatus = QString(),
        QString &oValidationTime = QString(),
        QString &oCreatedDate = QString(),
        QString &oGtaVersion = QString(),
        QString &oMaxTimeEstimated = QString(),
        QString &oInputCsv = QString(),
        QString &oOutputCsv = QString());
    QString getDocumentName(const QString & iDocFileFullPath);
    void setDocumentPath(const QString & iDocFileFullPath, const QString &iRelFilePath);
    void setDocumentName(const QString & iDocFileFullPath,const QString &iRelativeFilePath,const QString &iName);
    bool setDocumentUUID(const QString & iAbsoluteFilePath,const QString & iUUID);
    QString getDescriptionFromHeader(const QString & iDocFileFullPath);
    void setValidationInfo(const QString& docFileFullPath, const QString& validationStatus, const QString& validatorName, const QString& validationTime) const;
    void getDescAndInfo(const QString & iDocFileFullPath,
        QString &oUUID,
        QString &oModifiedDate = QString(),
        QString &oAuthName = QString(),
        QString &oValidatorName = QString(),
        QString &oValidationStatus = QString(),
        QString &oValidationTime = QString(),
        QString &oCreatedDate = QString(),
        QString &oDescription = QString(),
        QString &oGtaVersion = QString(),
        QString& oMaxTimeEstimated = QString(),
        QString& oInputCsv = QString(),
        QString& oOutputCsv = QString());

    bool getHeaderFromDoc(const QString& iDocPath, GTAHeader*& ioHeader);

    bool getFileInfoFromDoc(const QString& iDocFilePath, QList<GTAFILEInfo>& ioList );

    /**
    * This function return the list of variables used in all commands of the element, including reference of other document
    * @pElem: Element for which the variables to be retrieved
    */
    QStringList getAllVariableList(const GTAElement * pElem, QStringList& ioreferedItems,bool listReleaseVar = false) const;

    bool setDocReadOnlyState(const QString & iDocFilePath, bool bReadOnlyState);


private:
    QString _LastError;

    int getGenListIndex(QList<GTAGenericFunction> &functions, GTAGenericFunction &cmdFunc);

    /**
      * This function return true on successfully creation of xml root node
      * otherwise return false;
      * getLastError() method return the last error captured on fail
      */
    bool getRootElement(GTAElement *& ipElement,QDomDocument & iDomDoc,QDomElement & oElement);

    /**
      * This function return true on successfully fill the Element object from xml element
      * otherwise return false;
      * getLastError() method return the last error captured on fail
      */
    bool getRootElement(const QDomElement & iDomElem, GTAElement *& opElement);

    /**
      * This function return true on successfully creation of program configuration node in xml
      * otherwise return false;
      * getLastError() method return the last error captured on fail
      */
    bool getProgramConfig(GTAElement *& ipElement, QDomDocument & iDomDoc, QDomElement & oElement);

    /**
      * This function return true on successfully fill the Element object for program configuration
      * otherwise return false;
      * getLastError() method return the last error captured on fail
      */
    bool getProgramConfig(const QDomElement & iDomElem, GTAElement *& opElement);



    /**
      * This function return true on successfully fill the Element object for header object
      * otherwise return false;
      * getLastError() method return the last error captured on fail
      */
    //bool getHeader(const QDomElement & iDomElem, GTAElement *& opElement);

    bool getHeader(const QDomDocument & iDomDoc, GTAHeader *& ioHeader);

    /**
      * This function retrieves list of info from Dom document
      *
      */
    bool getFileInfoFromDoc(const QDomDocument & iDomDoc, QList<GTAFILEInfo>& ioList );

public:
    /**
      * This function return true on successfully creation of header node in xml
      * otherwise return false;
      * getLastError() method return the last error captured on fail
      */
    bool getHeaderDom(GTAElement *& ipElement, QDomDocument & iDomDoc,QDomElement & oElement);
    /**
      * This function creates and adds element dom for an element.
      * @iRefDoc, Dom document context
      * @ipElement, Element for which element dom has to be populated
      * @oAggregatingElem, element dom where its children are added with file info node.
      */
    bool getFileInfoDom(QDomDocument& iRefDoc, GTAElement* ipElement, QDomElement& oAggregatingElem);


};
#pragma warning (default : 4239)
#endif // GTADOCUMENTCONTROLLER_H
