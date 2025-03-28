using DevExpress.Internal.WinApi.Windows.UI.Notifications;
using DevExpress.Xpo.DB.Helpers;
using DocumentFormat.OpenXml.Spreadsheet;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;


namespace GTATransformToDocx
{
    class GTAXMLReportParser
    {
        GTAReportDM strReport;
        List<GTAReportDM> annexure;

        public GTAXMLReportParser()
        {
            strReport = new GTAReportDM();
            annexure = new List<GTAReportDM>();
        }

        ~GTAXMLReportParser()
        {
            strReport = null;
        }

        public GTAReportDM parseReportFile(String reportFile, bool isMilliSec)
        {
            XmlDocument doc = new XmlDocument();
            doc.Load(reportFile);

            strReport = new GTAReportDM();

            XmlNodeList children = doc.ChildNodes;

            if (children.Count > 0)
            {
                for (int i = 0; i < children.Count; i++)
                {
                    XmlNode reportNode = children.Item(i);

                    if (reportNode.Name == "REPORT")
                    {
                        if (reportNode.HasChildNodes)
                        {

                            for (int j = 0; j < reportNode.ChildNodes.Count; j++)
                            {
                                XmlNode reportNodeItems = reportNode.ChildNodes.Item(j);
                                if (reportNodeItems.Name == "REPORT_SUMMARY")
                                {
                                    parseSummarySection(reportNodeItems);
                                }
                                else if (reportNodeItems.Name == "REPORT_HEADER")
                                {
                                    parseHeaderSection(reportNodeItems);
                                }
                                else if (reportNodeItems.Name == "ATTACHMENTS")
                                {
                                    parseATTACHMENTSection(reportNodeItems);
                                }
                                else if (reportNodeItems.Name == "REPORT_INFO")
                                {
                                    parseReportInfoSection(reportNodeItems, GTAReportDM.CMD_SCOPE_TYPE.MAIN, strReport, true, isMilliSec);
                                    strReport.addNewMainCommand(GTAReportDM.CMD_SCOPE_TYPE.MAIN, "", "", false, "", "", "","","","","");
                                }
                                else if (reportNodeItems.Name == "DATABASE_FILES")
                                {
                                    parseDatabaseFilesSection(reportNodeItems);
                                }
                                else if (reportNodeItems.Name == "PRINT_TABLES")
                                {
                                    parsePrintTablesSection(reportNodeItems);
                                }
                                else if (reportNodeItems.Name == "PROCEDURE_PURPOSE")
                                {
									parseProcedurePurposeSection(reportNodeItems);
                                }
                                else if (reportNodeItems.Name == "PROCEDURE_CONCLUSION")
                                {
                                    parseProcedureConclusionSection(reportNodeItems);
                                }
                            }
                        }

                    }
                }
            }
            strReport.setAnnexures(annexure);
            return strReport;
        }

        public bool procedurePurposeNodePresent(String reportFile)
        {
            XmlDocument doc = new XmlDocument();
            doc.Load(reportFile);

            bool procedurePurposeNodePresent = false;
            XmlNodeList children = doc.ChildNodes;

            if (children.Count > 0)
            {
                for (int i = 0; i < children.Count; i++)
                {
                    XmlNode reportNode = children.Item(i);

                    if (reportNode.Name == "REPORT")
                    {
                        if (reportNode.HasChildNodes)
                        {

                            for (int j = 0; j < reportNode.ChildNodes.Count; j++)
                            {
                                XmlNode reportNodeItems = reportNode.ChildNodes.Item(j);
                                if (reportNodeItems.Name == "PROCEDURE_PURPOSE")
                                {
									procedurePurposeNodePresent = true;

								}
                            }
                        }
                    }
                }
            }

            return procedurePurposeNodePresent;
        }

		private void parseReportInfoSection(XmlNode ReportInfoNode, GTAReportDM.CMD_SCOPE_TYPE scope, GTAReportDM report, bool bAddMainCommand = true, bool isMilliSec = false)
		{
			try
			{
				XmlNodeList xmlNodes = ReportInfoNode.ChildNodes;
				for (int i = 0; i < xmlNodes.Count; i++)
				{
					XmlNode node = xmlNodes.Item(i);
					String complementName = "";
					String NodeName = node.Name;
					String isIgnored = "";
					String cmdName = "";

					if ((NodeName == "ACTION") || (NodeName == "CALL") || (NodeName == "CHECK") || (NodeName == "CONDITION"))
					{
						complementName = node.Attributes.GetNamedItem("COM_NAME").Value;
						cmdName = node.Attributes.GetNamedItem("NAME").Value;
						isIgnored = node.Attributes.GetNamedItem("IS_IGNORED").Value;
					}
					else if (NodeName == "TITLE")
					{
						isIgnored = node.Attributes.GetNamedItem("IS_IGNORED").Value;
					}

					String execDate = "";
					String execTime = "";
					String execEpoch = "";

					String statement = "";
					String status = "";
					String comment = "";
					String GTALineNumber = "";
					String feedback = "";
                    String defects = "";

					List<Other_Details> listOtherDetails = new List<Other_Details>();

					bool isIgnoreAction = false;

					List<Dictionary<String, String>> ParamDetails = new List<Dictionary<string, string>>();
					XmlNodeList childrens = node.ChildNodes;
					for (int j = 0; j < childrens.Count; j++)
					{
						XmlNode childNode = childrens.Item(j);
						if (childNode.Name == "TIMESTAMP")
						{
							execTime = childNode.Attributes.GetNamedItem("EXEC_TIME").Value;
							execDate = childNode.Attributes.GetNamedItem("EXEC_DATE").Value;
                            execEpoch = childNode.Attributes.GetNamedItem("EPOC_TIME").Value;
                            if (isMilliSec == true)
                            {
                                if (execEpoch != "")
                                {
                                    double time = Convert.ToDouble(execEpoch, System.Globalization.CultureInfo.InvariantCulture) % 1000;
                                    string strTime = time.ToString();
                                    execTime = execTime + "." + strTime;
                                }
                            }
                        }
                        else if (childNode.Name == "STATEMENT")
                        {
                            statement = childNode.InnerText;
                        }
                        else if (childNode.Name == "HEADING")
                        {
                            statement = childNode.InnerText;
                        }
                        else if (childNode.Name == "STATUS")
                        {
                            status = childNode.InnerText;
                        }
                        else if (childNode.Name == "COMMENTS")
                        {
                            comment = childNode.InnerText;
                        }
                        else if (childNode.Name == "GTA_LINENUMBER")
                        {
                            GTALineNumber = childNode.InnerText;
                        }
                        else if (childNode.Name == "USER_FEEDBACK")
                        {
                            feedback = childNode.InnerText;
                        }
                        else if (childNode.Name == "DEFECTS")
                        {
                            defects = childNode.InnerText;
                        }
                        else if (childNode.Name == "OTHER_DETAILS")
                        {
                            XmlNodeList otherDetailsNodeList = childNode.ChildNodes;
                            for (int k = 0; k < otherDetailsNodeList.Count; k++)
                            {
                                XmlNode paramInfoNode = otherDetailsNodeList.Item(k);
                                XmlNode paraminfo = paramInfoNode.Attributes.GetNamedItem("NAME");
                                if (paraminfo != null)
                                {
                                    Other_Details objOtherDetails = new Other_Details();
                                    objOtherDetails.paramDetails = new List<Param_Detail>();

                                    Param_Detail pDetail = new Param_Detail();
                                    pDetail.name = paraminfo.Name;
                                    pDetail.value = paraminfo.InnerText;

                                    objOtherDetails.paramDetails.Add(pDetail);

                                    XmlNodeList paramChildNodes = paramInfoNode.ChildNodes;
                                    foreach (XmlNode paramChild in paramChildNodes)
                                    {
                                        pDetail = new Param_Detail();
                                        pDetail.name = paramChild.Name;
                                        pDetail.value = paramChild.InnerText;

                                        objOtherDetails.paramDetails.Add(pDetail);
                                    }

                                    listOtherDetails.Add(objOtherDetails);
                                }
                            }
                        }
                    }

                    if (isIgnored == "TRUE")
                        isIgnoreAction = true;
                    else
                        isIgnoreAction = false;

                    if (statement != "")
                    {
                        if (NodeName == "TITLE")
                        {
                            if (scope == GTAReportDM.CMD_SCOPE_TYPE.MAIN)
                            {
                                //report.addNewMainCommand(scope, status, statement, isIgnoreAction, execDate, execTime, execEpoch, feedback, defects,comment);
                                report.addNewMainCommand(scope, status, statement, isIgnoreAction, execDate, execTime, execEpoch, feedback, defects, comment, GTALineNumber);
                                parseReportInfoSection(node, scope, report, true, isMilliSec);
                            }
                            else if (scope == GTAReportDM.CMD_SCOPE_TYPE.ANNEX)
                            {
                                report.addNewMainCommand(scope, status, statement, isIgnoreAction, execDate, execTime, execEpoch, feedback, defects, comment, GTALineNumber);
                                parseReportInfoSection(node, scope, report, true, isMilliSec);
                            }
                        }
                        else
                        {
                            report.insertCommandInMainCommand(cmdName + "_" + complementName, isIgnoreAction, status, statement, execDate, execTime, execEpoch, listOtherDetails, comment, NodeName, feedback, defects, GTALineNumber);
                            if (NodeName == "CONDITION")
                                parseReportInfoSection(node, scope, report, false, isMilliSec);
                        }

                        if (((NodeName == "CALL") || (cmdName == "Parallel Call")) && !isIgnoreAction)
                        {
                            GTAReportDM annexReport = new GTAReportDM(GTAReportDM.CMD_SCOPE_TYPE.ANNEX);
                            annexReport.setAnnexName(statement);
                            annexReport.setAnnexExecTimeDate(execTime, execDate, execEpoch);
                            annexReport.setAnnexResult(status);

                            /*annexReport.MainCommand.Statememt = statement;
                            execTimeStamp ExecTimeStamp = new execTimeStamp();
                            ExecTimeStamp.date = execDate;
                            ExecTimeStamp.time = execTime;
                            ExecTimeStamp.epoch = execEpoch;
                            annexReport.MainCommand.ExecTime = ExecTimeStamp;
                            annexReport.MainCommand.Result = status;*/

                            parseReportInfoSection(node, GTAReportDM.CMD_SCOPE_TYPE.ANNEX, annexReport, true, isMilliSec);
                            annexure.Add(annexReport);
                        }
                    }
                }

                //Adds the latest Main Command to list of Main Commands. Creates an empty Main command.
                if (bAddMainCommand)
                    report.addNewMainCommand(scope, "", "", false, "", "", "", "", "", "", "");
            }
            catch(Exception ex)
            {
                Console.WriteLine("parseReportInfoSection Inner Exception: " + ex.InnerException);
                Console.WriteLine("parseReportInfoSection Message : " + ex.Message);
                Console.WriteLine("parseReportInfoSection StackTrace : " + ex.StackTrace);
            }
        }

        private void parseHeaderSection(XmlNode HeaderNode)
        {
            if (HeaderNode.HasChildNodes)
            {
                XmlNodeList headerSectionNodeList = HeaderNode.ChildNodes;
                for (int i = 0; i < headerSectionNodeList.Count; i++)
                {
                    XmlNode headerNode = headerSectionNodeList.Item(i);
                    if (headerNode.Name == "REPORTINFO")
                    {
                        parseReportInfoHeader(headerNode);
                    }
                    else if (headerNode.Name == "TESTDETAIL")
                    {
                        parseTestDetailsHeader(headerNode);
                    }
                    else if (headerNode.Name == "SPECIFICATION")
                    {
                        parseSpecDetailsHeader(headerNode);
                    }
                    else if (headerNode.Name == "OTHERDETAILS")
                    {
                        parseOtherDetailsHeader(headerNode);
                    }
                }
            }
        }

        private void parseTestDetailsHeader(XmlNode TestDetails)
        {
            if (TestDetails.HasChildNodes)
            {
                XmlNodeList reportChildrenList = TestDetails.ChildNodes;
                if (reportChildrenList.Count > 0)
                {
                    String item = "";
                    String val = "";
                    for (int i = 0; i < reportChildrenList.Count; i++)
                    {
                        XmlNode itemNode = reportChildrenList.Item(i);
                        item = itemNode.Attributes.GetNamedItem("name").Value;
                        val = itemNode.Attributes.GetNamedItem("value").Value;
                        strReport.insertHeaderData(item, val, GTAReportDM.HEADERINFO.TESTDETAIL);
                    }
                }
            }
        }

        private void parseSpecDetailsHeader(XmlNode SpecDetails)
        {
            if (SpecDetails.HasChildNodes)
            {
                XmlNodeList reportChildrenList = SpecDetails.ChildNodes;
                if (reportChildrenList.Count > 0)
                {
                    String item = "";
                    String val = "";
                    for (int i = 0; i < reportChildrenList.Count; i++)
                    {
                        XmlNode itemNode = reportChildrenList.Item(i);
                        item = itemNode.Attributes.GetNamedItem("name").Value;
                        val = itemNode.Attributes.GetNamedItem("value").Value;
                        strReport.insertHeaderData(item, val, GTAReportDM.HEADERINFO.SPECIFICATION);
                    }
                }
            }
        }

        private void parseOtherDetailsHeader(XmlNode OtherDetails)
        {
            if (OtherDetails.HasChildNodes)
            {
                XmlNodeList reportChildrenList = OtherDetails.ChildNodes;
                if (reportChildrenList.Count > 0)
                {
                    String item = "";
                    String val = "";
                    for (int i = 0; i < reportChildrenList.Count; i++)
                    {
                        XmlNode itemNode = reportChildrenList.Item(i);
                        item = itemNode.Attributes.GetNamedItem("name").Value;
                        val = itemNode.Attributes.GetNamedItem("value").Value;
                        strReport.insertHeaderData(item, val, GTAReportDM.HEADERINFO.OTHERDETAILS);
                    }
                }
            }
        }

        private void parseReportInfoHeader(XmlNode ReportInfo)
        {
            if (ReportInfo.HasChildNodes)
            {
                XmlNodeList reportChildrenList = ReportInfo.ChildNodes;
                if (reportChildrenList.Count > 0)
                {
                    String date = "";
                    String title = "";
                    String desName = "";
                    String ltref = "";
                    String facility = "";
                    string gtaVersion = "";
                    string gtaResult = "";

                    for (int i = 0; i < reportChildrenList.Count; i++)
                    {
                        XmlNode itemNode = reportChildrenList.Item(i);
                        if (itemNode.Name == "DATE")
                        {
                            date = itemNode.InnerText;
                        }
                        else if (itemNode.Name == "TITLE")
                        {
                            title = itemNode.InnerText;
                        }
                        else if (itemNode.Name == "DESIGNERNAME")
                        {
                            desName = itemNode.InnerText;
                        }
                        else if (itemNode.Name == "LTRREF")
                        {
                            ltref = itemNode.InnerText;
                        }
                        else if (itemNode.Name == "FACILITY")
                        {
                            facility = itemNode.InnerText;
                        }
                        else if (itemNode.Name == "GTAVERSION")
                        {
                            gtaVersion = itemNode.InnerText;
                        }
                        else if (itemNode.Name == "GLOBAL_RESULT")
                        {
                            gtaResult = itemNode.InnerText;
                        }
                    }
                    strReport.insertReportInfo(date, title, desName, ltref, facility, gtaVersion, gtaResult);
                }
            }
        }

        private void parseSummarySection(XmlNode SummaryNode)
        {
            if (SummaryNode.HasChildNodes)
            {
                XmlNodeList SummarySectionNodeList = SummaryNode.ChildNodes;
                for (int i = 0; i < SummarySectionNodeList.Count; i++)
                {
                    XmlNode summaryNode = SummarySectionNodeList.Item(i);
                    if (summaryNode.Name == "REPORT_SUMMARY_INFOS")
                    {
                        parseReportInfosSummary(summaryNode);
                    }
                    else if (summaryNode.Name == "REPORT_SUMMARY_ITEMS")
                    {
                        parseItemsSummarySection(summaryNode);
                    }
                }
            }
        }

        private void parseReportInfosSummary(XmlNode SummaryInfos)
        {
            if (SummaryInfos.HasChildNodes)
            {
                XmlNodeList SummaryInfosChildrenList = SummaryInfos.ChildNodes;
                if (SummaryInfosChildrenList.Count > 0)
                {
                    for (int i = 0; i < SummaryInfosChildrenList.Count; i++)
                    {
                        XmlNode InfoNode = SummaryInfosChildrenList.Item(i);
                        String Info = InfoNode.Attributes.GetNamedItem("TITLE").Value;
                        String result = InfoNode.Attributes.GetNamedItem("RESULT").Value;
                        strReport.insertSummaryInfos(Info, result);
                    }
                }
            }
        }

        private void parseItemsSummarySection(XmlNode SummaryItems)
        {

            if (SummaryItems.HasChildNodes)
            {
                XmlNodeList SummaryItemsChildrenList = SummaryItems.ChildNodes;
                if (SummaryItemsChildrenList.Count > 0)
                {
                    for (int i = 0; i < SummaryItemsChildrenList.Count; i++)
                    {
                        XmlNode ItemNode = SummaryItemsChildrenList.Item(i);
                        String Item = ItemNode.Attributes.GetNamedItem("TITLE").Value;
                        String result = ItemNode.Attributes.GetNamedItem("RESULT").Value;
                        strReport.insertSummaryItems(Item, result);
                    }
                }
            }
        }

        private void parseATTACHMENTSection(XmlNode AttachementNode)
        {
            XmlNodeList attachementChildrenNodeList = AttachementNode.ChildNodes;
            for (int i = 0; i < attachementChildrenNodeList.Count; i++)
            {
                XmlNode itemNode = attachementChildrenNodeList.Item(i);
                String item = itemNode.Attributes.GetNamedItem("Path").Value;
                if(File.Exists(item))
                    strReport.insertATTACHMENTS(item);
            }
        }

        private void parseDatabaseFilesSection(XmlNode DatabaseFileNode)
        {
            if (DatabaseFileNode.HasChildNodes)
            {
                XmlNodeList reportChildrenList = DatabaseFileNode.ChildNodes;
                if (reportChildrenList.Count > 0)
                {
                    for (int i = 0; i < reportChildrenList.Count; i++)
                    {
                        XmlNode itemNode = reportChildrenList.Item(i);
                        strReport.insertDataSource(itemNode.Attributes.GetNamedItem("FILENAME").Value,
                            itemNode.Attributes.GetNamedItem("FILETYPE").Value,
                            itemNode.Attributes.GetNamedItem("FILEPATH").Value);
                    }
                }
            }
        }

        private void parsePrintTablesSection(XmlNode DatabaseFileNode)
        {
            if (DatabaseFileNode.HasChildNodes)
            {
                XmlNodeList reportChildrenList = DatabaseFileNode.ChildNodes;
                if (reportChildrenList.Count > 0)
                {
                    for (int i = 0; i < reportChildrenList.Count; i++)
                    {
                        GTAReportPrintTable objPrintTable = new GTAReportPrintTable();

                        XmlNode itemNode = reportChildrenList.Item(i);
                        objPrintTable.TableName = itemNode.Attributes.GetNamedItem("NAME").Value;

                        XmlNodeList tableChildrenList = itemNode.ChildNodes;
                        for (int j = 0; j < tableChildrenList.Count; j++)
                        {
                            XmlNode itemTableNode = tableChildrenList.Item(j);
                            if (itemTableNode.Name == "TABLE_HEADER")
                            {
                                string strFullHeaderName = itemTableNode.Attributes.GetNamedItem("VALUE").Value;

                                while (strFullHeaderName != "")
                                {
                                    int index = strFullHeaderName.IndexOf(";");
                                    if (index > 0)
                                    {
                                        objPrintTable.TableHeaders.Add(strFullHeaderName.Substring(0, index));
                                        strFullHeaderName = strFullHeaderName.Substring(index+1);
                                    }
                                    else
                                    {
                                        objPrintTable.TableHeaders.Add(strFullHeaderName);
                                        strFullHeaderName = "";
                                    }
                                }
                            }
                            else if (itemTableNode.Name == "PARAMETER_INFO")
                            {
                                GTAReportParamTable objParamTable = new GTAReportParamTable();
                                objParamTable.ParamName = itemTableNode.Attributes.GetNamedItem("NAME").Value;
                                objParamTable.ParamType = itemTableNode.Attributes.GetNamedItem("TYPE").Value;
                                objParamTable.ParamUnit = itemTableNode.Attributes.GetNamedItem("UNIT").Value;

                                XmlNodeList ParamGroupsList = itemTableNode.ChildNodes;
                                if (ParamGroupsList.Count > 0)
                                {
                                    XmlNodeList ParamGroupList = ParamGroupsList[0].ChildNodes;
                                    foreach (XmlNode ParamDetails in ParamGroupList)
                                    {
                                        Other_Details objOtherDetails = new Other_Details();
                                        objOtherDetails.paramDetails = new List<Param_Detail>();

                                        XmlNodeList ParamList = ParamDetails.ChildNodes;
                                        foreach (XmlNode Param in ParamList)
                                        {
                                            Param_Detail pDetail = new Param_Detail();
                                            pDetail.name = Param.Name;
                                            pDetail.value = Param.InnerText;
                                            objOtherDetails.paramDetails.Add(pDetail);
                                        }
                                        objParamTable.TableDetails.Add(objOtherDetails);
                                    }
                                }

                                objPrintTable.ParamTables.Add(objParamTable);
                            }
                        }

                        strReport.insertPrintTable(objPrintTable);
                    }
                }
            }
        }
        private void parseProcedurePurposeSection(XmlNode ProcedurePurposeNode)
        {
			string value = ProcedurePurposeNode.Attributes.GetNamedItem("VALUE").Value;
			strReport.insertProcedurePurpose(value);
		}

		private void parseProcedureConclusionSection(XmlNode ProcedureConclusionNode)
		{
			string value = ProcedureConclusionNode.Attributes.GetNamedItem("VALUE").Value;
            strReport.insertProcedureConclusion(value);
		}

		private void parseAnnexureList()
        {

        }

    }
}
