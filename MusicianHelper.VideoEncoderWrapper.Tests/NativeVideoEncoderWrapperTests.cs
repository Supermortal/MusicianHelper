using System.IO;
using Xunit;

namespace MusicianHelper.VideoEncoderWrapper.Tests
{
    public class NativeVideoEncoderWrapperTests
    {
        [Fact]
        public void encoder_works_correctly()
        {
            var vew = new NativeVideoEncoderProcess.VideoEncoderWrapper();
            var workingDir = vew.GetWorkingDirectory("Encoder");
            var outputPath = Path.Combine(workingDir, "output.wmv");

            if (File.Exists(outputPath))
                File.Delete(outputPath);

            vew.Encode("C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\paper-stained-3-texture.bmp", "C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\sorry_dave.wav", outputPath);

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

            vew.Encode("C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\paper-stained-3-texture.bmp", "C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\sorry_dave.mp3", outputPath);

            Assert.True(File.Exists(outputPath));

            var fi = new FileInfo(outputPath);
            Assert.True(fi.Length > 1000000);
        }

        [Fact]
        public void encoder_works_with_jpg_correctly()
        {
            var vew = new NativeVideoEncoderProcess.VideoEncoderWrapper();
            var workingDir = vew.GetWorkingDirectory("Encoder");
            var outputPath = Path.Combine(workingDir, "output_jpg.wmv");

            if (File.Exists(outputPath))
                File.Delete(outputPath);

            vew.Encode("C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\test.jpg", "C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\sorry_dave.wav", outputPath);

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

            vew.Encode("C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\test.png", "C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\sorry_dave.wav", outputPath);

            Assert.True(File.Exists(outputPath));

            var fi = new FileInfo(outputPath);
            Assert.True(fi.Length > 600000);
        }
    }
}
