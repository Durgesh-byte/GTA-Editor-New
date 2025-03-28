using System;
using System.Collections.Generic;
using System.Text;

namespace GTATransformToDocx
{
    public struct ReportInfo
    {
        public String Date;
        public String Title;
        public String DesignerName;
        public String LTRREF;
        public String Result;
        public String Facility;
        public String GTAVersion;
        
    }

    public struct OtherInfo
    {
        public String item;
        public String value;
    }

    class GTAReportHeader
    {
        ReportInfo objReportInfo;

        public ReportInfo reportInfo
        {
            get { return objReportInfo; }
            set { objReportInfo = value; }
        }

        List<OtherInfo> ListTestDetails;

        public List<OtherInfo> TestDetails
        {
            get { return ListTestDetails; }
            set { ListTestDetails = value; }
        }

        List<OtherInfo> ListSpecifications;

        public List<OtherInfo> Specifications
        {
            get { return ListSpecifications; }
            set { ListSpecifications = value; }
        }

        List<OtherInfo> ListOtherDetails;

        public List<OtherInfo> OtherDetails
        {
            get { return ListOtherDetails; }
            set { ListOtherDetails = value; }
        }

        public GTAReportHeader()
        {
            objReportInfo = new ReportInfo();
            ListTestDetails = new List<OtherInfo>();
            ListSpecifications = new List<OtherInfo>();
            ListOtherDetails = new List<OtherInfo>();
        }

        public void updateReportInfo(ReportInfo rptInfo)
        {
            reportInfo = rptInfo;
        }

        public void insertTestDetails(OtherInfo info)
        {
            TestDetails.Add(info);
        }

        public void insertSpecDetails(OtherInfo info)
        {
            Specifications.Add(info);
        }

        public void insertOtherDetails(OtherInfo info)
        {
            OtherDetails.Add(info);
        }
    }

}
