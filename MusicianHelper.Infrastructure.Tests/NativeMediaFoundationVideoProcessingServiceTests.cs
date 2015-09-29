using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MusicianHelper.Infrastructure.Models.Enums;
using MusicianHelper.Infrastructure.Services.Concrete;
using Xunit;

namespace MusicianHelper.Infrastructure.Tests
{
    public class NativeMediaFoundationVideoProcessingServiceTests
    {
        [Fact]
        public void wrapper_creates_correctly()
        {
            var vps = new NativeMediaFoundationVideoProcessingService();
            Assert.NotNull(vps);
        }

        [Fact]
        public void encodes_correctly()
        {
            var cd = Environment.CurrentDirectory;
            var outputPath = Path.Combine(cd, "output.wav");

            if (File.Exists(outputPath))
                File.Delete(outputPath);

            var vps = new NativeMediaFoundationVideoProcessingService();

            var imagePaths = new List<string>() { "C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Images\\paper-stained-3-texture.bmp" };

            vps.CreateVideoFromImages(imagePaths, "C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Audio\\sorry_dave.wav", outputPath, VideoQuality.FullHD);

            Assert.True(File.Exists(outputPath));
        }
    }
}
