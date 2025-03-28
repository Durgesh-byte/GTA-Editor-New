#include "AINGTAGenestaActInvalidDeserializer.h"
#include "AINGTAInvalidCommand.h"

AINGTAGenestaActInvalidDeserializer::AINGTAGenestaActInvalidDeserializer(const QString& iStmt):AINGTAGenestaParserItf(iStmt)
{

    

}

bool AINGTAGenestaActInvalidDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
{
    bool rC = false;
    
    if (!_statement.isEmpty())
    {
        
        QString sCheckValParamStatement = _statement;
        //sCheckValParamStatement.remove(GENESTA_CMD_RELEASE);
        sCheckValParamStatement.trimmed();
        QStringList lstofStatements;
        getPreprocessedStatements(sCheckValParamStatement,lstofStatements);
        
        if (lstofStatements.isEmpty())
            {_lastError="could not process statement";return false;}
        
        AINGTAInvalidCommand* pInvldCmd = NULL;
        QStringList parameterList;
        foreach(QString statement,lstofStatements)
        {
            if (NULL==pInvldCmd)
                pInvldCmd = new AINGTAInvalidCommand;
            if (!statement.isEmpty())
            {
                replaceGenSymbols(statement);
                pInvldCmd->setMessage(statement);
                               

            }
        }

        if (pInvldCmd)
        {
                      
            oLstCmds.append(pInvldCmd);
            rC=true;
            
        }
        

    }
    else
        _lastError="Nothing to evaluate with empty statement";


    return rC;
}

