using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GTATransformToDocx
{
    public struct DataSource
    {
        public String fileName;
        public String fileType;
        public String filePath;
    }

    class GTAReportDataSources
    {
        List<DataSource> listDataSources;

        public GTAReportDataSources()
        {
            listDataSources = new List<DataSource>();
        }

        public void InsertDataSource(DataSource dataSource)
        {
            listDataSources.Add(dataSource);
        }

        public List<DataSource> GetDataSources()
        {
            return listDataSources;
        }
    }
}
