using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using DevExpress.XtraRichEdit;
using DevExpress.XtraRichEdit.API.Native;

namespace GTATransformToDocx
{
    class GTACreateDocFromElement
    {

        public static void CreateDoc(string strXMLPath, string strDocumentPath)
        {
            FileInfo fileInfo = new FileInfo(strXMLPath);
            strXMLPath = fileInfo.FullName;

            String fileName = fileInfo.Name;

            fileInfo = new FileInfo(strDocumentPath);
            strDocumentPath = fileInfo.FullName;
            string strFileName = fileInfo.Name;
            strFileName = strFileName.Substring(0, strFileName.IndexOf("."));


            GTAXMLToDocParser xmlParser = new GTAXMLToDocParser();
            bool rc = xmlParser.parseXmlFile(strXMLPath);
            if (rc == true)
            {
                //Default Document Formatting
                DevExpress.XtraRichEdit.RichEditControlCompatibility.DefaultFontSize = 8;
                 DevExpress.XtraRichEdit.RichEditControlCompatibility.DefaultFontName = "Arial";

                 //Create new Rich Edit Document
                 RichEditControl richEditControl = new RichEditControl();
                 richEditControl.CreateNewDocument(false);
                 Document doc = richEditControl.Document;

                 try
                 {
                    // doc.SaveDocument(strDocumentPath, DevExpress.XtraRichEdit.DocumentFormat.OpenXml);

                     //Create Header Data - includes ReportInfo, Test Details, Specifications, Other Details
                     Console.WriteLine("Adding Header");
                     GTARichTextEditorUtils.insertHeaderData(doc, xmlParser.Header, xmlParser.ProcedureName);
                     Console.WriteLine("Header data added Successfully!\n");

           
                     //Create Main Commands in a new section
                     Console.WriteLine("Adding Main Commands");
                     GTARichTextEditorUtils.insertCommands(doc, xmlParser.Commands);
                     Console.WriteLine("Main Commands added Successfully!\n");

          

                     //Insert Footer content
                     GTARichTextEditorUtils.InsertHeaderFooter(doc, xmlParser.ProcedureName);

                     //Change all the page's orientation to Landscape
                     GTARichTextEditorUtils.SetDocOrientationLandscape(doc);

                
                     doc.SaveDocument(strDocumentPath, DevExpress.XtraRichEdit.DocumentFormat.OpenXml);

                     //Create TableBorders, Repeat TableHeaders
                   //  AINGTAOpenXMLUtils.EditWithOpenXml(strDocumentPath);

                     Console.WriteLine("Conversion to docx Successful!\n");
                 }
                 catch (Exception ex)
                 {
                     Console.WriteLine(ex.Message);
                 }
            }

                    
        }

    }
}
