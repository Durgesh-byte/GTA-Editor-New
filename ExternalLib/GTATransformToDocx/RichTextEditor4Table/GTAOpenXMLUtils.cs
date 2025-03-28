using System;
using System.Collections.Generic;

using System.Linq;

using DocumentFormat.OpenXml;
using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Wordprocessing;

namespace GTATransformToDocx
{
    class GTAOpenXMLUtils
    {
        public static void EditWithOpenXml(string filename)
        {
            try
            {
                using (WordprocessingDocument wordDoc = WordprocessingDocument.Open(filename, true))
                {
                    Body bd = wordDoc.MainDocumentPart.Document.Body;
                    OpenXmlElementList children = bd.ChildElements;

                    for (int i = 0; i < children.Count; i++)
                    {
                        OpenXmlElement elem = children.ElementAt(i);
                        if (elem.LocalName == "tbl")
                        {
                            bool isMainTable = isMainAnnexCommandTable(elem);

                            if (isMainTable)
                            {
                                addTableBorder(elem);
                                removeAllCellBorders(elem);
                                insertTopBorderInItemRow(elem);
                                repeatHeaderRows(elem);
                            }
                            bool isSummaryTable = isSummaryCommandTable(elem);
                            if (isSummaryTable)
                            {
                                insertTopBorderInItemRow(elem);
                                repeatHeaderRows(elem);
                            }
                        }
                    }
                    wordDoc.Close();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine("Error in opening the docx for formatting! Close the docx and retry\n");
            }
        }

        public static void addTableBorder(OpenXmlElement iElem)
        {
            IEnumerable<OpenXmlElement> elemes = iElem.Elements();
            for (int j = 0; j < elemes.Count(); j++)
            {
                OpenXmlElement innerElem = elemes.ElementAt(j);
                //Console.WriteLine(innerElem.LocalName);
                if (innerElem.LocalName == "tblPr")
                {
                    TableBorders tblBrdrs = getTableBorders();
                    innerElem.Append(tblBrdrs);
                }
            }
        }

        public static TableBorders getTableBorders()
        {
            TableBorders tblBrdrs = new TableBorders(

                     new TopBorder()
                     {
                         Val =
                             new EnumValue<BorderValues>(BorderValues.Single),
                         Size = 4,
                         Space = 0,
                         Color = "auto"

                     },
           new BottomBorder()
           {
               Val =
                   new EnumValue<BorderValues>(BorderValues.Single),
               Size = 4,
               Space = 0,
               Color = "auto"
           },
           new LeftBorder()
           {
               Val =
                   new EnumValue<BorderValues>(BorderValues.Single),
               Size = 4,
               Space = 0,
               Color = "auto"
           },
           new RightBorder()
           {
               Val =
                   new EnumValue<BorderValues>(BorderValues.Single),
               Size = 4,
               Space = 0,
               Color = "auto"
           },
               new InsideHorizontalBorder()
               {
                   Val =
                       new EnumValue<BorderValues>(BorderValues.None),
                   Size = 4,
                   Space = 0,
                   Color = "auto"
               },
           new InsideVerticalBorder()
           {
               Val =
                   new EnumValue<BorderValues>(BorderValues.Single),
               Size = 4,
               Space = 0,
               Color = "auto"
           }

            );
            return tblBrdrs;
        }

        public static void removeAllCellBorders(OpenXmlElement iElem)
        {
            IEnumerable<OpenXmlElement> elemes = iElem.Elements();
            for (int j = 0; j < elemes.Count(); j++)
            {
                OpenXmlElement innerElem = elemes.ElementAt(j);
                //Console.WriteLine(innerElem.LocalName);
                if (innerElem.LocalName == "tr")
                {
                    IEnumerable<OpenXmlElement> tcElems = innerElem.ChildElements;
                    for (int k = 0; k < tcElems.Count(); k++)
                    {
                        OpenXmlElement tcElem = tcElems.ElementAt(k);
                        IEnumerable<OpenXmlElement> pElems = tcElem.ChildElements;
                        for (int l = 0; l < pElems.Count(); l++)
                        {
                            OpenXmlElement pElem = pElems.ElementAt(l);
                            if (pElem.LocalName == "tcPr")
                            {
                                IEnumerable<OpenXmlElement> bookmarkElems = pElem.ChildElements;
                                for (int m = 0; m < bookmarkElems.Count(); m++)
                                {
                                    OpenXmlElement bElem = bookmarkElems.ElementAt(m);
                                    if (bElem.LocalName == "tcBorders")
                                    {
                                        bElem.Remove();
                                    }
                                }
                            }
                        }
                    }

                }

            }
        }

        public static void insertTopBorderInItemRow(OpenXmlElement iElem)
        {
            IEnumerable<OpenXmlElement> elemes = iElem.Elements();
            for (int j = 0; j < elemes.Count(); j++)
            {
                OpenXmlElement innerElem = elemes.ElementAt(j);
                //Console.WriteLine(innerElem.LocalName);
                if ((innerElem.LocalName == "tr"))
                {
                    int rowType = getRowType(innerElem);
                    if ((rowType == 11) || (rowType == 22))
                    {
                        for (int l = 0; l < innerElem.ChildElements.Count; l++)
                        {
                            OpenXmlElement trChild = innerElem.ElementAt(l);
                            IEnumerable<OpenXmlElement> tcChildren = trChild.ChildElements;
                            for (int k = 0; k < tcChildren.Count(); k++)
                            {
                                OpenXmlElement tcChild = tcChildren.ElementAt(k);
                                if (tcChild.LocalName == "tcPr")
                                {
                                    TableCellBorders tcBorders = new TableCellBorders();
                                    TopBorder topBrdr = new TopBorder();
                                    topBrdr.Color = "auto";
                                    topBrdr.Val = BorderValues.Single;
                                    topBrdr.Size = 4;
                                    topBrdr.Space = 0;
                                    tcBorders.Append(topBrdr);
                                    if (rowType == 22)
                                    {
                                        BottomBorder bottomBrdr = new BottomBorder();
                                        bottomBrdr.Color = "auto";
                                        bottomBrdr.Val = BorderValues.Single;
                                        bottomBrdr.Size = 4;
                                        bottomBrdr.Space = 0;
                                        tcBorders.Append(bottomBrdr);

                                    }
                                    tcChild.Append(tcBorders);
                                }
                            }
                        }
                    }
                }

            }
        }

        public static int getRowType(OpenXmlElement iElem)
        {
            if (iElem.ChildElements.Count >= 3)
            {
                OpenXmlElement trChild = iElem.ElementAt(0);
                IEnumerable<OpenXmlElement> tcChildren = trChild.ChildElements;
                for (int k = 0; k < tcChildren.Count(); k++)
                {
                    OpenXmlElement tcChild = tcChildren.ElementAt(k);
                    if (tcChild.LocalName == "p")
                    {
                        IEnumerable<OpenXmlElement> bookmarkElems = tcChild.ChildElements;
                        for (int m = 0; m < bookmarkElems.Count(); m++)
                        {
                            OpenXmlElement rElems = bookmarkElems.ElementAt(m);
                            if (rElems.LocalName == "r")
                            {
                                IEnumerable<OpenXmlElement> rChildren = rElems.ChildElements;
                                for (int n = 0; n < rChildren.Count(); n++)
                                {
                                    OpenXmlElement textElem = rChildren.ElementAt(n);

                                    if (textElem.LocalName == "t")
                                    {
                                        string itemVal = textElem.InnerText;
                                        if (itemVal != "")
                                        {
                                            int val = -1;
                                            bool isPass = int.TryParse(itemVal, out val);
                                            if (isPass)
                                            {
                                                return 11;
                                            }
                                            else
                                            {
                                                if (itemVal == "Item")
                                                {
                                                    return 22;
                                                }
                                            }
                                        }

                                    }
                                }
                            }
                        }
                    }
                }

            }
            return -1;
        }

        public static bool isSummaryCommandTable(OpenXmlElement iElem)
        {
            bool isSummaryTable = false;
            IEnumerable<OpenXmlElement> elemes = iElem.Elements();
            bool isFirstRow = false;
            String tableName = "";
            for (int j = 0; j < elemes.Count(); j++)
            {
                OpenXmlElement innerElem = elemes.ElementAt(j);
                //Console.WriteLine(innerElem.LocalName);
                if ((innerElem.LocalName == "tr") && (isFirstRow == false))
                {
                    isFirstRow = true;
                    tableName = innerElem.InnerText;
                    IEnumerable<OpenXmlElement> tcElems = innerElem.ChildElements;
                    for (int k = 0; k < tcElems.Count(); k++)
                    {
                        OpenXmlElement tcElem = tcElems.ElementAt(k);
                        IEnumerable<OpenXmlElement> pElems = tcElem.ChildElements;
                        for (int l = 0; l < pElems.Count(); l++)
                        {
                            OpenXmlElement pElem = pElems.ElementAt(l);
                            if (pElem.LocalName == "p")
                            {
                                IEnumerable<OpenXmlElement> bookmarkElems = pElem.ChildElements;
                                for (int m = 0; m < bookmarkElems.Count(); m++)
                                {
                                    OpenXmlElement bElem = bookmarkElems.ElementAt(m);
                                    if (bElem.LocalName == "bookmarkStart")
                                    {
                                        IList<OpenXmlAttribute> attrs = bElem.GetAttributes();
                                        for (int n = 0; n < attrs.Count; n++)
                                        {
                                            OpenXmlAttribute attr = attrs.ElementAt(n);
                                            if (attr.LocalName == "name")
                                            {
                                                if (attr.Value == "Summary")
                                                {
                                                    return true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                }

            }
            return isSummaryTable;
        }

        public static bool isMainAnnexCommandTable(OpenXmlElement iElem)
        {
            bool isMainCommandTable = false;
            IEnumerable<OpenXmlElement> elemes = iElem.Elements();
            bool isFirstRow = false;
            String tableName = "";
            for (int j = 0; j < elemes.Count(); j++)
            {
                OpenXmlElement innerElem = elemes.ElementAt(j);
                //Console.WriteLine(innerElem.LocalName);
                if ((innerElem.LocalName == "tr") && (isFirstRow == false))
                {
                    isFirstRow = true;
                    tableName = innerElem.InnerText;
                    IEnumerable<OpenXmlElement> tcElems = innerElem.ChildElements;
                    for (int k = 0; k < tcElems.Count(); k++)
                    {
                        OpenXmlElement tcElem = tcElems.ElementAt(k);
                        IEnumerable<OpenXmlElement> pElems = tcElem.ChildElements;
                        for (int l = 0; l < pElems.Count(); l++)
                        {
                            OpenXmlElement pElem = pElems.ElementAt(l);
                            if (pElem.LocalName == "p")
                            {
                                IEnumerable<OpenXmlElement> bookmarkElems = pElem.ChildElements;
                                for (int m = 0; m < bookmarkElems.Count(); m++)
                                {
                                    OpenXmlElement bElem = bookmarkElems.ElementAt(m);
                                    if (bElem.LocalName == "bookmarkStart")
                                    {
                                        IList<OpenXmlAttribute> attrs = bElem.GetAttributes();
                                        for (int n = 0; n < attrs.Count; n++)
                                        {
                                            OpenXmlAttribute attr = attrs.ElementAt(n);
                                            if (attr.LocalName == "name")
                                            {
                                                if ((attr.Value == "MainCommands") || (attr.Value.Contains("Annexure") ))
                                                {
                                                    return true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                }

            }
            return isMainCommandTable;
        }

        public static void repeatHeaderRows(OpenXmlElement iElem)
        {
            IEnumerable<OpenXmlElement> elemes = iElem.Elements();
            bool isFirstRow = false;
            for (int j = 0; j < elemes.Count(); j++)
            {
                OpenXmlElement innerElem = elemes.ElementAt(j);
                //Console.WriteLine(innerElem.LocalName);
                if ((innerElem.LocalName == "tr") && (isFirstRow == false))
                {
                    isFirstRow = true;
                    innerElem.Append(new TableHeader());
                    break;
                }
            }
        }
    }
}
