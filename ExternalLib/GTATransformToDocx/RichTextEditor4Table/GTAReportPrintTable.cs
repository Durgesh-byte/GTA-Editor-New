using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GTATransformToDocx
{
    public class GTAReportPrintTable
    {
        public string TableName;
        public List<string> TableHeaders;
        public List<GTAReportParamTable> ParamTables;

        public GTAReportPrintTable()
        {
            TableHeaders = new List<string>();
            ParamTables = new List<GTAReportParamTable>();
        }
    }

    public class GTAReportParamTable
    {
        public string ParamName;
        public string ParamType;
        public string ParamUnit;
        List<Other_Details> lstTableDetails;

        public List<Other_Details> TableDetails
        {
            get { return lstTableDetails; }
            set { lstTableDetails = value; }
        }

        public GTAReportParamTable()
        {
            TableDetails = new List<Other_Details>();
        }
    }
}
