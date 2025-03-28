using System;
using System.Collections.Generic;
using System.Text;

namespace GTATransformToDocx
{
    public enum CommandType { ACTION, CHECK, CALL, TITLE };
    public enum SubCommandType { TITLE, CONDITION, PRINT, NA};

    public struct Param_Detail
    {
        public String name;
        public String value;
    }

    public struct Other_Details
    {
        public List<Param_Detail> paramDetails;
    }

    public struct execTimeStamp
    {
        public String date;
        public String time;
        public String epoch;
    }

    public struct Command
    {
        public String name;
        public bool isIgnored;
        public CommandType cmdType;
        public SubCommandType subCmdType;
        public String result;
        public String Statememt;
        public execTimeStamp execTime;
        public List<Other_Details> details;
        public String comment;
        public String feedback;
        public String references;
        public String linenumber;


        public String precision;
        public String precisionUnit;

        public String timeout;
        public String timeoutUnit;

        public String actionOnFail;
        public String dumpList;

               
    }

    class GTAReportCommands
    {
        List<Command> lstCommands;
        public List<Command> Commands
        {
            get { return lstCommands; }
            set { lstCommands = value; }
        }

        String strResult;
        public String Result
        {
            get { return strResult; }
            set { strResult = value; }
        }

        String strStatememt;
        public String Statememt
        {
            get { return strStatememt; }
            set { strStatememt = value; }
        }

        execTimeStamp strExecTime;
        public execTimeStamp ExecTime
        {
            get { return strExecTime; }
            set { strExecTime = value; }
        }

        bool isIgnored;
        public bool Ignored
        {
            get { return isIgnored; }
            set { isIgnored = value; }
        }

        String feedback;
        public String userFeedback
        {
            get { return feedback; }
            set { feedback = value; }
        }

        String references;
        public String cmdReferences
        {
            get { return references; }
            set { references = value; }
        }

        String comment;
        public String cmdComment
        {
            get { return comment; }
            set { comment = value; }
        }

        String linenumber;
        public String cmdLineNumber
        {
            get { return linenumber; }
            set { linenumber = value; }
        }


        String precision;
        public String cmdPrecision
        {
            get { return precision; }
            set { precision = value; }
        }

        String precisionUnit;
        public String cmdPrecisionUnit
        {
            get { return precisionUnit; }
            set { precisionUnit = value; }
        }

        String timeout;
        public String cmdTimeout
        {
            get { return timeout; }
            set { timeout = value; }
        }

        String timeoutUnit;
        public String cmdTimeoutUnit
        {
            get { return timeoutUnit; }
            set { timeoutUnit = value; }
        }


        String actionOnFail;
        public String cmdActionOnFail
        {
            get { return actionOnFail; }
            set { actionOnFail = value; }
        }

        String dumpList;
        public String cmdDumpList
        {
            get { return dumpList; }
            set { dumpList = value; }
        }


        public GTAReportCommands(String result, String statement, bool isIgnored, string execDate, string execTime, string execEpoch, string iFeedback, string iRef, string iComment, string ilinenumber)
        {
            
            Commands = new List<Command>();
            Result = result;
            Statememt = statement;
            Ignored = isIgnored;
            strExecTime = new execTimeStamp();
            strExecTime.date = execDate;
            strExecTime.time = execTime;
            strExecTime.epoch = execEpoch;
            feedback = iFeedback;
            references = iRef;
            comment = iComment;
            linenumber = ilinenumber;
        }

        public void insertCommand(string name, bool isIgnored, string result, string Statememt, string execDate, string execTime, string execEpoch, List<Other_Details> listOtherDetails, string comment, String strCommandType, String iFeedback, String iReference, string ilinenumber)
        {
            Command cmd = new Command();
            execTimeStamp timeDate = new execTimeStamp();
            timeDate.date = execDate;
            timeDate.time = execTime;
            timeDate.epoch = execEpoch;

            cmd.name = name;
            cmd.result = result;
            cmd.Statememt = Statememt;
            cmd.isIgnored = isIgnored;
            cmd.execTime = timeDate;
            cmd.comment = comment;
            cmd.references = iReference;
            cmd.feedback = iFeedback;
            cmd.linenumber = ilinenumber;

            if (strCommandType == "TITLE")
                cmd.cmdType = CommandType.TITLE;
            else if (strCommandType == "CALL")
                cmd.cmdType = CommandType.CALL;
            else if (strCommandType == "ACTION")
                cmd.cmdType = CommandType.ACTION;
            else if (strCommandType == "CHECK")
                cmd.cmdType = CommandType.CHECK;
            else
                cmd.cmdType = CommandType.ACTION;

            cmd.details = listOtherDetails;

            Commands.Add(cmd);
        }

    }
}
