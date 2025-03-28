using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Microsoft.Office.Interop.Word;
using System.Collections.Specialized;
using System.Windows;

namespace GTATransformToDocx
{
    class GTAInteropWordUtils
    {
        public static void InsertAttachments(string FileName, List<string> listAttachments, string strBookmarkID)
        {
            Console.WriteLine("Adding Attachments");

            Application app = new Application();
            WdAlertLevel iAlerts = app.DisplayAlerts;
            app.Visible = false;

            Document doc = new Document();
            try
            {
                doc = app.Documents.Open(FileName);
                foreach (Bookmark bookmark in doc.Bookmarks)
                {
                    if (bookmark.Name == strBookmarkID)
                    {
                        Range bookmarkRange = bookmark.Range;

                        /*StringCollection collection = new StringCollection();
                        foreach (string strAttachment in listAttachments)
                            collection.Add(strAttachment);
                        Clipboard.SetFileDropList(collection);
                        
                        bookmarkRange.Paste();

                        foreach (string strAttachment in listAttachments)
                        {
                            object objDataTypeMetafile = WdPasteDataType.wdPasteOLEObject;
                            bookmarkRange.PasteSpecial("", "", "", true, objDataTypeMetafile, strAttachment, GetAttachmentName(strAttachment));
                        }*/

                        foreach (string strAttachment in listAttachments)
                            bookmarkRange.InlineShapes.AddOLEObject("", strAttachment, false, true, "", 0, GetAttachmentName(strAttachment));

                        break;
                    }
                }

                doc.Save();
                Console.WriteLine("Attachments added Successfully!\n");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine("Adding one or more Attachments failed, Check the paths provided in xml or contact Admin!\n");
            }
            finally
            {
                if (doc != null)
                    doc.Close();
                //app.Documents.Close();
                app.Quit();
            }
        }

        static string GetAttachmentName(string AttachmentPathName)
        {
            while (AttachmentPathName.Contains("/"))
                AttachmentPathName = AttachmentPathName.Substring(AttachmentPathName.IndexOf("/") + 1);

            return AttachmentPathName;
        }
    }
}
