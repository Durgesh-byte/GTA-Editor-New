#include "AINGTAGenestaActManualDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTAActionManual.h"
AINGTAGenestaActManualDeserializer::AINGTAGenestaActManualDeserializer(const QString& iStmt):AINGTAGenestaParserItf(iStmt)
{

}

bool AINGTAGenestaActManualDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
{
    bool rC = false;
   
    /*int xxxx=_statement.indexOf(QRegExp("“*”"));*/
    if (!_statement.isEmpty())
    {
        //manual action - this is message manual action to be done

        QString sMessage;
        AINGTAActionManual* pManAction = new AINGTAActionManual;
        pManAction->setAction(ACT_MANUAL);


       
        if (_statement.contains(GENESTA_CMD_CHECK_FWC_PROC))
        {
            _statement.remove(GENESTA_CMD_CHECK_FWC_PROC);
            sMessage= (_statement);
           
          
        }
        else if (_statement.contains(GENESTA_CMD_CHECK_FWC_DISP))
        {
            _statement.remove(GENESTA_CMD_CHECK_FWC_DISP);
            sMessage= (_statement);
           
                
        }
        else if (_statement.contains(GENESTA_CMD_CHECK_ECAM_DISP))
        {
            _statement.remove(GENESTA_CMD_CHECK_ECAM_DISP);
            sMessage= (_statement);
           
            
        }
        else if (_statement.contains(GENESTA_CMD_CHECK_VIZ_PROC))
        {
            _statement.remove(GENESTA_CMD_CHECK_VIZ_PROC);
            sMessage= (_statement);
           
           
        }
        else if (_statement.contains(GENESTA_CMD_CHECK_MANUAL))
        {
            _statement.remove(GENESTA_CMD_CHECK_MANUAL);
            sMessage= (_statement);
            
        }
        else if (_statement.contains(GENESTA_CMD_MANUAL_ACT))
        {
            
            int sepStartIndex =_statement.indexOf(GENESTA_ACT_MSG_SEP);
            sMessage= _statement.mid(sepStartIndex+1);
                     
            
        }
        else
        {
            delete pManAction;
            pManAction=NULL;
            _lastError="Command beloging to manual cmd group could not be evaluated";
        }

        if (pManAction)
        {
            pManAction->setAcknowledgement(true);
            sMessage.remove("“");
            sMessage.remove("”");
            pManAction->setMessage(sMessage);
            oLstCmds.append(pManAction);
            rC=true;
        }

    }
    else
         _lastError="Nothing to evaluate with empty statement";

    return rC;
}



 
 

 
   
