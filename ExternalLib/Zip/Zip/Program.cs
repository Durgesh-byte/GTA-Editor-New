using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Packaging;
using System.IO;

namespace Zip
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 2)
            {


                String ZipFileName = args[0];
                String pathToZip = args[1];
                ZipClass.callZipFunction(ZipFileName,pathToZip);
            }
            else
            {
                Console.WriteLine("Syntax:");
                Console.WriteLine("Zip.exe <ZipFileName> <Directory/File To Zip>");
                Console.WriteLine("e.g. Zip.exe \"c:\\temp\\Output.zip\" \"C:\\users\\Temp\\MYdir\" ");
            }
        }
    }


    public static class ZipClass
    {
        public static void callZipFunction(String ZipFileName, String pathToZip)
        {
                if ((ZipFileName != "") && (pathToZip != ""))
                {

                    String zipFileName = Path.GetFileName(ZipFileName);

                    FileAttributes attr1 = File.GetAttributes(pathToZip);
                    //if ((attr1 & FileAttributes.Directory) == FileAttributes.Directory)
                    //{
                    //    Console.WriteLine("Wrong Arguements.");
                    //    Console.WriteLine("Syntax:");
                    //    Console.WriteLine("Zip.exe <ZipFileName> <Directory/File To Zip>");
                    //    Console.WriteLine("e.g. Zip.exe \"c:\\temp\\Output.zip\" \"C:\\users\\Temp\\MYdir\" ");
                    //}
                    //else 
                    if (!ZipFileName.ToLower().EndsWith(".zip"))
                    {
                        Console.WriteLine("Wrong Arguements.");
                        Console.WriteLine("Syntax:");
                        Console.WriteLine("Zip.exe <ZipFileName> <Directory/File To Zip>");
                        Console.WriteLine("e.g. Zip.exe \"c:\\temp\\Output.zip\" \"C:\\users\\Temp\\MYdir\" ");
                    }
                    else
                    {

                        FileAttributes attr = File.GetAttributes(pathToZip);
                        if ((attr & FileAttributes.Directory) == FileAttributes.Directory)
                            ZipClass.AddDirectoryToZip(ZipFileName, pathToZip);
                        else
                            ZipClass.AddFileToZip(ZipFileName, pathToZip);

                    }
                }
                else
                {
                    Console.WriteLine("Syntax:");
                    Console.WriteLine("Zip.exe <ZipFileName> <Directory/File To Zip>");
                    Console.WriteLine("e.g. Zip.exe \"c:\\temp\\Output.zip\" \"C:\\users\\Temp\\MYdir\" ");
                }
        }
        public static void AddDirectoryToZip(string zipFilename, string dirToAdd, CompressionOption compression = CompressionOption.Normal)
        {
            using (Package zip = System.IO.Packaging.Package.Open(zipFilename, FileMode.OpenOrCreate))
            {
                DirectoryInfo di = new DirectoryInfo(dirToAdd);
                FileInfo[] files = di.GetFiles("*.*", SearchOption.AllDirectories);
                for (int i = 0; i < files.Length; i++)
                {
                    
                    dirToAdd = dirToAdd.Replace("/", "\\");
                    String relPath = files[i].FullName.Replace(dirToAdd, "");
                    Console.WriteLine(dirToAdd);
                    Console.WriteLine(relPath);
                    string destFilename = relPath;
                    if(relPath.StartsWith("\\"))
                        destFilename = "." + relPath;
                    else
                        destFilename = ".\\" + relPath;
                    destFilename = destFilename.Replace(" ", "_");
                    Uri uri = PackUriHelper.CreatePartUri(new Uri(destFilename, UriKind.Relative));
                    if (zip.PartExists(uri))
                    {
                        zip.DeletePart(uri);
                    }
                    PackagePart part = zip.CreatePart(uri, "", compression);
                    using (FileStream fileStream = new FileStream(files[i].FullName, FileMode.Open, FileAccess.Read))
                    {
                        using (Stream dest = part.GetStream())
                        {
                            CopyTo(fileStream, dest);
                        }
                    }
                }
            }
        }
        public static void AddFileToZip(string zipFilename, string fileToAdd, CompressionOption compression = CompressionOption.Normal)
        {
            using (Package zip = System.IO.Packaging.Package.Open(zipFilename, FileMode.OpenOrCreate))
            {
                string destFilename = ".\\" + Path.GetFileName(fileToAdd);
                Uri uri = PackUriHelper.CreatePartUri(new Uri(destFilename, UriKind.Relative));
                if (zip.PartExists(uri))
                {
                    zip.DeletePart(uri);
                }
                PackagePart part = zip.CreatePart(uri, "", compression);
                using (FileStream fileStream = new FileStream(fileToAdd, FileMode.Open, FileAccess.Read))
                {
                    using (Stream dest = part.GetStream())
                    {
                        CopyTo(fileStream, dest);
                    }
                }
            }
        }
        private static void CopyTo(this Stream input, Stream output)
        {
            byte[] buffer = new byte[16 * 1024]; // Fairly arbitrary size
            int bytesRead;

            while ((bytesRead = input.Read(buffer, 0, buffer.Length)) > 0)
            {
                output.Write(buffer, 0, bytesRead);
            }
        }    
    }
}
