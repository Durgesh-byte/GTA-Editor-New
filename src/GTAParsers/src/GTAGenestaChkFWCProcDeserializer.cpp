#include "AINGTAGenestaChkFWCProcDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTACheckFwcWarning.h"
AINGTAGenestaChkFWCProcDeserializer::AINGTAGenestaChkFWCProcDeserializer(const QString& iStmt):AINGTAGenestaParserItf(iStmt)
{

}

bool AINGTAGenestaChkFWCProcDeserializer:: getCommands(QList<AINGTACommandBase*>& oLstCmds)
{
    bool rC = false;
    if (!_statement.isEmpty())
    {
        // _lastError="De serialization under construction";


        if (_statement.contains(GENESTA_CMD_CHECK_FWC_PROC))
        {
            _statement.remove(GENESTA_CMD_CHECK_FWC_PROC);
            QString isNotDisplayedOnEWD = "is NOT displayed on E/WD";
            QString isDisplayedOnEWD = "is  displayed on E/WD";

            QString newStatement = _statement;
            QString  isDisplayed;
            if(_statement.contains(isNotDisplayedOnEWD))
            {
                newStatement.remove(isNotDisplayedOnEWD);
                isDisplayed = "NOT";

            }
            else if(_statement.contains(isDisplayedOnEWD))
            {
                newStatement.remove(isDisplayedOnEWD);
                isDisplayed = "";
            }
            else
            {

                _lastError="Message for FWC Warning could not be evaluated";
            }


            QStringList lstStatements = newStatement.remove("“").split("”",QString::SkipEmptyParts);
            for(int i = 0; i < lstStatements.count(); i++)
            {
                QString sMessage = lstStatements.at(i);
                if (!sMessage.trimmed().isEmpty())
                {
                    AINGTACheckFwcWarning* pWarng = new AINGTACheckFwcWarning;
                    pWarng->setMessage(sMessage.trimmed());
                    pWarng->setCondition(isDisplayed);
                    pWarng->setWaitForBuffer("0");
                    //default values set as in GTA
                    pWarng->setWaitAck(true);
                    //        pWarng->setWaitForBuffer(0);
                    pWarng->setHeader(QString());
                    oLstCmds.append(pWarng);
                    rC=true;
                }
                else
                    _lastError="Message for FWC Warning could not be evaluated";
            }

        }

    }
    else
        _lastError="Nothing to evaluate with empty statement";


    return rC;
}



