using System;
using System.Collections.Generic;
using System.Text;

namespace GTATransformToDocx
{
    class GTAReportDM
    {
        GTAReportSummary clsSummaryData;

        internal GTAReportSummary SummaryData
        {
            get { return clsSummaryData; }
            set { clsSummaryData = value; }
        }

        GTAReportHeader clsReportHeaderData;

        public GTAReportHeader ReportHeaderData
        {
            get { return clsReportHeaderData; }
            set { clsReportHeaderData = value; }
        }

        GTAReportATTACHMENTS clsReportAttachement;

        public GTAReportATTACHMENTS ReportAttachement
        {
            get { return clsReportAttachement; }
            set { clsReportAttachement = value; }
        }

        List<GTAReportCommands> lstMainCommands;

        public List<GTAReportCommands> MainCommands
        {
            get { return lstMainCommands; }
            set { lstMainCommands = value; }
        }

        List<GTAReportDM> lstAnnexures;

        public List<GTAReportDM> Annexures
        {
            get { return lstAnnexures; }
            set { lstAnnexures = value; }
        }

        GTAReportCommands objMainCommand;

        public GTAReportCommands MainCommand
        {
            get { return objMainCommand; }
            set { objMainCommand = value; }
        }

        private GTAReportDataSources clsReportDataSources;

        public GTAReportDataSources ReportDataSources
        {
            get { return clsReportDataSources; }
            set { clsReportDataSources = value; }
        }

        List<GTAReportPrintTable> lstPrintTables;

        public List<GTAReportPrintTable> PrintTables
        {
            get { return lstPrintTables; }
            set { lstPrintTables = value; }
        }

        public string procedurePurpose { get; set; }

        public string procedureConclusion {  get; set; }


        String annexureName;
        String annexureExecTime;
        String annexureExecDate;
        String annexureExecEpoch;
        String annexureResult;

        public enum HEADERINFO { TESTDETAIL, SPECIFICATION, OTHERDETAILS };
        public enum CMD_SCOPE_TYPE { MAIN, ANNEX };

        CMD_SCOPE_TYPE Scope_Type;

        public CMD_SCOPE_TYPE enumScope_Type
        {
            get { return Scope_Type; }
            set { Scope_Type = value; }
        }

        public GTAReportDM(CMD_SCOPE_TYPE scope_type = CMD_SCOPE_TYPE.MAIN)
        {
            clsSummaryData = new GTAReportSummary();
            clsReportHeaderData = new GTAReportHeader();
            clsReportAttachement = new GTAReportATTACHMENTS();
            clsReportDataSources = new GTAReportDataSources();
            objMainCommand = new GTAReportCommands("", "", false, "", "", "","","","","");
            lstMainCommands = new List<GTAReportCommands>();
            lstAnnexures = new List<GTAReportDM>();
            lstPrintTables = new List<GTAReportPrintTable>();
            Scope_Type = scope_type;
            procedurePurpose = "";
            procedureConclusion = "";
        }

        public void insertSummaryItems(String summaryItemText, String summaryItemResult)
        {
            summaryItemStruct strSummary = new summaryItemStruct();
            strSummary.SummaryItemResult = summaryItemResult;
            strSummary.SummaryItemText = summaryItemText;
            SummaryData.insertItemsSummary(strSummary);
        }

        public void insertSummaryInfos(String summaryInfoText, String summaryInfoResult)
        {
            summaryInfoStruct strSummary = new summaryInfoStruct();
            strSummary.SummaryInfoResult = summaryInfoResult;
            strSummary.SummaryInfoText = summaryInfoText;
            SummaryData.insertInfosSummary(strSummary);
        }

        public void insertReportInfo(String date, String title, String desName, string ltrref, string facility, string gtaVersion, string gtaResult)
        {
            ReportInfo strReportInfo = new ReportInfo();
            strReportInfo.Date = date;
            strReportInfo.DesignerName = desName;
            strReportInfo.Title = title;
            strReportInfo.LTRREF = ltrref;
            strReportInfo.Facility = facility;
            strReportInfo.GTAVersion = gtaVersion;
            strReportInfo.Result = gtaResult;
            ReportHeaderData.updateReportInfo(strReportInfo);
        }

        public void insertHeaderData(String item, String value, HEADERINFO infoType)
        {
            OtherInfo info = new OtherInfo();
            info.item = item;
            info.value = value;
            switch (infoType)
            {
                case HEADERINFO.TESTDETAIL:
                    ReportHeaderData.insertTestDetails(info);
                    break;
                case HEADERINFO.SPECIFICATION:
                    ReportHeaderData.insertSpecDetails(info);
                    break;
                case HEADERINFO.OTHERDETAILS:
                    ReportHeaderData.insertOtherDetails(info);
                    break;
                default:
                    break;
            }
        }

        public void insertATTACHMENTS(string item)
        {
            ReportAttachement.insertAttachement(item);
        }

        public void insertDataSource(string fileName, string fileType, string filePath )
        {
            DataSource dataSource = new DataSource();
            dataSource.fileName = fileName;
            dataSource.fileType = fileType;
            dataSource.filePath = filePath;
            ReportDataSources.InsertDataSource(dataSource);
        }

        public void insertPrintTable(GTAReportPrintTable iPrintTable)
        {
            PrintTables.Add(iPrintTable);
        }

        public void insertCommandInMainCommand(string name, bool isIgnored, string result, string Statememt, string execDate, string execTime, string execEpoch, List<Other_Details> details, string comment, String commandType, String iFeedback, String iDefect, string ilinenumber)
        {
            if (MainCommand.Statememt == "")
                MainCommand.Statememt = "—No Section Title—";
            MainCommand.insertCommand(name, isIgnored, result, Statememt, execDate, execTime, execEpoch, details, comment, commandType, iFeedback, iDefect, ilinenumber);
        }

        public void insertProcedurePurpose(string procedurePurposeValue)
        {
            procedurePurpose = procedurePurposeValue;

		}

        public void insertProcedureConclusion(string procedureConclusionValue)
        {
            procedureConclusion = procedureConclusionValue;

		}

        public void addNewMainCommand(CMD_SCOPE_TYPE cmdScope, String Result, String statement, bool isIgnored, string execDate, string execTime, string execEpoch, string feedback, string defect,string iComment, string ilinenumber)
        {
            //Ignore MainCommand that has no statement
            if (MainCommand.Statememt != "")
                MainCommands.Add(MainCommand);
            MainCommand = new GTAReportCommands(Result, statement, isIgnored, execDate, execTime, execEpoch, feedback, defect, iComment, ilinenumber);
        }

        public void setAnnexures(List<GTAReportDM> annexures)
        {
            this.Annexures = annexures;
        }

        public void setAnnexName(String annexName)
        {
            annexureName = annexName;
        }

        public void setAnnexExecTimeDate(String annexTime, String annexDate, String annexEpoch)
        {
            annexureExecTime = annexTime;
            annexureExecDate = annexDate;
            annexureExecEpoch = annexEpoch;
        }

        public void setAnnexResult(String annexResult)
        {
            annexureResult = annexResult;
        }

        public String getAnnexName()
        {
            return annexureName;
        }

        public String getAnnexEpochTime()
        {
            return annexureExecEpoch;
        }

        public String getAnnexResult()
        {
            return annexureResult;
        }	
		
    }
}
