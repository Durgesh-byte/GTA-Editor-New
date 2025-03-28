#include "AINGTAGenestaChkBITEMsgDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTACheckBiteMessage.h"
AINGTAGenestaChkBITEMsgDeserializer::AINGTAGenestaChkBITEMsgDeserializer(const QString& iStmt):AINGTAGenestaParserItf(iStmt)
{

}

bool AINGTAGenestaChkBITEMsgDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
{ 
    bool rC = false;
    //AINGTACommandBase*  pCmd = NULL;
    if (!_statement.isEmpty())
    {
        /* Example:
            check BITE Message “000000ENG1A-0159-RTT VLV TEMP SNSR ELEC FAIL” is NOTdisplayed on Previous leg report::2000::continue:
            check BITE Message “000000ENG1A-0129-PMA SIG FAIL” is displayed on Post Flight Report::2000::continue:
          */
        QString sFWCWarningStatement = _statement.remove(GENESTA_CMD_CHECK_BITE_MESSAGE);
        int dQuoteStart = sFWCWarningStatement.indexOf("“");
        int dQuoteEnd   = sFWCWarningStatement.indexOf("”",dQuoteStart);

        QString sMessage = QString();
        int lengthOfChars = dQuoteEnd-dQuoteStart-1;
        if(lengthOfChars >=0)
            sMessage = sFWCWarningStatement.mid(dQuoteStart+1,lengthOfChars);
        QString sDisplayStatus;

        if (sFWCWarningStatement.contains("is NOTdisplayed"))
            sDisplayStatus = "NOT";//todo: update in utils, also use same in AINGTACheckFWCWarningWidget.ui

        if (!sMessage.isEmpty())
        {
            AINGTACheckBiteMessage* pBiteMsg = new AINGTACheckBiteMessage;
            pBiteMsg->setMessage(sMessage);
            pBiteMsg->setCondition(sDisplayStatus);

            //default values set as in GTA
            pBiteMsg->setWaitStatus(true);
            pBiteMsg->setBuffDepth("0");
            QString reportOn(CHECK_BITE_MESSAGE_TYPE);
            QStringList reportOnList = reportOn.split(";");
            if (!reportOnList.isEmpty())
                pBiteMsg->setOnReport(reportOnList.at(0));
            oLstCmds.append(pBiteMsg);
            rC=true;
        }
        else
            _lastError="Message for check BITE Message could not be evaluated";
    }
    else
        _lastError="Nothing to evaluate with empty statement";

    return rC;
}



