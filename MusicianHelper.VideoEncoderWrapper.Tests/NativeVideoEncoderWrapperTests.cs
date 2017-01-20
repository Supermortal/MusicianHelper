using System;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using Xunit;

namespace MusicianHelper.VideoEncoderWrapper.Tests
{
    public class NativeVideoEncoderWrapperTests
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

        [Fact]
        public void encoder_works_correctly()
        {
            var vew = new NativeVideoEncoderProcess.VideoEncoderWrapper();
            var workingDir = vew.GetWorkingDirectory("Encoder");
            var outputPath = Path.Combine(workingDir, "output.wmv");

            if (File.Exists(outputPath))
                File.Delete(outputPath);

            vew.EncodingPercentageUpdated += (sender, args) =>
            {
                System.Diagnostics.Debug.WriteLine("Encoding complete: " + args.EncodingPercentage);
            };

            vew.Encode(GetBaseDirectory() + "\\TEST\\test.bmp", GetBaseDirectory() + "\\TEST\\untitled.wav", outputPath);

            Assert.True(File.Exists(outputPath));

            var fi = new FileInfo(outputPath);
            Assert.True(fi.Length > 1000000);
        }

        [Fact]
        public void encoder_works_with_mp3_correctly()
        {
            var vew = new NativeVideoEncoderProcess.VideoEncoderWrapper();
            var workingDir = vew.GetWorkingDirectory("Encoder");
            var outputPath = Path.Combine(workingDir, "output_mp3.wmv");

            if (File.Exists(outputPath))
                File.Delete(outputPath);

            vew.EncodingPercentageUpdated += (sender, args) =>
            {
                System.Diagnostics.Debug.WriteLine("Encoding complete: " + args.EncodingPercentage);
            };

            vew.Encode(GetBaseDirectory() + "\\TEST\\paper-stained-3-texture.bmp", GetBaseDirectory() + "\\TEST\\sorry_dave.mp3", outputPath);

            Assert.True(File.Exists(outputPath));

            var fi = new FileInfo(outputPath);
            if (fi.Exists)
            {
                Assert.True(fi.Length > 1000000);
            }
        }

        [Fact]
        public void encoder_works_with_jpg_correctly()
        {
            var vew = new NativeVideoEncoderProcess.VideoEncoderWrapper();
            var workingDir = vew.GetWorkingDirectory("Encoder");
            var outputPath = Path.Combine(workingDir, "output_jpg.wmv");

            if (File.Exists(outputPath))
                File.Delete(outputPath);

            vew.EncodingPercentageUpdated += (sender, args) =>
            {
                System.Diagnostics.Debug.WriteLine("Encoding complete: " + args.EncodingPercentage);
            };

            vew.Encode(GetBaseDirectory() + "\\TEST\\test.jpg", GetBaseDirectory() + "\\TEST\\sorry_dave.wav", outputPath);

            Assert.True(File.Exists(outputPath));

            var fi = new FileInfo(outputPath);
            Assert.True(fi.Length > 600000);
        }

        [Fact]
        public void encoder_works_with_png_correctly()
        {
            var vew = new NativeVideoEncoderProcess.VideoEncoderWrapper();
            var workingDir = vew.GetWorkingDirectory("Encoder");
            var outputPath = Path.Combine(workingDir, "output_png.wmv");

            if (File.Exists(outputPath))
                File.Delete(outputPath);

            vew.EncodingPercentageUpdated += (sender, args) =>
            {
                System.Diagnostics.Debug.WriteLine("Encoding complete: " + args.EncodingPercentage);
            };

            vew.Encode(GetBaseDirectory() + "\\TEST\\test.png", GetBaseDirectory() + "\\TEST\\sorry_dave.wav", outputPath);

            Assert.True(File.Exists(outputPath));

            var fi = new FileInfo(outputPath);
            Assert.True(fi.Length > 600000);
        }

        [Fact]
        public async Task async_encoder_works_correctly()
        {
            var vew = new NativeVideoEncoderProcess.VideoEncoderWrapper();
            var workingDir = vew.GetWorkingDirectory("Encoder");
            var outputPath = Path.Combine(workingDir, "output.wmv");

            if (File.Exists(outputPath))
                File.Delete(outputPath);

            vew.EncodingPercentageUpdated += (sender, args) =>
            {
                System.Diagnostics.Debug.WriteLine("Encoding complete: " + args.EncodingPercentage);
            };

            vew.EncodingCompleted += (sender, args) =>
            {
                System.Diagnostics.Debug.WriteLine("ENCODING COMPLETE!");
                Assert.True(File.Exists(outputPath));

                var fi = new FileInfo(outputPath);
                Assert.True(fi.Length > 1000000);
            };

            await vew.EncodeAsync(
                GetBaseDirectory() + "\\TEST\\paper-stained-3-texture.bmp",
                GetBaseDirectory() + "\\TEST\\sorry_dave.wav", 
                outputPath);          
        }
    }
}
