using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Xsl;
using System.Xml;
using System.IO;


namespace AINGTATransformToHtml
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length > 2)
            {
                String inputXSL = args[0];
                String inputXML = args[1];
                String outputHTMLFile = args[2];
                transfromToHTML(inputXSL, inputXML, outputHTMLFile);
            }
            else
                Console.WriteLine("Invalid Inputs!\nUsage: <xsl file> <xml file> <html file>");
        }
        public static void transfromToHTML(String inputXSL,String inputXML, String outputFileName)
        {
                XslCompiledTransform xslt = new XslCompiledTransform();
                xslt.Load(inputXSL);
                // Execute the transform and output the results to a file.
                string fileName = inputXML;
                xslt.Transform(fileName, outputFileName);
        }
    }
}
