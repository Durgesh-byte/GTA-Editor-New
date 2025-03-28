#ifndef GTAICDPARSER_H
#define GTAICDPARSER_H

#include "GTAParsers.h"

#pragma warning(push, 0)
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QList>
#include "GTAICDParameter.h"
#include <QDomNode>
#include <QFutureWatcher>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTAParsers_SHARED_EXPORT GTAICDParser//: public QObject
{
    //Q_OBJECT
public:
    enum ParseFlag{A429OUT,A429IN, ANAOUT,ANAIN,DESCOUT,DESCIN,CANOUT,CANIN,AFDXOUT,AFDXIN,PMRF,MODELF,VCOMSTATIC,PIRF};

    GTAICDParser(const QString& iICDFilePath/*, QObject * parent =0*/);
    ~GTAICDParser();

    void setDefaultParseFlag();
    void setParseFlag(const QHash<ParseFlag,bool> & iFlags);

    bool parse();
    QString getLastError() const;
    QString getEquipmentName();
    bool getParameterList(QList<GTAICDParameter *> & oList) const;

public slots:
    void onParseFinish();

private:

    QString _ErrMessage;
    QList<GTAICDParameter *> _ICDParamList;
    QString _sFilePath;
    QString _sEquipment;

    QHash<ParseFlag,bool> _ParsingFalgeData;

    /**
      * This function parse the ICD file type of csv
      */
    bool parseICD();
    /**
       This function parse PMR or ICDL type of xml file
       */
//    bool parsePMR();
//    /**
//       This function return the ICD parameter from "Variable" node in xml which has root element "PMR"
//       */

//    GTAICDParameter * getParameterFromPMR(const QDomNode & iVarNode);
//    /**
//       This function return the ICD parameter from "Parameter" node in xml which has root element "ICDL"
//       */

    /**
      * This function parse the PMR xml file
      * This method is fast to read the pmr file using QXmlStreamReader approach
      */
    bool parsePmrXmlFile();

    GTAICDParameter * getParameterFromICDL(const QDomNode & iParamNode);

    /**
       * This function return the parameter read from ICD data
       */
    GTAICDParameter * getParameterFromICD(const QString & iICDDataLine);
    bool parsePIR();

    bool isValidICDFile();
    bool isValidPIRFile();
private slots:

    void onParseFinish(QFutureWatcher<GTAICDParameter*> *& pWatcher);
	static void clearParameterList(QList<GTAICDParameter*> & iList);


};



#endif // GTAICDPARSER_H
