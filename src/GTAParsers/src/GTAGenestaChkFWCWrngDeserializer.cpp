#include "AINGTAGenestaChkFWCWrngDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTACheckFwcWarning.h"
AINGTAGenestaChkFWCWrngDeserializer::AINGTAGenestaChkFWCWrngDeserializer(const QString& iStmt):AINGTAGenestaParserItf(iStmt)
{

}


bool AINGTAGenestaChkFWCWrngDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
{
    AINGTACommandBase*  pCmd = NULL;
    bool rC = false;
    if (!_statement.isEmpty())
    {
    /* Example:
        1- check FWC Warning “ENG 1 FUEL VALVE FAULT - FUEL VALVE NOT CLOSED” is displayed on E/WD
        2- check FWC Warning “ENG 1 FUEL HEAT SYS - FUEL RETURN VALVE OPEN” is NOT displayed on E/WD
    */

        QString sFWCWarningStatement = _statement.remove(GENESTA_CMD_CHECK_FWC_WARNING);
        int dQuoteStart = sFWCWarningStatement.indexOf("“");
        int dQuoteEnd   = sFWCWarningStatement.indexOf("”",dQuoteStart);

        QString sMessage = QString();
        int lengthOfChars = dQuoteEnd-dQuoteStart-1;
        if(lengthOfChars >=0)
            sMessage = sFWCWarningStatement.mid(dQuoteStart+1,lengthOfChars);

        QString sDisplayStatus;

        if (sFWCWarningStatement.contains("is NOT displayed"))
            sDisplayStatus = "NOT";//todo: update in utils, also use same in AINGTACheckFWCWarningWidget.ui

        if (!sMessage.isEmpty())
        {
            AINGTACheckFwcWarning* pWarng = new AINGTACheckFwcWarning;
            pWarng->setMessage(sMessage);
            pWarng->setCondition(sDisplayStatus);
            pWarng->setWaitForBuffer("0");
            //default values set as in GTA
            pWarng->setWaitAck(true);
	//        pWarng->setWaitForBuffer(0);
            pWarng->setHeader(QString());
            pCmd=pWarng;
            oLstCmds.append(pWarng);
            rC=true;
        }
        else
            _lastError="Message for FWC Warning could not be evaluated";
    }
    else
        _lastError="Nothing to evaluate with empty statement";

    return rC;
}



