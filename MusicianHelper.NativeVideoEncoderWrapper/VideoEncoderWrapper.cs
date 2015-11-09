using System;
using System.IO;
using System.Text;

namespace MusicianHelper.NativeVideoEncoderProcess
{
    public class VideoEncoderWrapper
    {

        private string _basePath = null;
        private string GetBaseDirectory()
        {
#if DEBUG
            if (_basePath == null)
            {
                var currentDir = Environment.CurrentDirectory;
                var parts = currentDir.Split('\\');
                var sb = new StringBuilder();
                for (int i = 0; i < parts.Length; i++)
                {
                    var part = parts[i];

                    sb.Append(part);
                    sb.Append('\\');
                    if (part == "MusicianHelper")
                    {
                        break;
                    }
                }

                _basePath = sb.ToString().TrimEnd('\\');
            }

            return _basePath;
#endif

            return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Musician Helper");
        }

        private string _workingDirectory = null;
        public string GetWorkingDirectory(string module)
        {
            //if (_workingDirectory == null)
            //{
                var sb = new StringBuilder();
                sb.Append("MusicianHelper.NativeVideoEncoderWrapper\\");
                sb.Append(module);
                sb.Append('\\');
                var config = "Release";
#if DEBUG
                config = "Debug";
#endif
                sb.Append(config);

                var baseDir = GetBaseDirectory();
                _workingDirectory = Path.Combine(baseDir,
                    sb.ToString());
            //}

            return _workingDirectory;
        }

        public void Encode(string imageFilePath, string audioFilePath, string videoOutputPath)
        {
            imageFilePath = CheckConvertImage(imageFilePath);

            var encoderPath = GetWorkingDirectory("Encoder");

            //Create process
            System.Diagnostics.Process pProcess = new System.Diagnostics.Process();

            //strCommand is path and file name of command to run
            pProcess.StartInfo.FileName = Path.Combine(encoderPath, "MusicianHelper.MediaFoundationVideoEncoder.exe");

            var sb = new StringBuilder();
            sb.Append(imageFilePath);
            sb.Append(' ');
            sb.Append(audioFilePath);
            sb.Append(' ');
            sb.Append(videoOutputPath);
            //strCommandParameters are parameters to pass to program
            pProcess.StartInfo.Arguments = sb.ToString();

            pProcess.StartInfo.UseShellExecute = false;
            pProcess.StartInfo.RedirectStandardOutput = true;

            //Set output of program to be written to process output stream
            pProcess.StartInfo.RedirectStandardOutput = true;

            //Optional
            pProcess.StartInfo.WorkingDirectory = encoderPath;

            //Start the process
            pProcess.Start();

            //Get program output
            string strOutput = pProcess.StandardOutput.ReadToEnd();

            //Wait for process to finish
            pProcess.WaitForExit();
        }

        public string CheckConvertImage(string imageFilePath)
        {
            if (imageFilePath.EndsWith("bmp"))
                return imageFilePath;

            var parts = imageFilePath.Split('.');
            var extension = parts[parts.Length - 1];
            var cip = GetConvertedImagePath(imageFilePath);
            ConvertImage(imageFilePath, cip);

            return cip;
        }

        public string GetConvertedImagePath(string imageFilePath)
        {
            var path = imageFilePath.Replace('/', '\\');

            var parts = imageFilePath.Split('\\');
            var imageName = parts[parts.Length - 1];

            var exParts = imageName.Split('.');
            exParts[0] = exParts[0] += "_converted";

            var newImageName = exParts[0] + ".bmp";

            return Path.Combine(GetWorkingDirectory("ImageConverter"), newImageName);
        }

        public void ConvertImage(string imageFilePath, string convertedImagePath, string mimeType = "image/bmp")
        {
            var sb = new StringBuilder();
            sb.Append("MusicianHelper.NativeVideoEncoderWrapper\\ImageConverter\\");
            var config = "Release";
#if DEBUG
            config = "Debug";
#endif
            sb.Append(config);

            var baseDir = GetBaseDirectory();
            var converterPath = Path.Combine(baseDir,
                sb.ToString());

            //Create process
            System.Diagnostics.Process pProcess = new System.Diagnostics.Process();

            //strCommand is path and file name of command to run
            pProcess.StartInfo.FileName = Path.Combine(converterPath, "MusicianHelper.GDIPlusImageConverter.exe");

            sb = new StringBuilder();
            sb.Append(imageFilePath);
            sb.Append(' ');
            sb.Append(convertedImagePath);
            sb.Append(' ');
            sb.Append(mimeType);
            //strCommandParameters are parameters to pass to program
            pProcess.StartInfo.Arguments = sb.ToString();

            pProcess.StartInfo.UseShellExecute = false;
            pProcess.StartInfo.RedirectStandardOutput = true;

            //Set output of program to be written to process output stream
            pProcess.StartInfo.RedirectStandardOutput = true;

            //Optional
            pProcess.StartInfo.WorkingDirectory = converterPath;

            //Start the process
            pProcess.Start();

            //Get program output
            //TODO check for errors in conversion
            string strOutput = pProcess.StandardOutput.ReadToEnd();

            //Wait for process to finish
            pProcess.WaitForExit();
        }

    }
}
