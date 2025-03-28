using System;
using System.Drawing;
using System.Collections.Generic;

using DevExpress.XtraRichEdit.API.Native;
using System.Windows.Forms;
using Microsoft.Office.Interop.Word;
using System.IO;
using DevExpress.XtraRichEdit.API.Word;

namespace GTATransformToDocx
{
    enum FontStyleType
    {
        None,
        Bold,
        Italic,
        HeaderName,
        HeaderTestsCount,
        FooterDate,
        FooterName,
        FooterPageNo,
        ReportInfo,
        ReportInfoValues,
        TestDetails,
        SpecificationsHeader,
        OtherDetailsHeader,
        SummaryHeader,
        MainCommandsHeader,
        ItemName,
        MainTitle,
        MainResult,
        ActionTitle,
        ActionText,
        ActionSplText,
        ActionResult,
        ActionResultKO,
        CheckTitle,
        CheckText,
        CheckResult,
        CheckResultKO,
        ResultNA,
        ResultTimeStamp,
        DescriptionReference,
        ResultsUserFeedback,
        Command,
        Strikeout,
        ColorAction,
        ProcedurePurpose,
        Conclusion
    }

    class GTARichTextEditorUtils
    {
        static int AnnexureTableNo = 0;
        //     static int count = 1;

        public static void InsertReportHeaderData(DevExpress.XtraRichEdit.API.Native.Document document, GTAReportHeader docHeader)
        {
            InsertReportHeaderInfo(document, docHeader.reportInfo);
            InsertReportHeaderTestDetails(document, docHeader.TestDetails);
            InsertReportHeaderSpecifications(document, docHeader.Specifications);
            InsertReportHeaderOtherDetails(document, docHeader.OtherDetails);
        }

        public static void InsertReportHeaderInfo(DevExpress.XtraRichEdit.API.Native.Document document, ReportInfo objReportInfo)
        {
            DevExpress.XtraRichEdit.API.Native.Table tbl = document.Tables.Create(document.Range.End, 6, 2, AutoFitBehaviorType.FixedColumnWidth);
            document.Bookmarks.Create(tbl.Range, "HeaderInfo");

            //Set Column Width
            tbl[0, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tbl[0, 0].PreferredWidth = 12.0f;
            tbl[0, 1].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tbl[0, 1].PreferredWidth = 63.0f;

            string strDate = objReportInfo.Date;
            string strDesigner = objReportInfo.DesignerName;
            string strFacility = objReportInfo.Facility;
            string strTitle = objReportInfo.Title;
            string strResult = objReportInfo.Result;
            string strLTRAReference = objReportInfo.LTRREF;


			document.InsertText(tbl[0, 0].Range.Start, "LTRA Title: ");
            document.InsertText(tbl[0, 1].Range.Start, strTitle);
			document.InsertText(tbl[1, 0].Range.Start, "LTRA Reference: ");
			document.InsertText(tbl[1, 1].Range.Start, strLTRAReference);
			document.InsertText(tbl[2, 0].Range.Start, "Designer: ");
            document.InsertText(tbl[2, 1].Range.Start, strDesigner);
            document.InsertText(tbl[3, 0].Range.Start, "Execution Date: ");
            document.InsertText(tbl[3, 1].Range.Start, strDate);
            document.InsertText(tbl[4, 0].Range.Start, "Facility Used");
            document.InsertText(tbl[4, 1].Range.Start, strFacility);
            document.InsertText(tbl[5, 0].Range.Start, "Global Result: ");
            //document.InsertText(tbl[4, 1].Range.Start, strResult);
            AddFormattedResult(strResult, tbl[5, 1].Range.Start, document);
            ApplyFontSize(tbl[4, 1].Range, 12, document);

            for (int ii = 0; ii < 5; ii++)
            ApplyStyle(FontStyleType.ReportInfo, tbl[ii, 0].Range, document);
            ApplyFormattedStyle(tbl[0, 1].Range, document, rFontSize: 14, bBold: true);
			ApplyFormattedStyle(tbl[1, 1].Range, document, rFontSize: 14, bBold: true);
			ApplyStyle(FontStyleType.ReportInfoValues, tbl[2, 1].Range, document);
            ApplyStyle(FontStyleType.ReportInfo, tbl[3, 1].Range, document);
            ApplyStyle(FontStyleType.ReportInfoValues, tbl[4, 1].Range, document);

            tbl.ForEachCell(new TableCellProcessorDelegate(RemoveTableBorderLines));
            tbl.ForEachCell(new TableCellProcessorDelegate(SetCellVerticalAlignmentToCenter));

            document.Paragraphs.Insert(tbl.Range.End);
            document.Paragraphs.Insert(tbl.Range.End);
        }

        public static void InsertReportHeaderTestDetails(DevExpress.XtraRichEdit.API.Native.Document document, List<OtherInfo> lstTestDetails)
        {
            DocumentRange dRange = document.InsertText(document.Range.End, "Additional information:");
            ApplyFormattedStyle(dRange, document, rFontSize: 10, typeUnderline: UnderlineType.Single);
            document.Paragraphs.Insert(dRange.End);
            document.Paragraphs.Insert(dRange.End);

            DevExpress.XtraRichEdit.API.Native.Table tbl = InsertHeaderOtherInfoTable(document, lstTestDetails);
            document.Bookmarks.Create(tbl.Range, "HeaderTestDetails");

            ApplyStyle(FontStyleType.TestDetails, tbl.Range, document);
        }

        public static void InsertReportHeaderSpecifications(DevExpress.XtraRichEdit.API.Native.Document document, List<OtherInfo> lstSpecifications)
        {
            DevExpress.XtraRichEdit.API.Native.Table tbl = InsertHeaderOtherInfoTable(document, lstSpecifications);
            document.Bookmarks.Create(tbl.Range, "HeaderSpecifications");

            ApplyStyle(FontStyleType.SpecificationsHeader, tbl.FirstRow.Range, document);
        }

        public static void InsertReportHeaderOtherDetails(DevExpress.XtraRichEdit.API.Native.Document document, List<OtherInfo> lstOtherDetails)
        {
            int OtherDetailsTableSize = 6;
            int ii = 0;

            while (lstOtherDetails.Count > ii)
            {
                int BookmartkNo = ii / 6 + 1;
                List<OtherInfo> TempOtherDetails = new List<OtherInfo>();
                for (int jj = 0; jj < OtherDetailsTableSize; jj++)
                {
                    if (ii < lstOtherDetails.Count)
                    {
                        TempOtherDetails.Add(lstOtherDetails[ii]);
                        ii++;
                    }
                }

                DevExpress.XtraRichEdit.API.Native.Table tbl = InsertHeaderOtherInfoTable(document, TempOtherDetails);
                document.Bookmarks.Create(tbl.Range, "HeaderOtherInfo" + BookmartkNo.ToString());

                ApplyStyle(FontStyleType.OtherDetailsHeader, tbl.FirstRow.Range, document);
            }
        }

        internal static DevExpress.XtraRichEdit.API.Native.Table InsertHeaderOtherInfoTable(DevExpress.XtraRichEdit.API.Native.Document document, List<OtherInfo> lstTestDetails)
        {
            DevExpress.XtraRichEdit.API.Native.Table tbl = document.Tables.Create(document.Range.End, 2, lstTestDetails.Count, AutoFitBehaviorType.AutoFitToWindow);

            int ii = -1;
            foreach (OtherInfo Info in lstTestDetails)
            {
                ii++;

                string strName = Info.item;
                string strValue = Info.value;

                document.InsertText(tbl[0, ii].Range.Start, strName);
                document.InsertText(tbl[1, ii].Range.Start, strValue);
            }

            document.Paragraphs.Insert(tbl.Range.End);
            document.Paragraphs.Insert(tbl.Range.End);

            tbl.ForEachCell(new TableCellProcessorDelegate(SetCellVerticalAlignmentToCenter));
            return tbl;
        }

        public static void InsertReportSummaryData(DevExpress.XtraRichEdit.API.Native.Document document, GTAReportSummary docSummary)
        {
            InsertReportSummaryInfos(document, docSummary.SummaryInfos);
            InsertReportSummaryItems(document, docSummary.SummaryItems);
        }

        private static void InsertReportSummaryInfos(DevExpress.XtraRichEdit.API.Native.Document document, List<summaryInfoStruct> SummaryInfos)
        {
            DevExpress.XtraRichEdit.API.Native.Table tblSummaryInfos = document.Tables.Create(document.Range.End, (SummaryInfos.Count-2), 3, AutoFitBehaviorType.AutoFitToContents);

            //Set Column Width
            tblSummaryInfos[0, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tblSummaryInfos[0, 0].PreferredWidth = 20.0f;
            tblSummaryInfos[0, 1].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tblSummaryInfos[0, 1].PreferredWidth = 40.0f;
            tblSummaryInfos[0, 2].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tblSummaryInfos[0, 2].PreferredWidth = 40.0f;

            // Set Titles 
            document.InsertText(tblSummaryInfos[0, 0].Range.Start, "Summary ");
            document.InsertText(tblSummaryInfos[0, 2].Range.Start, "Comments");
            tblSummaryInfos.MergeCells(tblSummaryInfos[0, 0], tblSummaryInfos[0, 1]);

            ApplyStyle(FontStyleType.SummaryHeader, tblSummaryInfos.FirstRow.Range, document);

            int ii = 0;

            tblSummaryInfos.Rows.Append();

            foreach (summaryInfoStruct item in SummaryInfos)
            {
                ii++;

                if (ii == 1)
                {
                    document.InsertText(tblSummaryInfos[ii, 0].Range.Start, item.SummaryInfoText);
                    AddFormattedResult(item.SummaryInfoResult, tblSummaryInfos[ii, 1].Range.Start, document);
                }

                // To colorize "OK", "KO" and "NA"
                if (ii > 1 && ii < 5)
                {
                    AddFormattedText(item.SummaryInfoText, tblSummaryInfos[ii, 0].Range.Start, document);
                    document.InsertText(tblSummaryInfos[ii, 1].Range.Start, item.SummaryInfoResult);
                }

                if (ii == 5)
                {
                    String TextToWriteInComment = "\n" + item.SummaryInfoText + " : " + item.SummaryInfoResult;
                    document.InsertText(tblSummaryInfos[ii - 2, 2].Range.Start, TextToWriteInComment);
                }

                if (ii == 6)
                {
                    String ListOfDefectsToWriteInComment = item.SummaryInfoText + " : " + item.SummaryInfoResult;
                    document.InsertText(tblSummaryInfos[ii - 3, 2].Range.Start, ListOfDefectsToWriteInComment);
                }
            }
        }
        private static void InsertReportSummaryItems(DevExpress.XtraRichEdit.API.Native.Document document, List<summaryItemStruct> SummaryItems)
        {
            DevExpress.XtraRichEdit.API.Native.Table tblSummaryItems = document.Tables.Create(document.Range.End, (SummaryItems.Count) + 1 , 3, AutoFitBehaviorType.AutoFitToContents);
            document.Bookmarks.Create(tblSummaryItems.Range, "Summary");

            //Set Column Width
            tblSummaryItems[0, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tblSummaryItems[0, 0].PreferredWidth = 20.0f;
            tblSummaryItems[0, 1].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tblSummaryItems[0, 1].PreferredWidth = 40.0f;
            tblSummaryItems[0, 2].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tblSummaryItems[0, 2].PreferredWidth = 40.0f;

            int ii = 0;
            tblSummaryItems.Rows.Append();

            foreach (summaryItemStruct item in SummaryItems)
            {
                AddFormattedResult(item.SummaryItemResult, tblSummaryItems[ii + 1, 0].Range.Start, document);

                DocumentRange dRange = document.InsertText(tblSummaryItems[ii + 1, 1].Range.Start, "> Item " + ii.ToString() + " - ");
                ApplyStyle(FontStyleType.Bold, dRange, document);

                DocumentRange dRangeBM = document.CreateRange(tblSummaryItems[ii + 1, 1].Range.Start, 7 + ii.ToString().Length);
                AddBookMark("Source@SummaryItem", item.SummaryItemText, dRangeBM, document);

                dRange = document.InsertText(dRange.End, item.SummaryItemText);
                ApplyStyle(FontStyleType.None, dRange, document);

                ii++;
            }

            if (SummaryItems.Count < 1)
            {
                tblSummaryItems.Rows.Append();
                DocumentRange dRange = document.InsertText(tblSummaryItems[2, 0].Range.Start, "> —No Section Title—");
                AddBookMark("Source@SummaryItem", "—No Section Title—", document.CreateRange(dRange.Start, 2), document);
            }

            tblSummaryItems.Rows.Append();

            int iRow = 0;

            foreach (TableRow MyRow in tblSummaryItems.Rows)
            {
                iRow++;

                if (iRow > 0)
                {
                    foreach (TableCell MyCell in MyRow.Cells)
                    {
                        MyCell.Borders.Top.LineStyle = TableBorderLineStyle.None;
                        if (MyRow != tblSummaryItems.LastRow)
                        MyCell.Borders.Bottom.LineStyle = TableBorderLineStyle.None;
                    }
                }
            }

            tblSummaryItems.ForEachCell(new TableCellProcessorDelegate(SetCellVerticalAlignmentToCenter));

            document.Paragraphs.Insert(tblSummaryItems.Range.End);
            document.Paragraphs.Insert(tblSummaryItems.Range.End);
        }

        public static void InsertProcedurePurpose(DevExpress.XtraRichEdit.API.Native.Document document, string procedurePurpose)
        {
			DevExpress.XtraRichEdit.API.Native.Table tbl = document.Tables.Create(document.Range.End, 2, 1, AutoFitBehaviorType.AutoFitToContents);

			//Set Column Width
			tbl[0, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
			tbl[0, 0].PreferredWidth = 5.0f;
			tbl[1, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
			tbl[1, 0].PreferredWidth = 5.0f;

			document.InsertText(tbl[0, 0].Range.Start, "Purpose");

			tbl.Rows.Append();
			document.InsertText(tbl[1, 0].Range.Start, "\n" + procedurePurpose);
			tbl.Rows.Append();

			ApplyStyle(FontStyleType.ProcedurePurpose, tbl.FirstRow.Range, document);
			ApplyStyle(FontStyleType.ReportInfo, tbl[1, 0].Range, document);

			foreach (TableRow MyRow in tbl.Rows)
			{
				if (MyRow != tbl.FirstRow)
				{
					foreach (TableCell MyCell in MyRow.Cells)
					{
						MyCell.Borders.Top.LineStyle = TableBorderLineStyle.None;
						if (MyRow != tbl.LastRow)
							MyCell.Borders.Bottom.LineStyle = TableBorderLineStyle.None;
					}
				}
			}
			tbl.ForEachCell(new TableCellProcessorDelegate(SetCellVerticalAlignmentToCenter));
		}

		public static void InsertProcedureConclusion(DevExpress.XtraRichEdit.API.Native.Document document, string procedureConclusion)
		{
			DevExpress.XtraRichEdit.API.Native.Table tbl = document.Tables.Create(document.Range.End, 2, 1, AutoFitBehaviorType.AutoFitToContents);

			//Set Column Width
			tbl[0, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
			tbl[0, 0].PreferredWidth = 5.0f;
			tbl[1, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
			tbl[1, 0].PreferredWidth = 5.0f;

			document.InsertText(tbl[0, 0].Range.Start, "Conclusion");

			tbl.Rows.Append();
			document.InsertText(tbl[1, 0].Range.Start, "\n" + procedureConclusion);
			tbl.Rows.Append();

			ApplyStyle(FontStyleType.Conclusion, tbl.FirstRow.Range, document);
			ApplyStyle(FontStyleType.ReportInfo, tbl[1, 0].Range, document);

			foreach (TableRow MyRow in tbl.Rows)
			{
				if (MyRow != tbl.FirstRow)
				{
					foreach (TableCell MyCell in MyRow.Cells)
					{
						MyCell.Borders.Top.LineStyle = TableBorderLineStyle.None;
						if (MyRow != tbl.LastRow)
							MyCell.Borders.Bottom.LineStyle = TableBorderLineStyle.None;
					}
				}
			}
			tbl.ForEachCell(new TableCellProcessorDelegate(SetCellVerticalAlignmentToCenter));
		}
		public static DevExpress.XtraRichEdit.API.Native.Table InsertMainCommands(DevExpress.XtraRichEdit.API.Native.Document document, GTAReportDM GTAReport)
        {
            #region #CreateTable
            // Insert new table.

            if (GTAReport.MainCommands.Count < 1)
                return null;
            else
                document.AppendSection();

            //Table tbl = document.Tables.Create(document.Range.End, 1, 3, AutoFitBehaviorType.FixedColumnWidth);
            DevExpress.XtraRichEdit.API.Native.Table tbl = document.Tables.Create(document.Range.End, 1, 4, AutoFitBehaviorType.FixedColumnWidth);

            if (GTAReport.enumScope_Type == GTAReportDM.CMD_SCOPE_TYPE.MAIN)
                document.Bookmarks.Create(tbl.Range, "MainCommands");
            else if (GTAReport.enumScope_Type == GTAReportDM.CMD_SCOPE_TYPE.ANNEX)
            {
                AnnexureTableNo++;
                document.Bookmarks.Create(tbl.Range, "Annexure" + AnnexureTableNo.ToString());
            }
            // Create Table header.
            document.InsertText(tbl[0, 0].Range.Start, "Item");
            DocumentRange dRange = document.InsertText(tbl[0, 1].Range.Start, "Line Number");
            dRange = document.InsertText(tbl[0, 2].Range.Start, "Test Description");
            //DocumentRange dRange = document.InsertText(tbl[0, 1].Range.Start, "Test Description");
            if (GTAReport.enumScope_Type == GTAReportDM.CMD_SCOPE_TYPE.ANNEX)
            {
                DevExpress.XtraRichEdit.API.Native.Paragraph p1 = document.Paragraphs.Insert(dRange.End);
                //document.InsertText(p1.Range.Start, GTAReport.getAnnexName());
                DocumentRange dRangeTarget = document.InsertText(p1.Range.Start, "< ");
                AddFormattedStatement(dRangeTarget, GTAReport.getAnnexName(), document);
                DocumentRange dRangeBM = document.CreateRange(dRangeTarget.Start, 2);
                AddBookMark("Target@CallItem", GTAReport.getAnnexName() + GTAReport.getAnnexEpochTime(), dRangeBM, document);
            }

            dRange = document.InsertText(tbl[0, 3].Range.Start, "Test Results");
            //dRange = document.InsertText(tbl[0, 2].Range.Start, "Test Results");
            if (GTAReport.enumScope_Type == GTAReportDM.CMD_SCOPE_TYPE.ANNEX)
            {
                DevExpress.XtraRichEdit.API.Native.Paragraph p1 = document.Paragraphs.Insert(dRange.End);
                //document.InsertText(p1.Range.Start, GTAReport.getAnnexResult());
                AddFormattedResult(GTAReport.getAnnexResult(), p1.Range.Start, document);
            }

            ApplyStyle(FontStyleType.MainCommandsHeader, tbl.FirstRow.Range, document);

            //Set Column Width
            tbl[0, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tbl[0, 0].PreferredWidth = 10.0f;
            tbl[0, 1].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tbl[0, 1].PreferredWidth = 10.0f;
            tbl[0, 2].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tbl[0, 2].PreferredWidth = 60.0f;
            tbl[0, 3].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tbl[0, 3].PreferredWidth = 20.0f;

            // Insert Table data.
            try
            {
                tbl.BeginUpdate();

                int ii = 0;
                foreach (var Cmds in GTAReport.MainCommands)
                {
                    if (!Cmds.Ignored)
                    {

                        ii++;
                        TableRow row = tbl.Rows.Append();

                        TableCell cell = row.FirstCell;
                        DocumentRange r0 = document.InsertText(cell.Range.Start, ii.ToString());

                        //adding column for page number entries
                        DocumentRange r0_1 = document.InsertText(cell.Next.Range.Start, Cmds.cmdLineNumber);

                        //DocumentRange r1 = document.InsertText(cell.Next.Range.Start, Cmds.Statememt);
                        DocumentRange r1 = document.InsertText(cell.Next.Next.Range.Start, Cmds.Statememt);
                        ApplyStyle(FontStyleType.MainTitle, r1, document);

                        if (GTAReport.enumScope_Type == GTAReportDM.CMD_SCOPE_TYPE.MAIN)
                        {
                            //r1 = document.InsertText(cell.Next.Range.Start, "< ");
                            r1 = document.InsertText(cell.Next.Next.Range.Start, "< ");
                            AddBookMark("Target@SummaryItem", Cmds.Statememt, document.CreateRange(r1.Start, 2), document);
                        }

                        //DocumentRange r2 = AddFormattedResult(Cmds.Result, cell.Next.Next.Range.Start, document);
                        DocumentRange r2 = AddFormattedResult(Cmds.Result, cell.Next.Next.Next.Range.Start, document);
                        DocumentRange r3 = null;
                        if (Cmds.ExecTime.time != "" && (Cmds.Result.ToLower() == "ok" || Cmds.Result.ToLower() == "ko" || Cmds.Result.ToLower() == "nok"))
                        {
                            r3 = document.InsertText(r2.End, "  (" + Cmds.ExecTime.time + ")");
                            ApplyStyle(FontStyleType.ResultTimeStamp, r3, document);
                        }

                        row = tbl.Rows.Append();

                        string strPrvCommandType = "";
                        foreach (var Cmd in Cmds.Commands)
                        {
                            if (!Cmd.isIgnored)
                            {
                                row = tbl.Rows.Append();
                                cell = row.FirstCell.Next;

                                //adding linenumber
                                dRange = document.InsertText(cell.Range.Start, Cmd.linenumber);

                                string strCommandType = GetCommandTypeFromEnum(Cmd.cmdType);
                                strCommandType = strCommandType + ":";

                                if (strCommandType != strPrvCommandType)
                                    strPrvCommandType = strCommandType;

                                DocumentRange dRangeStmt = cell.Next.Range;
                                if ((strCommandType.ToLower() == "call:") || (Cmd.name.ToLower() == "parallel call_"))
                                {
                                    dRangeStmt = document.InsertText(cell.Next.Range.Start, "> ");
                                    ApplyStyle(FontStyleType.ActionText, dRangeStmt, document);
                                    DocumentRange dRangeBM = document.CreateRange(dRangeStmt.Start, 2);
                                    AddBookMark("Source@CallItem", Cmd.Statememt + Cmd.execTime.epoch, dRangeBM, document);
                                }
                                else
                                    dRangeStmt = document.InsertText(cell.Next.Range.Start, "");

                                int FormattedStmtLength = AddFormattedStatement(dRangeStmt, Cmd.Statememt, document);
                                if ((Cmd.comment != "") || (Cmd.references != ""))
                                {
                                    DevExpress.XtraRichEdit.API.Native.Paragraph p1 = document.Paragraphs.Insert(dRangeStmt.End);
                                    String comment = Cmd.comment;


                                    //       DocumentRange dRangeReferences = null;
                                    if (Cmd.references != "")
                                    {
                                        String temp = "Ref:\"" + Cmd.references + "\"";
                                        if (Cmd.comment != "")
                                            comment += "\n";
                                        comment += temp;

                                    }
                                    document.InsertText(p1.Range.Start, comment);
                                    ApplyStyle(FontStyleType.DescriptionReference, p1.Range, document);

                                }



                                //Format the Result
                                DocumentRange dRangeResult;
                                if (strCommandType.ToLower() == "action:")
                                    dRangeResult = AddFormattedResult(Cmd.result, cell.Next.Next.Range.Start, document, "Action");
                                else
                                    dRangeResult = AddFormattedResult(Cmd.result, cell.Next.Next.Range.Start, document);

                                DocumentRange dRangeTimeStamp = null;
                                if (Cmd.execTime.time != "" && (Cmd.result.ToLower() == "ok" || Cmd.result.ToLower() == "ko" || Cmd.result.ToLower() == "nok"))
                                {
                                    dRangeTimeStamp = document.InsertText(dRangeResult.End, "  (" + Cmd.execTime.time + ")");
                                    ApplyStyle(FontStyleType.ResultTimeStamp, dRangeTimeStamp, document);

                                    DocumentPosition dPos = dRangeTimeStamp.End;
                                    foreach (Other_Details otherDetails in Cmd.details)
                                    {
                                        bool bParam = true;
                                        string strOtherDetails = "";
                                        DevExpress.XtraRichEdit.API.Native.Paragraph pDetails = document.Paragraphs.Insert(dPos);

                                        String toolTip = "";
                                        foreach (Param_Detail paramDetail in otherDetails.paramDetails)
                                        {
                                            if (paramDetail.name != "NAME")
                                            {
                                                if (!bParam)
                                                {
                                                    strOtherDetails += " ";
                                                    strOtherDetails += paramDetail.name;
                                                    strOtherDetails += " = ";
                                                }
                                                strOtherDetails += paramDetail.value;
                                                if (bParam)
                                                    strOtherDetails += ": ";
                                                else
                                                    strOtherDetails += "; ";
                                            }
                                            else
                                            {
                                                toolTip = paramDetail.value;
                                            }
                                            bParam = false;
                                        }


                                        DocumentRange dOtherDetails = document.InsertText(pDetails.Range.Start, strOtherDetails.Trim());
                                        DevExpress.XtraRichEdit.API.Native.Hyperlink hyperlink = document.Hyperlinks.Create(pDetails.Range.Start, strOtherDetails.Trim().Length);
                                        hyperlink.ToolTip = toolTip;
                                        ApplyStyle(FontStyleType.ActionSplText, pDetails.Range, document);
                                        dPos = dOtherDetails.End;
                                    }
                                }

                                DocumentRange dRangeFeedback = null;
                                if (Cmd.feedback != "" && dRangeTimeStamp != null)
                                {
                                    String temp = "\nUser Feedback:\"" + Cmd.feedback + "\"";
                                    dRangeFeedback = document.InsertText(dRangeTimeStamp.End, temp);
                                    ApplyStyle(FontStyleType.ResultsUserFeedback, dRangeFeedback, document);
                                }


                            }

                        }

                    }
                    //tbl.Rows.Append();
                }

                foreach (TableRow tRow in tbl.Rows)
                {
                    foreach (TableCell tCell in tRow.Cells)
                    {
                        foreach (DevExpress.XtraRichEdit.API.Native.Paragraph p in document.Paragraphs.Get(tCell.Range))
                        {
                            p.SpacingBefore = 8.0f;
                        }
                    }
                }

                tbl.ForEachCell(new TableCellProcessorDelegate(SetCellVerticalAlignmentToCenter));
                tbl.ForEachCell(new TableCellProcessorDelegate(RemoveTableBorderLines));
                SetParagraphLineSpacing(tbl.Range, document);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine("Error while writing Commands in docx!\n");
            }
            finally
            {
                document.Paragraphs.Insert(tbl.Range.End);
                document.Paragraphs.Insert(tbl.Range.End);
                tbl.EndUpdate();
            }

            return tbl;
            #endregion #CreateTable
        }

        public static void SetDocOrientationLandscape(DevExpress.XtraRichEdit.API.Native.Document document)
        {
            //Change the document orientation to Landscape
            foreach (DevExpress.XtraRichEdit.API.Native.Section section in document.Sections)
                section.Page.Landscape = true;
        }

        public static void InsertAttachmentsHead(DevExpress.XtraRichEdit.API.Native.Document document, GTAReportDM GTAReport)
        {
            List<string> listAttachments = new List<string>();

            DocumentRange dRange = document.InsertText(document.Range.End, "ATTACHMENTS");
            ApplyStyle(FontStyleType.HeaderName, dRange, document);
            document.Paragraphs.Insert(dRange.End);
            document.Paragraphs.Insert(dRange.End);
            DevExpress.XtraRichEdit.API.Native.Paragraph para = document.Paragraphs.Insert(dRange.End);

            document.Bookmarks.Create(para.Range, "AttachmentsBody");
        }

        public static void InsertPrintTables(DevExpress.XtraRichEdit.API.Native.Document document, GTAReportDM GTAReport)
        {
            DocumentRange dRange = document.InsertText(document.Range.End, "PRINT TABLES");
            ApplyStyle(FontStyleType.HeaderName, dRange, document);
            document.Paragraphs.Insert(dRange.End);
            document.Paragraphs.Insert(dRange.End);
            DevExpress.XtraRichEdit.API.Native.Paragraph para = document.Paragraphs.Insert(dRange.End);

            //Insert PrintTables
            int PrintTableNo = 0;
            foreach (GTAReportPrintTable objPrintTable in GTAReport.PrintTables)
            {
                PrintTableNo++;
                if (PrintTableNo != 1)
                    document.AppendSection();

                DocumentRange dTableNameRange = document.InsertText(document.Range.End, objPrintTable.TableName);
                ApplyFormattedStyle(dTableNameRange, document, bBold: true, rFontSize: 10, typeUnderline: UnderlineType.Single);
                document.Paragraphs.Insert(dRange.End);
                document.Paragraphs.Insert(dRange.End);

                DevExpress.XtraRichEdit.API.Native.Table tbl = document.Tables.Create(document.Range.End, objPrintTable.ParamTables.Count + 1, objPrintTable.TableHeaders.Count, AutoFitBehaviorType.AutoFitToWindow);

                int columnNo = 0;
                foreach (string columnName in objPrintTable.TableHeaders)
                {
                    document.InsertText(tbl[0, columnNo].Range.Start, columnName);
                    columnNo++;
                }

                ApplyFormattedStyle(tbl.FirstRow.Range, document, ParagraphAlignment.Left, bBold: true);

                if (objPrintTable.ParamTables.Count > 0)
                {
                    int rowNo = 1;
                    foreach (GTAReportParamTable objParamTable in objPrintTable.ParamTables)
                    {
                        columnNo = 3;
                        document.InsertText(tbl[rowNo, 0].Range.Start, objParamTable.ParamName);
                        document.InsertText(tbl[rowNo, 1].Range.Start, objParamTable.ParamType);
                        document.InsertText(tbl[rowNo, 2].Range.Start, objParamTable.ParamUnit);

                        foreach (Other_Details otherDetails in objParamTable.TableDetails)
                        {
                            DocumentRange dTableDetailsRange = tbl[rowNo, columnNo].Range;
                            foreach (Param_Detail pDetail in otherDetails.paramDetails)
                            {
                                string strDetail = pDetail.name + " = " + pDetail.value;
                                dTableDetailsRange = document.InsertText(dTableDetailsRange.Start, strDetail);
                                DevExpress.XtraRichEdit.API.Native.Paragraph newPara = document.Paragraphs.Insert(dTableDetailsRange.End);
                                dTableDetailsRange = newPara.Range;
                            }
                            columnNo++;
                        }
                        rowNo++;
                    }


                    DocumentRange dTableRange = document.CreateRange(tbl[1, 0].Range.Start, tbl[objPrintTable.ParamTables.Count, objPrintTable.TableHeaders.Count - 1].Range.End.ToInt());
                    ApplyFormattedStyle(dTableRange, document, ParagraphAlignment.Left);
                    //document.Bookmarks.Create(tbl.Range, "PrintTable" + PrintTableNo.ToString());
                }

                tbl.ForEachCell(new TableCellProcessorDelegate(SetCellVerticalAlignmentToCenter));
                //tbl.ForEachCell(new TableCellProcessorDelegate(RemoveTableBorderLines));
            }

        }

        public static void InsertDataSources(DevExpress.XtraRichEdit.API.Native.Document document, GTAReportDM GTAReport)
        {
            DocumentRange dRange = document.InsertText(document.Range.End, "DATA SOURCES");
            ApplyStyle(FontStyleType.HeaderName, dRange, document);
            document.Paragraphs.Insert(dRange.End);
            document.Paragraphs.Insert(dRange.End);
            DevExpress.XtraRichEdit.API.Native.Paragraph para = document.Paragraphs.Insert(dRange.End);

            DevExpress.XtraRichEdit.API.Native.Table tbl = document.Tables.Create(document.Range.End, GTAReport.ReportDataSources.GetDataSources().Count + 1, 3, AutoFitBehaviorType.FixedColumnWidth);
            document.InsertText(tbl[0, 0].Range.Start, "Name");
            document.InsertText(tbl[0, 1].Range.Start, "Type");
            document.InsertText(tbl[0, 2].Range.Start, "Path");

            //Set Column Width
            tbl[0, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tbl[0, 0].PreferredWidth = 20.0f;
            tbl[0, 1].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tbl[0, 1].PreferredWidth = 10.0f;
            tbl[0, 2].PreferredWidthType = WidthType.FiftiethsOfPercent;
            tbl[0, 2].PreferredWidth = 70.0f;

            ApplyFormattedStyle(tbl.FirstRow.Range, document, ParagraphAlignment.Left, bBold: true);

            if (GTAReport.ReportDataSources.GetDataSources().Count > 0)
            {
                int RowID = 1;
                foreach (DataSource dataSource in GTAReport.ReportDataSources.GetDataSources())
                {
                    document.InsertText(tbl[RowID, 0].Range.Start, dataSource.fileName);
                    document.InsertText(tbl[RowID, 1].Range.Start, dataSource.fileType);
                    document.InsertText(tbl[RowID, 2].Range.Start, dataSource.filePath);
                    RowID++;
                }

                DocumentRange dTblStyleRange = document.CreateRange(tbl[1, 0].Range.Start, tbl[GTAReport.ReportDataSources.GetDataSources().Count, 2].Range.End.ToInt() - tbl[1, 0].Range.Start.ToInt());
                ApplyFormattedStyle(dTblStyleRange, document, ParagraphAlignment.Left);
            }

            tbl.ForEachCell(new TableCellProcessorDelegate(SetCellVerticalAlignmentToCenter));
        }

        public static void InsertHeaderFooter(DevExpress.XtraRichEdit.API.Native.Document document, GTAReportDM GTAReport, string strFileName)
        {
            Console.WriteLine("Adding Header, Footer");

            // Modify the header of the HeaderFooterType.First type.
            foreach (DevExpress.XtraRichEdit.API.Native.Section MySection in document.Sections)
            {
                //Header
                {
                    SubDocument myHeader = MySection.BeginUpdateHeader(HeaderFooterType.Primary);

                    //Image image = Image.FromFile("C:\\Users\\sadesu\\Documents\\~Work\\GTA\\SVN\\GenericToolForAutomation\\GTA_Data\\LTRA_Templates\\Airbus_LTRA_LOGO.png");
                    DocumentPosition pos = myHeader.CreatePosition(0);
                    Image image = Properties.Resources.Airbus_LTRA_NEW_LOGO;
                    DevExpress.XtraRichEdit.API.Native.Shape imgShape = myHeader.Shapes.InsertPicture(pos, image);
                    //Shape imgShape = myHeader.InsertPicture(pos, image /*DocumentImageSource.FromFile(".\\Tool_Data\\LTRA_Templates\\Airbus_LTRA_LOGO.png")*/);

                    SizeF iSize = imgShape.Size;
                    iSize.Height = 480f;
                    iSize.Width = 480f;
                    imgShape.Size = iSize;

                    imgShape.RelativeHorizontalPosition = ShapeRelativeHorizontalPosition.Margin;
                    imgShape.HorizontalAlignment = ShapeHorizontalAlignment.Left;
                    imgShape.RelativeVerticalPosition = ShapeRelativeVerticalPosition.TopMargin;
                    imgShape.VerticalAlignment = ShapeVerticalAlignment.Center;

                    DocumentRange HeaderRange = myHeader.InsertText(myHeader.CreatePosition(0), "LAB TEST REPORT AND ANALYSIS");
                    ApplyStyle(FontStyleType.HeaderName, HeaderRange, myHeader);

                    DevExpress.XtraRichEdit.API.Native.Paragraph p = myHeader.Paragraphs.Insert(HeaderRange.End);
                    int nTests = GTAReport.SummaryData.SummaryItems.Count;
                    if (nTests < 1) nTests = 1;
                    HeaderRange = myHeader.InsertText(p.Range.Start, "Number of Tests: " + nTests.ToString());
                    ApplyStyle(FontStyleType.HeaderTestsCount, HeaderRange, myHeader);

                    myHeader.Paragraphs.Insert(HeaderRange.End);

                    //Create a dummy table for Bottom Border
                    DevExpress.XtraRichEdit.API.Native.Table tbl = myHeader.Tables.Create(HeaderRange.End, 1, 1, AutoFitBehaviorType.AutoFitToWindow);

                    tbl[0, 0].Borders.Left.LineStyle = TableBorderLineStyle.None;
                    tbl[0, 0].Borders.Right.LineStyle = TableBorderLineStyle.None;
                    tbl[0, 0].Borders.Top.LineStyle = TableBorderLineStyle.None;
                    tbl[0, 0].Borders.Bottom.LineStyle = TableBorderLineStyle.Single;
                    tbl[0, 0].Borders.Bottom.LineThickness = 1.5f;

                    MySection.EndUpdateHeader(myHeader);
                }

                //Footer
                {
                    SubDocument myFooter = MySection.BeginUpdateFooter(HeaderFooterType.Primary);
                    DevExpress.XtraRichEdit.API.Native.Paragraph p = myFooter.Paragraphs.Insert(myFooter.Range.End);

                    DevExpress.XtraRichEdit.API.Native.Table tbl = myFooter.Tables.Create(p.Range.End, 3, 3, AutoFitBehaviorType.AutoFitToWindow);

                    foreach (TableRow MyRow in tbl.Rows)
                    {
                        foreach (TableCell MyCell in MyRow.Cells)
                        {
                            MyCell.Borders.Left.LineStyle = TableBorderLineStyle.None;
                            MyCell.Borders.Right.LineStyle = TableBorderLineStyle.None;
                            MyCell.Borders.Bottom.LineStyle = TableBorderLineStyle.None;

                            if (MyRow == tbl.FirstRow)
                            {
                                MyCell.Borders.Top.LineStyle = TableBorderLineStyle.Single;
                                MyCell.Borders.Top.LineThickness = 1.5f;
                            }
                            else
                                MyCell.Borders.Top.LineStyle = TableBorderLineStyle.None;
                        }
                    }

                    myFooter.InsertText(tbl[1, 0].Range.Start, GTAReport.ReportHeaderData.reportInfo.Date);
                    ApplyStyle(FontStyleType.FooterDate, tbl[1, 0].Range, myFooter);

                    myFooter.InsertText(tbl[1, 1].Range.Start, strFileName /*GetFooterName(GTAReport.ReportHeaderData.OtherDetails)*/);
                    ApplyStyle(FontStyleType.FooterName, tbl[1, 1].Range, myFooter);

                    DevExpress.XtraRichEdit.API.Native.Field fld = myFooter.Fields.Create(tbl[1, 2].Range.Start, "PAGE \\* ARABICDASH");
                    myFooter.Fields.Update();
                    ApplyStyle(FontStyleType.FooterPageNo, tbl[1, 2].Range, myFooter);

                    myFooter.InsertText(tbl[2, 2].Range.Start, "GTA Version Used: " + GTAReport.ReportHeaderData.reportInfo.GTAVersion);
                    ApplyStyle(FontStyleType.FooterPageNo, tbl[2, 2].Range, myFooter);

                    myFooter.Paragraphs.Insert(tbl.Range.End);
                    MySection.EndUpdateFooter(myFooter);
                }
            }
            Console.WriteLine("Header, Footer added Successfully\n");
        }

        public static void InsertHyperlinks(DevExpress.XtraRichEdit.API.Native.Document document)
        {
            Console.WriteLine("Adding Hyperlinks");

            foreach (DevExpress.XtraRichEdit.API.Native.Bookmark bookmark in document.Bookmarks)
            {
                string strSourceBMName = bookmark.Name;
                if (strSourceBMName.Contains("Source@"))
                {
                    string strTargetBMName = "Target@" + strSourceBMName.Substring(strSourceBMName.IndexOf("@") + 1);
                    if (document.Bookmarks[strTargetBMName] != null)
                    {
                        DevExpress.XtraRichEdit.API.Native.Hyperlink hyperlink = document.Hyperlinks.Create(bookmark.Range);

                        hyperlink.Anchor = strTargetBMName;
                        if (strSourceBMName.Contains("CallItem"))
                        {
                            ApplyStyle(FontStyleType.Command, hyperlink.Range, document);
                        }
                    }
                }
                else if (strSourceBMName.Contains("Target@"))
                {
                    string strTargetBMName = "Source@" + strSourceBMName.Substring(strSourceBMName.IndexOf("@") + 1);
                    if (document.Bookmarks[strTargetBMName] != null)
                    {
                        DevExpress.XtraRichEdit.API.Native.Hyperlink hyperlink = document.Hyperlinks.Create(bookmark.Range);
                        hyperlink.Anchor = strTargetBMName;
                    }
                }

            }
            Console.WriteLine("Hyperlinks added Successfully\n");
        }

        static void SetCellVerticalAlignmentToCenter(TableCell cell, int i, int j)
        {
            cell.VerticalAlignment = TableCellVerticalAlignment.Center;
        }

        static void RemoveTableBorderLines(TableCell MyCell, int i, int j)
        {
            MyCell.Borders.Top.LineStyle = TableBorderLineStyle.None;
            MyCell.Borders.Bottom.LineStyle = TableBorderLineStyle.None;
            MyCell.Borders.Left.LineStyle = TableBorderLineStyle.None;
            MyCell.Borders.Right.LineStyle = TableBorderLineStyle.None;
        }

        private static string GetFooterName(List<OtherInfo> OtherDetails)
        {
            string strFooterName = "";

            string strAircraftName = "NA";
            string strEquipmentName = "NA";
            string strStandardName = "NA";

            foreach (OtherInfo Info in OtherDetails)
            {
                if (Info.item.ToLower() == "aircraft")
                    strAircraftName = Info.value;
                else if (Info.item.ToLower() == "equipment")
                    strEquipmentName = Info.value;
                else if (Info.item.ToLower() == "standard")
                    strStandardName = Info.value;
            }

            strFooterName = strAircraftName + " / " + strEquipmentName + " | " + strStandardName;

            return strFooterName;
        }

        public static void ApplyStyle(FontStyleType Styler, DocumentRange Range, SubDocument doc, Color color = default(Color))
        {
            doc.BeginUpdate();
            foreach (DevExpress.XtraRichEdit.API.Native.Paragraph p in doc.Paragraphs.Get(Range))
            {
                p.LeftIndent = 0.0f;
                p.Alignment = ParagraphAlignment.Left;

                CharacterProperties cp = doc.BeginUpdateCharacters(Range);
                cp.Bold = false;
                cp.Underline = UnderlineType.None;
                cp.Italic = false;
                cp.AllCaps = false;
                cp.FontName = "Arial";
                cp.FontSize = 8;
                cp.ForeColor = Color.Black;

                switch (Styler)
                {
                    case FontStyleType.None:
                        {
                        }
                        break;
                    case FontStyleType.Bold:
                        {
                            cp.Bold = true;
                        }
                        break;
                    case FontStyleType.Italic:
                        {
                            cp.Italic = true;
                        }
                        break;
                    case FontStyleType.HeaderName:
                        {
                            cp.FontSize = 18;
                            p.Alignment = ParagraphAlignment.Center;
                        }
                        break;
                    case FontStyleType.HeaderTestsCount:
                        {
                            cp.FontSize = 8;
                            p.Alignment = ParagraphAlignment.Right;
                        }
                        break;
                    case FontStyleType.FooterDate:
                        {
                            cp.FontName = "Calibri";
                            cp.FontSize = 11;
                        }
                        break;
                    case FontStyleType.FooterName:
                        {
                            cp.FontName = "Calibri";
                            cp.FontSize = 11;
                            cp.Italic = true;
                            p.Alignment = ParagraphAlignment.Center;
                        }
                        break;
                    case FontStyleType.FooterPageNo:
                        {
                            cp.FontName = "Calibri";
                            cp.FontSize = 11;
                            p.Alignment = ParagraphAlignment.Right;
                        }
                        break;
                    case FontStyleType.ReportInfo:
                        {
                            cp.FontSize = 10;
                        }
                        break;
                    case FontStyleType.ReportInfoValues:
                        {
                            cp.Bold = true;
                            cp.FontSize = 10;

                        }
                        break;
                    case FontStyleType.TestDetails:
                        {
                            cp.Bold = true;
                            p.Alignment = ParagraphAlignment.Center;
                        }
                        break;
                    case FontStyleType.SpecificationsHeader:
                        {
                            cp.Bold = true;
                            p.Alignment = ParagraphAlignment.Center;
                        }
                        break;
                    case FontStyleType.OtherDetailsHeader:
                        {
                            cp.Bold = true;
                            p.Alignment = ParagraphAlignment.Center;
                        }
                        break;
                    case FontStyleType.SummaryHeader:
                        {
                            cp.Bold = true;
                            p.Alignment = ParagraphAlignment.Center;
                        }
                        break;
                    case FontStyleType.MainCommandsHeader:
                        {
                            cp.Bold = true;
                            p.Alignment = ParagraphAlignment.Center;
                        }
                        break;
                    case FontStyleType.ItemName:
                        {
                            cp.Bold = true;
                            cp.Underline = UnderlineType.Dashed;
                        }
                        break;
                    case FontStyleType.MainTitle:
                        {
                            cp.AllCaps = true;
                            cp.Bold = true;
                            cp.Underline = UnderlineType.Single;
                        }
                        break;
                    case FontStyleType.MainResult:
                        {
                            cp.Bold = true;
                        }
                        break;
                    case FontStyleType.ActionTitle:
                        {
                            cp.AllCaps = true;
                            cp.Underline = UnderlineType.Single;
                        }
                        break;
                    case FontStyleType.ActionText:
                        {
                        }
                        break;
                    case FontStyleType.Command:
                        {

                            cp.FontName = "Arial Black";
                            cp.ForeColor = Color.Gray;
                            cp.Bold = true;
                        }
                        break;
                    case FontStyleType.ActionSplText:
                        {
                            cp.Italic = true;
                        }
                        break;
                    case FontStyleType.ActionResult:
                        {
                            cp.ForeColor = Color.Green;
                            cp.FontName = "Wingdings";
                            cp.FontSize = 12.0f;
                        }
                        break;
                    case FontStyleType.ActionResultKO:
                        {
                            cp.ForeColor = Color.Red;
                            cp.FontName = "Wingdings";
                            cp.FontSize = 12.0f;
                        }
                        break;
                    case FontStyleType.CheckTitle:
                        {
                            cp.AllCaps = true;
                            cp.Underline = UnderlineType.Single;
                        }
                        break;
                    case FontStyleType.CheckText:
                        {
                            p.LeftIndent = 0.5f;
                        }
                        break;
                    case FontStyleType.CheckResult:
                        {
                            cp.Bold = true;
                            cp.ForeColor = Color.Green;
                        }
                        break;
                    case FontStyleType.CheckResultKO:
                        {
                            cp.Bold = true;
                            cp.ForeColor = Color.Red;
                        }
                        break;
                    case FontStyleType.ResultNA:
                        {
                            cp.Bold = true;
                            cp.ForeColor = Color.Gray;
                        }
                        break;
                    case FontStyleType.ResultTimeStamp:
                        {
                            cp.Italic = true;
                        }
                        break;
                    case FontStyleType.ResultsUserFeedback:
                    case FontStyleType.DescriptionReference:
                        {
                            cp.Italic = true;
                            cp.ForeColor = Color.SlateGray;
                        }
                        break;

                    case FontStyleType.Strikeout:
                        {
                            cp.Strikeout = DevExpress.XtraRichEdit.API.Native.StrikeoutType.Single;
                        }
                        break;

                    case FontStyleType.ColorAction:
                        {
                            cp.Bold = true;
                            cp.ForeColor = color;
                        }
                        break;

                    case FontStyleType.ProcedurePurpose:
                        {
							cp.FontSize = 14;
							p.Alignment = ParagraphAlignment.Center;
						}
                        break;

					case FontStyleType.Conclusion:
						{
							cp.FontSize = 14;
							p.Alignment = ParagraphAlignment.Center;
						}
						break;
					default:
                        break;
                }

                doc.EndUpdateCharacters(cp);
            }
            doc.EndUpdate();
        }

        public static void ApplyFormattedStyle(DocumentRange Range, SubDocument doc, ParagraphAlignment pAlignment = ParagraphAlignment.Left, string rFontName = "Arial", int rFontSize = 8,
            bool bBold = false, bool bItalic = false, bool bAllCaps = false, UnderlineType typeUnderline = UnderlineType.None, Color rColor = default(Color), Color bColor = default(Color))
        {
            if (object.Equals(rColor, default(Color))) rColor = Color.Black;

            doc.BeginUpdate();
            ParagraphProperties pp = doc.BeginUpdateParagraphs(Range);
            pp.Alignment = ParagraphAlignment.Center;
            pp.LeftIndent = DevExpress.Office.Utils.Units.InchesToDocumentsF(0.05f);
            pp.Alignment = pAlignment;
            pp.BackColor = bColor;
            pp.Style.ForeColor = rColor;
            pp.Style.FontSize = rFontSize;
            pp.Style.Underline = typeUnderline;
            pp.Style.Bold = bBold;
            pp.Style.Italic = bItalic;
            pp.Style.FontName = rFontName;
            doc.EndUpdateParagraphs(pp);
            doc.EndUpdate();
        }

        public static string GetCommandTypeFromEnum(CommandType cmdType)
        {
            string strCommandType = "";

            switch (cmdType)
            {
                case CommandType.ACTION:
                    strCommandType = "ACTION";
                    break;
                case CommandType.CHECK:
                    strCommandType = "CHECK";
                    break;
                case CommandType.CALL:
                    strCommandType = "CALL";
                    break;
                case CommandType.TITLE:
                    strCommandType = "TITLE";
                    break;
                default:
                    strCommandType = "ACTION";
                    break;
            }

            return strCommandType;
        }

        static void AddBookMark(string strHead, string strBody, DocumentRange dRange, DevExpress.XtraRichEdit.API.Native.Document document)
        {
            strBody = strBody.Replace(" ", "_");
            string strBMName = strHead + strBody;
            if (strBMName.Length > 40)
            {
                strBody = String.Format("{0:X}", strBody.GetHashCode());
                strBMName = strHead + strBody;
            }

            if (document.Bookmarks[strBMName] == null)
                document.Bookmarks.Create(dRange, strBMName);
        }

        static int AddFormattedStatement(DocumentRange iDRange, string strCmdStatement, DevExpress.XtraRichEdit.API.Native.Document document, string strPos = "End")
        {
            string strNormal = "";
            string strBold = "";
            string strCommandType = "";
            string strMessage = "";
            string strStatement = strCmdStatement;
            int StatementLength = 0;

            DocumentRange dRange = document.CreateRange(iDRange.Start, iDRange.Length);
            DocumentPosition dPos = dRange.End;
            if (strPos == "Start")
                dPos = dRange.Start;

            while (strStatement != "")
            {
                strNormal = "";
                strBold = "";
                strCommandType = "";
                strMessage = "";
                int hashbIndex = strStatement.IndexOf("#b#");
                int hashcIndex = strStatement.IndexOf("#c#");
                int hashColorIndex = strStatement.IndexOf("#color#");
                if (hashbIndex >= 0 || hashcIndex >= 0 || hashColorIndex >= 0)
                {
                    string strKey = "";
                    // if (hashcIndex >= 0)
                    // {
                    //     strKey = "#c#";
                    //     if (hashbIndex >= 0 && hashcIndex > hashbIndex)
                    //         strKey = "#b#";
                    //}
                    //else if(hashColorIndex >=0)
                    //{
                    //    strKey = "#color#";
                    //    if (hashbIndex >= 0 && hashColorIndex > hashbIndex)
                    //        strKey = "#b#";
                    //}
                    //else
                    //     strKey = "#b#";

                    //check for the first occurence of relevant marker. Use the appropriate strKey and write into the document accordingly.
                    if ((hashcIndex >= 0) && ((hashbIndex == -1) ? true : hashcIndex < hashbIndex) && ((hashColorIndex == -1) ? true : hashcIndex < hashColorIndex))
                    {
                        strKey = "#c#";
                    }
                    else if (hashColorIndex >= 0 && ((hashbIndex == -1) ? true : hashColorIndex < hashbIndex) && ((hashcIndex == -1) ? true : hashColorIndex < hashcIndex))
                    {
                        strKey = "#color#";
                    }
                    else
                        strKey = "#b#";

                    strNormal = strStatement.Substring(0, strStatement.IndexOf(strKey));
                    strStatement = strStatement.Substring(strStatement.IndexOf(strKey) + strKey.Length);
                    StatementLength = StatementLength + strNormal.Length;

                    if (strStatement.IndexOf(strKey) >= 0)
                    {
                        if (strKey == "#c#")
                            strCommandType = strStatement.Substring(0, strStatement.IndexOf(strKey));
                        else if (strKey == "#color#")
                            strMessage = strStatement.Substring(0, strStatement.IndexOf(strKey));
                        else
                            strBold = strStatement.Substring(0, strStatement.IndexOf(strKey));

                        strStatement = strStatement.Substring(strStatement.IndexOf(strKey) + strKey.Length);
                        StatementLength = StatementLength + strCommandType.Length;
                    }
                    else
                    {
                        StatementLength = StatementLength + strStatement.Length;
                        if (strKey == "#b#")
                            strBold = strStatement;
                        else if (strKey == "#c#")
                            strCommandType = strStatement;
                        else if (strKey == "#color#")
                            strMessage = strStatement;
                        strStatement = "";
                    }
                }
                else
                {
                    strNormal = strStatement;
                    strStatement = "";
                    StatementLength = StatementLength + strNormal.Length;
                }

                if (strNormal != "")
                {
                    dRange = document.InsertText(dPos, strNormal);
                    ApplyStyle(FontStyleType.ActionText, dRange, document);
                    ApplyFormattedStyle(dRange, document, rFontName: "Arial", rColor: Color.Black, bColor: Color.White);
                }

                if (strBold != "")
                {
                    dRange = document.InsertText(dRange.End, strBold);
                    ApplyStyle(FontStyleType.Bold, dRange, document);
                }
                if (strMessage != "")
                {
                    Color CmdColor = Color.FromArgb(0, 255, 194, 0);
                    if (strMessage.Contains("#amber#"))
                    {
                        strMessage = strMessage.Replace("#amber#", "");
                        CmdColor = Color.FromArgb(190, 150, 0);
                    }
                    else if (strMessage.Contains("#green#"))
                    {
                        strMessage = strMessage.Replace("#green#", "");
                        CmdColor = Color.Green;
                    }
                    else if (strMessage.Contains("#red#"))
                    {
                        strMessage = strMessage.Replace("#red#", "");
                        CmdColor = Color.Red;
                    }
                    else if (strMessage.Contains("#cyan#"))
                    {
                        strMessage = strMessage.Replace("#cyan#", "");
                        CmdColor = Color.Cyan;
                    }
                    else if (strMessage.Contains("#white#"))
                    {
                        strMessage = strMessage.Replace("#white#", "");
                        CmdColor = Color.White;
                    }
                    else if (strMessage.Contains("#darkcyan#"))
                    {
                        strMessage = strMessage.Replace("#darkcyan#", "");
                        CmdColor = Color.FromArgb(0, 140, 255);
                    }

                    dRange = document.InsertText(dRange.End, strMessage);
                    if (CmdColor != Color.White)
                    {
                        ApplyFormattedStyle(dRange, document, rFontName: "Arial", rColor: CmdColor, bColor: Color.White);
                    }
                    else
                    {
                        ApplyFormattedStyle(dRange, document, rFontName: "Arial", rColor: CmdColor, bColor: Color.Black);
                        CmdColor = Color.Black;
                    }
                    ApplyStyle(FontStyleType.ColorAction, dRange, document, CmdColor);
                }
                if (strCommandType != "")
                {
                    dRange = document.InsertText(dRange.End, strCommandType);
                    Color CmdColor = Color.Gray;
                    if (strCommandType.ToLower() == "check")
                        CmdColor = Color.Green;
                    ApplyFormattedStyle(dRange, document, rFontName: "Arial Black", rColor: CmdColor, bColor: Color.White);
                }

                dPos = dRange.End;
            }

            return StatementLength;
        }

        static DocumentRange AddFormattedResult(string strResult, DocumentPosition dPosition, DevExpress.XtraRichEdit.API.Native.Document document, string strResultType = "")
        {
            if (strResult == null || strResult == "")
                strResult = "NA";

            DocumentRange dRange;
            if (strResultType == "Action")
            {
                if (strResult.ToLower() == "ok")
                {
                    char testStatus = (char)0252;
                    dRange = document.InsertText(dPosition, testStatus.ToString());
                    ApplyStyle(FontStyleType.ActionResult, dRange, document);
                }
                else if (strResult.ToLower() == "ko" || strResult.ToLower() == "nok")
                {
                    char testStatus = (char)0251;
                    dRange = document.InsertText(dPosition, testStatus.ToString());
                    ApplyStyle(FontStyleType.ActionResultKO, dRange, document);
                }
                else
                {

                    dRange = document.InsertText(dPosition, strResult);
                    ApplyStyle(FontStyleType.ResultNA, dRange, document);
                }
            }
            else
            {
                dRange = document.InsertText(dPosition, strResult);
                if (strResult.ToLower() == "ok")
                    ApplyStyle(FontStyleType.CheckResult, dRange, document);
                else if (strResult.ToLower() == "ko" || strResult.ToLower() == "nok")
                    ApplyStyle(FontStyleType.CheckResultKO, dRange, document);
                else
                    ApplyStyle(FontStyleType.ResultNA, dRange, document);
            }

            return dRange;
        }
        static bool AddFormattedText(string strText, DocumentPosition dPosition, DevExpress.XtraRichEdit.API.Native.Document document, string strResultType = "")
        {
            if (strText == null || strText == "")
                strText = "NA";

            int IndexOf = 0;
            string strInBlack = "";
            string strInColor = "";

            DocumentRange dRangeInColor;
            DocumentRange dRangeInBlack;

            if (strText.Contains("OK"))
            {                
                IndexOf = strText.IndexOf("OK");
                strInBlack = strText.Remove(IndexOf);

                dRangeInBlack = document.InsertText(dPosition, strInBlack);
                ApplyStyle(FontStyleType.SummaryHeader, dRangeInBlack, document);

                strInColor = "OK";
                dRangeInColor = document.InsertText(dPosition, strInColor);
                ApplyStyle(FontStyleType.CheckResult, dRangeInColor, document);
            }

            if (strText.Contains("KO"))
            {
                IndexOf = strText.IndexOf("KO");
                strInBlack = strText.Remove(IndexOf);

                dRangeInBlack = document.InsertText(dPosition, strInBlack);
                ApplyStyle(FontStyleType.SummaryHeader, dRangeInBlack, document);

                strInColor = "KO";
                dRangeInColor = document.InsertText(dPosition, strInColor);
                ApplyStyle(FontStyleType.CheckResultKO, dRangeInColor, document);
            }

            if (strText.Contains("NA"))
            {
                IndexOf = strText.IndexOf("NA");
                strInBlack = strText.Remove(IndexOf);

                dRangeInBlack = document.InsertText(dPosition, strInBlack);
                ApplyStyle(FontStyleType.SummaryHeader, dRangeInBlack, document);

                strInColor = "NA";
                dRangeInColor = document.InsertText(dPosition, strInColor);
                ApplyStyle(FontStyleType.ResultNA, dRangeInColor, document);
            }
            return true;   
        }
        static void ApplyFontSize(DocumentRange dRange, int nFontSize, DevExpress.XtraRichEdit.API.Native.Document doc)
        {
            CharacterProperties cp = doc.BeginUpdateCharacters(dRange);
            cp.FontSize = nFontSize;
            doc.EndUpdateCharacters(cp);
        }

        static void SetParagraphLineSpacing(DocumentRange dRange, DevExpress.XtraRichEdit.API.Native.Document document)
        {
            ParagraphProperties pp = document.BeginUpdateParagraphs(dRange);
            pp.LineSpacingType = ParagraphLineSpacing.Multiple;
            pp.LineSpacingMultiplier = 1.5f;
            document.EndUpdateParagraphs(pp);
        }


        public static void insertHeaderData(DevExpress.XtraRichEdit.API.Native.Document document, GTAReportHeader docHeader, String fileName)
        {

            DocumentRange dRange = document.InsertText(document.Range.End, fileName);
            ApplyFormattedStyle(document.Range, document, ParagraphAlignment.Center, "Arial", 10, true, false, false);
            document.Paragraphs.Insert(dRange.End);
            DevExpress.XtraRichEdit.API.Native.Paragraph para = document.Paragraphs.Insert(dRange.End);


            List<OtherInfo> lstTestDetails = docHeader.TestDetails;
            int rowCount = lstTestDetails.Count;
            const int colCount = 2;

            DevExpress.XtraRichEdit.API.Native.Table tbl = document.Tables.Create(document.Range.End, rowCount, colCount, AutoFitBehaviorType.AutoFitToContents);
            ApplyFormattedStyle(tbl.Range, document, ParagraphAlignment.Justify, "Arial", 8, false, false);


            int count = 0;
            foreach (OtherInfo obj in lstTestDetails)
            {
                document.InsertText(tbl[count, 0].Range.Start, obj.item);
                document.InsertText(tbl[count, 1].Range.Start, obj.value);
                count++;
            }

            document.Paragraphs.Insert(dRange.End);
            document.Paragraphs.Insert(dRange.End);
            DevExpress.XtraRichEdit.API.Native.Paragraph para1 = document.Paragraphs.Insert(dRange.End);

        }

        public static void insertCommands(DevExpress.XtraRichEdit.API.Native.Document document, List<Command> lstCommands)
        {

            int rowCnt = lstCommands.Count;
            if (rowCnt > 0)
            {

                const int colcnt = 8;

                DevExpress.XtraRichEdit.API.Native.Table tbl = document.Tables.Create(document.Range.End, rowCnt + 1, colcnt, AutoFitBehaviorType.FixedColumnWidth);
                ApplyFormattedStyle(tbl.Range, document, ParagraphAlignment.Justify, "Arial", 8);

                tbl.BeginUpdate();
                tbl[0, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
                tbl[0, 0].PreferredWidth = 5.0f;

                tbl[0, 1].PreferredWidthType = WidthType.FiftiethsOfPercent;
                tbl[0, 1].PreferredWidth = 35.0f;

                tbl[0, 2].PreferredWidthType = WidthType.FiftiethsOfPercent;
                tbl[0, 2].PreferredWidth = 10.0f;

                tbl[0, 3].PreferredWidthType = WidthType.FiftiethsOfPercent;
                tbl[0, 3].PreferredWidth = 10.0f;

                tbl[0, 4].PreferredWidthType = WidthType.FiftiethsOfPercent;
                tbl[0, 4].PreferredWidth = 10.0f;

                tbl[0, 5].PreferredWidthType = WidthType.FiftiethsOfPercent;
                tbl[0, 5].PreferredWidth = 10.0f;

                tbl[0, 6].PreferredWidthType = WidthType.FiftiethsOfPercent;
                tbl[0, 6].PreferredWidth = 10.0f;

                tbl[0, 7].PreferredWidthType = WidthType.FiftiethsOfPercent;
                tbl[0, 7].PreferredWidth = 10.0f;

                tbl.EndUpdate();

                int count = 0;

                ApplyFormattedStyle(tbl.FirstRow.Range, document, ParagraphAlignment.Justify, "Arial", 8, true, false, true, UnderlineType.None, Color.Black, Color.FromArgb(250, 191, 143));
                document.InsertText(tbl[count, 0].Range.Start, "LINE NO.");
                document.InsertText(tbl[count, 1].Range.Start, "ACTION");
                document.InsertText(tbl[count, 2].Range.Start, "TYPE");
                document.InsertText(tbl[count, 3].Range.Start, "COMMENT");
                document.InsertText(tbl[count, 4].Range.Start, "TIME OUT");
                document.InsertText(tbl[count, 5].Range.Start, "PRECISION");
                document.InsertText(tbl[count, 6].Range.Start, "ACTION ON FAIL");
                document.InsertText(tbl[count, 7].Range.Start, "DUMP LIST");

                tbl.BeginUpdate();
                highlightTableRow(tbl.FirstRow, colcnt, Color.FromArgb(250, 191, 143));
                tbl.EndUpdate();


                count++;
                foreach (Command cmd in lstCommands)
                {

                    TableRow tblRow = tbl.Rows[count];

                    tbl.BeginUpdate();
                    ApplyFormattedStyle(tblRow.Range, document, ParagraphAlignment.Justify, "Arial", 8, false, false, false, UnderlineType.None, Color.Black, Color.White);
                    highlightTableRow(tblRow, colcnt, Color.White);

                    if (cmd.isIgnored == true)
                    {

                        ApplyFormattedStyle(tblRow.Range, document, ParagraphAlignment.Justify, "Arial", 8, true, false, false, UnderlineType.None, Color.FromArgb(79, 98, 40), Color.FromArgb(217, 149, 148));
                        highlightTableRow(tblRow, colcnt, Color.FromArgb(217, 149, 148));
                        ApplyStyle(FontStyleType.Strikeout, tblRow.Range, document);

                    }
                    else
                    {
                        if (cmd.cmdType == CommandType.CHECK)
                        {
                            ApplyFormattedStyle(tblRow.Range, document, ParagraphAlignment.Justify, "Arial", 8, false, false, false, UnderlineType.None, Color.FromArgb(0, 128, 58), Color.White);
                            highlightTableRow(tblRow, colcnt, Color.White);

                        }
                        else
                        {
                            if (cmd.subCmdType == SubCommandType.TITLE)
                            {
                                ApplyFormattedStyle(tblRow.Range, document, ParagraphAlignment.Justify, "Arial", 8, false, false, false, UnderlineType.None, Color.Black, Color.FromArgb(141, 179, 226));
                                highlightTableRow(tblRow, colcnt, Color.FromArgb(141, 179, 226));

                            }
                            else if (cmd.subCmdType == SubCommandType.PRINT)
                            {
                                ApplyFormattedStyle(tblRow.Range, document, ParagraphAlignment.Justify, "Arial", 8, false, false, false, UnderlineType.None, Color.FromArgb(87, 134, 190), Color.White);
                                highlightTableRow(tblRow, colcnt, Color.White);
                            }
                            else if (cmd.subCmdType == SubCommandType.CONDITION)
                            {
                                ApplyFormattedStyle(tblRow.Range, document, ParagraphAlignment.Justify, "Arial", 8, false, false, false, UnderlineType.None, Color.FromArgb(227, 108, 31), Color.White);
                                highlightTableRow(tblRow, colcnt, Color.White);
                            }
                            else
                            {
                                ApplyFormattedStyle(tblRow.Range, document, ParagraphAlignment.Justify, "Arial", 8, false, false, false, UnderlineType.None, Color.Black, Color.White);
                                highlightTableRow(tblRow, colcnt, Color.White);
                            }
                        }


                    }
                    tbl.EndUpdate();

                    document.InsertText(tbl[count, 0].Range.Start, count.ToString());
                    document.InsertText(tbl[count, 1].Range.Start, cmd.Statememt);

                    String type = (cmd.cmdType == CommandType.ACTION) ? "Action" : "Check";

                    document.InsertText(tbl[count, 2].Range.Start, type);
                    document.InsertText(tbl[count, 3].Range.Start, cmd.comment);

                    String timeout = cmd.timeout;
                    if (timeout == "")
                        timeout = "--";
                    else
                        timeout = timeout + " " + cmd.timeoutUnit;
                    document.InsertText(tbl[count, 4].Range.Start, timeout);


                    String precision = cmd.precision;
                    if (precision == "")
                        precision = "--";
                    else
                        precision = precision + " " + cmd.precisionUnit;

                    document.InsertText(tbl[count, 5].Range.Start, precision);


                    document.InsertText(tbl[count, 6].Range.Start, cmd.actionOnFail);
                    document.InsertText(tbl[count, 7].Range.Start, cmd.dumpList);

                    count++;
                }



            }

        }


        public static void highlightTableRow(TableRow tableRow, int totalColumns, Color color)
        {

            for (int i = 0; i < totalColumns; i++)
            {
                tableRow.Cells[i].BackgroundColor = color;
            }
        }

        public static void InsertHeaderFooter(DevExpress.XtraRichEdit.API.Native.Document document, string strFileName)
        {
            Console.WriteLine("Adding Page number");

            DevExpress.XtraRichEdit.API.Native.Section firstSection = document.Sections[0];
            firstSection.PageNumbering.ContinueNumbering = false;
            firstSection.PageNumbering.NumberingFormat = NumberingFormat.CardinalText;

            SubDocument myHeader = firstSection.BeginUpdateFooter(HeaderFooterType.Primary);
            DocumentRange range = myHeader.InsertText(myHeader.CreatePosition(0), "Page ");
            DevExpress.XtraRichEdit.API.Native.Field field = myHeader.Fields.Create(range.End, "PAGE \\* ARABICDASH");
            myHeader.Fields.Update();
            firstSection.EndUpdateFooter(myHeader);

            firstSection.Margins.Left = 225;
            firstSection.Margins.Right = 225;
            firstSection.Margins.Top = 300;
            firstSection.Margins.Bottom = 300;

            document.Fields.Update();

            //Console.WriteLine("Page number added Successfully\n");
        }

        /*
         *@brief This function read a .txt file that give list of tools with them versions. 
         *      She write these informations in the LTRA document. 
         *@param Document, it's a object that contains description of redaction of LTRA
         *@param strFileVersionsToolsBench, contains path of the .txt file. 
         */
        public static void InsertVersionsToolsBench(DevExpress.XtraRichEdit.API.Native.Document document, GTAReportDM GTAReport, string strFileVersionsToolsBench)
        {
            if (strFileVersionsToolsBench != "")
            {
                String line;
                Dictionary<string, string> listVersionToolsBench = new Dictionary<string, string>();
                try
                {
                    StreamReader reader = new StreamReader(strFileVersionsToolsBench);
                    line = reader.ReadLine();
                    while (line != null)
                    {
                        string[] tabNameAndVersion;
                        tabNameAndVersion = line.Split(new[] { " - " }, StringSplitOptions.RemoveEmptyEntries);
                        if (tabNameAndVersion.Length > 1)
                        {
                            listVersionToolsBench.Add(tabNameAndVersion[0], tabNameAndVersion[1]);
                        }
                        else
                        {
                            listVersionToolsBench.Add(tabNameAndVersion[0], "UNKNOW VERSION");
                        }
                        line = reader.ReadLine();
                    }
                    reader.Close();

                }
                catch (Exception e)
                {
                    Console.WriteLine("Exception: " + e.Message);
                }
                finally
                {
                    Console.WriteLine("Executing finally block.");
                }
                DevExpress.XtraRichEdit.API.Native.Table tbl = document.Tables.Create(document.Range.End, listVersionToolsBench.Count + 1, 2, AutoFitBehaviorType.FixedColumnWidth);
                document.InsertText(tbl[0, 0].Range.Start, "Name Tools");
                document.InsertText(tbl[0, 1].Range.Start, "Versions");
                tbl[0, 0].PreferredWidthType = WidthType.FiftiethsOfPercent;
                tbl[0, 0].PreferredWidth = 50.0f;
                tbl[0, 1].PreferredWidthType = WidthType.FiftiethsOfPercent;
                tbl[0, 1].PreferredWidth = 50.0f;

                if (listVersionToolsBench.Count > 0)
                {
                    int RowID = 1;
                    ApplyFormattedStyle(tbl.FirstRow.Range, document, ParagraphAlignment.Center, rFontName: "Arial", bBold: true);
                    foreach (KeyValuePair<string, string> kvp in listVersionToolsBench)
                    {
                        document.InsertText(tbl[RowID, 0].Range.Start, kvp.Key);
                        document.InsertText(tbl[RowID, 1].Range.Start, kvp.Value);
                        RowID++;
                    }
                }
                tbl.ForEachCell(new TableCellProcessorDelegate(SetCellVerticalAlignmentToCenter));
            }
            else
            {
                Console.WriteLine("No file versions tools bench give in parameter");
            }
        }
    }
}

