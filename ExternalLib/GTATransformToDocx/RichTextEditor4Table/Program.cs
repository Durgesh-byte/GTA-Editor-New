using DocumentFormat.OpenXml.Spreadsheet;
using Microsoft.Office.Interop.Word;
using System;
using System.Diagnostics;
using System.IO;
using System.Threading;

namespace GTATransformToDocx
{
    class program
    {
        [STAThread]
        static void Main(string[] args)
        {
            //Thread.Sleep(5000);
            Console.WriteLine("GTADocx Plugin V1.1 _/\\_ - Airbus India \n");
            
            string XMLPath = "";
            string DocxPath = "";
			string docType = "";
            string fileVersionsToolsBench = "";

            //XMLPath = "C:\\Users\\I0000680\\Desktop\\foreach_edit.xml";
            //DocxPath = "C:\\Users\\I0000680\\Desktop\\foreach_edit.docx";

            //XMLPath = "C:\\Users\\I0000680\\Desktop\\test12.xml";
            //DocxPath = @"C:\Users\I0000680\Desktop\test12.docx";
            //XMLPath = @"C:\Users\I0000680\Desktop\trial - Copy.xml";
            //XMLPath = @"C:\Users\I0000680\Desktop\trial_LTRA_parallel_call.xml";
            //DocxPath = @"C:\Users\I0000680\Desktop\trial.docx";

            //AINGTACreateLTRA.CreateLTRA(XMLPath, DocxPath, "false");

            foreach (String arg in args)
                System.Console.WriteLine(arg + "\t");
            if (args.Length >= 2)
            {
                string FirstInput = args[0];
                if (File.Exists(FirstInput))
                {
                    FileInfo XMLFileInfo = new FileInfo(FirstInput);
                    if (XMLFileInfo.Extension.ToLower() == ".xml")
                    {
                        XMLPath = FirstInput;
                    }
                    else
                    {
                        System.Console.WriteLine("File Format Error, Input should be an XML!");
                        return;
                    }
                }
                else
                {
                    System.Console.WriteLine("XML File doesn't exist!");
                    return;
                }

                DocxPath = args[1];
                docType = args[2];

                if (args.Length >= 3)
                {
                    fileVersionsToolsBench = args[3];
                }

                if (docType == "PRINT_DOC")
                {
                    GTACreateDocFromElement.CreateDoc(XMLPath, DocxPath);
                }
                else
                {
                    GTACreateLTRA.CreateLTRA(XMLPath, DocxPath, docType, fileVersionsToolsBench);
                }
            }
            else
            {
                System.Console.WriteLine("Please enter arguments.");
                System.Console.WriteLine("Usage: GTARichTextEditor <XML Path> <Docx Path>");
                return;
            }
        }
    }
}
