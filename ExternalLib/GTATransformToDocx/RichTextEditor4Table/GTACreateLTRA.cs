using System;
using System.IO;

using DevExpress.XtraRichEdit;
using DevExpress.XtraRichEdit.API.Native;


namespace GTATransformToDocx
{
    class GTACreateLTRA
    {
        public static void CreateLTRA(string strXMLPath, string strDocumentPath, string milliSec, string strFileVersionsToolsBench)
        {
            try
            {

                bool isMilliSec = milliSec.Contains("true") ? true : false;
                FileInfo fileInfo = new FileInfo(strXMLPath);
                strXMLPath = fileInfo.FullName;

                fileInfo = new FileInfo(strDocumentPath);
                strDocumentPath = fileInfo.FullName;
                string strFileName = fileInfo.Name;
                strFileName = strFileName.Substring(0, strFileName.IndexOf("."));

                GTAXMLReportParser xmlParser = new GTAXMLReportParser();
                GTAReportDM GTAReport = xmlParser.parseReportFile(strXMLPath, isMilliSec);

                //Default Document Formatting
                DevExpress.XtraRichEdit.RichEditControlCompatibility.DefaultFontSize = 8;
                DevExpress.XtraRichEdit.RichEditControlCompatibility.DefaultFontName = "Arial";

                //Create new Rich Edit Document
                RichEditControl richEditControl = new RichEditControl();
                richEditControl.CreateNewDocument(false);
                Document doc = richEditControl.Document;


                doc.SaveDocument(strDocumentPath, DevExpress.XtraRichEdit.DocumentFormat.OpenXml);

                //Create Header Data - includes ReportInfo, Test Details, Specifications, Other Details
                Console.WriteLine("Adding Header");
                GTARichTextEditorUtils.InsertReportHeaderData(doc, GTAReport.ReportHeaderData);
                Console.WriteLine("Header data added Successfully!\n");

                if (xmlParser.procedurePurposeNodePresent(strXMLPath))
                {
					Console.WriteLine("Adding Procedure Purpose");
					doc.AppendSection();
					GTARichTextEditorUtils.InsertProcedurePurpose(doc, GTAReport.procedurePurpose);
					Console.WriteLine("Procedure Purpose added Successfully!\n");

					Console.WriteLine("Adding Procedure Conclusion");
					doc.AppendSection();
					GTARichTextEditorUtils.InsertProcedureConclusion(doc, GTAReport.procedureConclusion);
					Console.WriteLine("Procedure Conclusion added Successfully!\n");
				}

				//Create Summary Data in a new section
				Console.WriteLine("Adding Summary");
                doc.AppendSection();
                GTARichTextEditorUtils.InsertReportSummaryData(doc, GTAReport.SummaryData);
                Console.WriteLine("Summary added Successfully!\n");

                Console.WriteLine("Add versions");
                doc.AppendSection();
                GTARichTextEditorUtils.InsertVersionsToolsBench(doc, GTAReport, strFileVersionsToolsBench);
                Console.WriteLine("versions added Successfully!\n");

                //Create Main Commands in a new section
                Console.WriteLine("Adding Main Commands");
                GTARichTextEditorUtils.InsertMainCommands(doc, GTAReport);
                Console.WriteLine("Main Commands added Successfully!\n");

                //Create Section for Print-Tables
                if (GTAReport.PrintTables.Count > 0)
                {
                    Console.WriteLine("Adding Print-Tables");
                    doc.AppendSection();
                    GTARichTextEditorUtils.InsertPrintTables(doc, GTAReport);
                    Console.WriteLine("Print-Tables added Successfully!\n");
                }

                //Create Section for Attachments
                doc.AppendSection();
                GTARichTextEditorUtils.InsertAttachmentsHead(doc, GTAReport);

                //Create Annexures in a new section
                Console.WriteLine("Adding Annexures");
                foreach (GTAReportDM Annexure in GTAReport.Annexures)
                    GTARichTextEditorUtils.InsertMainCommands(doc, Annexure);
                Console.WriteLine("Annexures added Successfully!\n");

				//Create DataSources in a new section
				Console.WriteLine("Adding Data source information");
                doc.AppendSection();
                GTARichTextEditorUtils.InsertDataSources(doc, GTAReport);
                Console.WriteLine("Data source added Successfully!\n");

				//Insert Header and Footer content
				GTARichTextEditorUtils.InsertHeaderFooter(doc, GTAReport, strFileName);

                //Change all the page's orientation to Landscape
                GTARichTextEditorUtils.SetDocOrientationLandscape(doc);

                //Insert Hyperlinks
                GTARichTextEditorUtils.InsertHyperlinks(doc);
                doc.SaveDocument(strDocumentPath, DevExpress.XtraRichEdit.DocumentFormat.OpenXml);

                //Create TableBorders, Repeat TableHeaders
                GTAOpenXMLUtils.EditWithOpenXml(strDocumentPath);

                //Insert Attachments
                GTAInteropWordUtils.InsertAttachments(strDocumentPath, GTAReport.ReportAttachement.getAllAttachments(), "AttachmentsBody");

				Console.WriteLine("Conversion to docx Successful!\n");
            }
            catch (Exception ex)
            {
                //Console.WriteLine(ex.Message);
                Console.WriteLine("CreateLTRA Inner Exception: " + ex.InnerException);
                Console.WriteLine("CreateLTRA Message : " + ex.Message);
                Console.WriteLine("CreateLTRA StackTrace : " + ex.StackTrace);
            }
        }
    }
}
