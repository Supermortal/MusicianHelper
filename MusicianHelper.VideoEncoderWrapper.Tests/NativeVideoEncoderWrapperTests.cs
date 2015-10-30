using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xunit;

namespace MusicianHelper.VideoEncoderWrapper.Tests
{
    public class NativeVideoEncoderWrapperTests
    {
        [Fact]
        public void encoder_works_correctly()
        {
            var vew = new NativeVideoEncoderProcess.VideoEncoderWrapper();
            var workingDir = vew.GetWorkingDirectory();
            var outputPath = Path.Combine(workingDir, "output.wmv");

            if (File.Exists(outputPath))
                File.Delete(outputPath);
           
            vew.Encode("C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\paper-stained-3-texture.bmp", "C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\sorry_dave.wav", "output.wmv");

            Assert.True(File.Exists(outputPath));

            var fi = new FileInfo(outputPath);
            Assert.True(fi.Length > 1000000);
        }
    }
}
