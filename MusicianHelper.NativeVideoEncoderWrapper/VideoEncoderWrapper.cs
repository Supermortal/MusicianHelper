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
        public string GetWorkingDirectory()
        {
            if (_workingDirectory == null)
            {
                var sb = new StringBuilder();
                sb.Append("MusicianHelper.NativeVideoEncoderWrapper\\Encoder\\");
                var config = "Release";
#if DEBUG
                config = "Debug";
#endif
                sb.Append(config);

                var baseDir = GetBaseDirectory();
                _workingDirectory = Path.Combine(baseDir,
                    sb.ToString());
            }

            return _workingDirectory;
        }

        public void Encode(string imageFilePath, string audioFilePath, string videoOutputPath)
        {
            var sb = new StringBuilder();
            sb.Append("MusicianHelper.NativeVideoEncoderWrapper\\Encoder\\");
            var config = "Release";
#if DEBUG
            config = "Debug";
#endif
            sb.Append(config);

            var baseDir = GetBaseDirectory();
            var encoderPath = Path.Combine(baseDir,
                sb.ToString());

            //Create process
            System.Diagnostics.Process pProcess = new System.Diagnostics.Process();

            //strCommand is path and file name of command to run
            pProcess.StartInfo.FileName = Path.Combine(encoderPath, "MusicianHelper.MediaFoundationVideoEncoder.exe");

            sb = new StringBuilder();
            sb.Append(imageFilePath);
            sb.Append(' ');
            sb.Append(audioFilePath);
            sb.Append(' ');
            sb.Append(videoOutputPath);
            //strCommandParameters are parameters to pass to program
            pProcess.StartInfo.Arguments = sb.ToString();

            pProcess.StartInfo.UseShellExecute = false;

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
    }
}
