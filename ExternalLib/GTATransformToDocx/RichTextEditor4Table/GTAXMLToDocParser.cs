using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace GTATransformToDocx
{
    class GTAXMLToDocParser
    {

        public GTAReportHeader headerData;
        public GTAReportHeader Header
        {
            get { return headerData;  }
            set { headerData = value; }
        }

        List<Command> _lstCommands;
        public List<Command> Commands
        {
            get { return _lstCommands; }
            set { _lstCommands = value; }
        }

        String _elementName;
        public String ProcedureName
        {
            get { return _elementName; }
            set { _elementName = value; }
        }

        public GTAXMLToDocParser()
        {
            headerData = new GTAReportHeader();
            _lstCommands = new List<Command>();
        }

        public bool parseXmlFile(String iXmlFilePath)
        {
            bool rc = false;
            XmlDocument doc = new XmlDocument();

            doc.Load(iXmlFilePath);
            if (doc.HasChildNodes)
            {

                XmlNode rootNode = doc.ChildNodes.Item(0);
                if ((rootNode.Name == "ELEMENT") && (rootNode.HasChildNodes))
                {

                    _elementName = rootNode.Attributes.GetNamedItem("NAME").Value;
                    XmlNodeList children = rootNode.ChildNodes;
                    for (int j = 0; j < children.Count; j++)
                    {

                        XmlNode child = children.Item(j);
                        if (child.Name == "HEADER")
                        {
                            parseHeaderInfo(child);
                        }
                        else if(child.Name == "COMMANDS")
                        {
                            rc = parseCommands(child);
                        }
                    }

                }
            }
            return rc;
        }
        public void parseHeaderInfo(XmlNode iHeaderNode)
        {
            if (iHeaderNode.HasChildNodes)
            {
                XmlNode headerInfoRootNode = iHeaderNode.ChildNodes.Item(0);
                if((headerInfoRootNode.Name == "HEADER") && (headerInfoRootNode.HasChildNodes))
                {
                    String headerName = headerInfoRootNode.Attributes.GetNamedItem("NAME").Value;
                    OtherInfo info = new OtherInfo();
                    info.item = "Name";
                    info.value = headerName;
                    headerData.insertTestDetails(info);
                    XmlNodeList children = headerInfoRootNode.ChildNodes;
                   for (int i = 0; i < children.Count; i++)
                    {
                       OtherInfo details = new OtherInfo();
                        XmlNode child = children.Item(i);
                        if (child != null && child.Name == "ITEM")
                        {
                            XmlAttributeCollection collection = child.Attributes;
                            XmlNode NameNode = collection.GetNamedItem("NAME");
                            if (NameNode != null)
                            {
                                if (NameNode.Value == "Keywords")
                                {
                                    details.item = "Keywords";
                                    details.value = collection.GetNamedItem("VALUE").Value;
                                    headerData.insertTestDetails(details);
                                }
                                else if (NameNode.Value == "Designer Name")
                                {
                                    details.item = "Designer Name";
                                    details.value = collection.GetNamedItem("VALUE").Value;
                                    headerData.insertTestDetails(details);
                                }
                                else if (NameNode.Value == "DESCRIPTION")
                                {
                                    details.item = "Description";
                                    details.value = collection.GetNamedItem("VALUE").Value;
                                    headerData.insertTestDetails(details);
                                }
                            }
                        }
                    }
                }
            }
        }
        public bool parseCommands(XmlNode iCommandNode)
        {
            bool rc = false;
            if (iCommandNode.HasChildNodes)
            {

                XmlNodeList children = iCommandNode.ChildNodes;
                for (int i = 0; i < children.Count; i++)
                {
                    XmlNode child = children.Item(i);
                    Command cmd = new Command();


                    if (child.Name == "Action")
                    {
                        cmd.cmdType = CommandType.ACTION;
                        String type = child.Attributes.GetNamedItem("TYPE").Value;
                        if (type == "Title")
                        {
                            cmd.subCmdType = SubCommandType.TITLE;
                        }
                        else if (type == "Print")
                        {
                            cmd.subCmdType = SubCommandType.PRINT;
                        }
                        else if (type == "Condition")
                        {
                            cmd.subCmdType = SubCommandType.CONDITION;
                        }
                        else
                        {
                            cmd.subCmdType = SubCommandType.NA;
                        }
                        
                    }
                    else if (child.Name == "Check")
                    {
                        cmd.cmdType = CommandType.CHECK;
                    }

                    cmd.Statememt = child.Attributes.GetNamedItem("STATEMENT").Value;
                    cmd.comment = child.Attributes.GetNamedItem("COMMENT").Value;
                    cmd.isIgnored = ((child.Attributes.GetNamedItem("IS_IGNORED").Value == "TRUE" ) ? true : false);
                    cmd.precision = child.Attributes.GetNamedItem("PRECISION").Value;
                    cmd.precisionUnit = child.Attributes.GetNamedItem("PRECISION_UNIT").Value;
                    cmd.timeout = child.Attributes.GetNamedItem("TIMEOUT").Value;
                    cmd.timeoutUnit = child.Attributes.GetNamedItem("TIMEOUT_UNIT").Value;
                    cmd.actionOnFail = child.Attributes.GetNamedItem("ACTION_ON_FAIL").Value;
                    cmd.dumpList = child.Attributes.GetNamedItem("DUMP_LIST").Value;

                    _lstCommands.Add(cmd);
                   
                }
            }
            if (_lstCommands.Count > 0)
            {
                rc = true;
            }
            return rc;
        }
          
    }
}
