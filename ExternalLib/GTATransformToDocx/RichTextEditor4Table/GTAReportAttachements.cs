using System;
using System.Collections.Generic;
using System.Text;

namespace GTATransformToDocx
{
    class GTAReportATTACHMENTS
    {
        List<String> attachementList;

        public GTAReportATTACHMENTS()
        {
            attachementList = new List<String>();
        }
        public void insertAttachement(String attachmentItem)
        {
            attachementList.Add(attachmentItem);
        }
        public List<String> getAllAttachments()
        {
            return attachementList;
        }
    }
}
