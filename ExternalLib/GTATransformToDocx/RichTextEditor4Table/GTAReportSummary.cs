using DevExpress.XtraRichEdit.Import.Doc;
using System;
using System.Collections.Generic;
using System.Text;

namespace GTATransformToDocx
{
    public struct summaryInfoStruct
    {
        public String SummaryInfoText;
        public String SummaryInfoResult;
    }

    public struct summaryItemStruct
    {
        public String SummaryItemText;
        public String SummaryItemResult;
    }

    class GTAReportSummary
    {
        List<summaryItemStruct> lstSummaryItems;

        public List<summaryItemStruct> SummaryItems
        {
            get { return lstSummaryItems; }
            set { lstSummaryItems = value; }
        }

        List<summaryInfoStruct> lstSummaryInfos;

        public List<summaryInfoStruct> SummaryInfos
        {
            get { return lstSummaryInfos; }
            set { lstSummaryInfos = value; }
        }

        public GTAReportSummary()
        {
            lstSummaryItems = new List<summaryItemStruct>();
            lstSummaryInfos = new List<summaryInfoStruct>();
        }
        public void insertInfosSummary(summaryInfoStruct summary)
        {
            SummaryInfos.Add(summary);
        }

        public void insertItemsSummary(summaryItemStruct summary)
        {
            SummaryItems.Add(summary);
        }
    }
}
