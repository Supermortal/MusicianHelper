using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MusicianHelperNativeVideoEncoderLib;
using Xunit;

namespace MusicianHelper.NativeVideoEncoderClient.Tests
{
    public class VideoEncoderTests
    {
        [Fact]
        public void videoencoder_created_correctly()
        {
            var obj = new VideoEncoder();
            Assert.NotNull(obj);
        }

        [Fact]
        public void videoencoder_get_set_imagefilepath()
        {
            const string expected = "Test set string";

            var obj = new VideoEncoder {ImageFilePath = expected};

            Assert.Equal(expected, obj.ImageFilePath);
        }

        [Fact]
        public void videoencoder_get_set_videooutputpath()
        {
            const string expected = "Test set string";

            var obj = new VideoEncoder { VideoOutputPath = expected };

            Assert.Equal(expected, obj.VideoOutputPath);
        }

        [Fact]
        public void videoencoder_get_set_audiofilepath()
        {
            const string expected = "Test set string";

            var obj = new VideoEncoder { AudioFilePath = expected };

            Assert.Equal(expected, obj.AudioFilePath);
        }

        [Fact]
        public void videoencoder_encode()
        {
            //VideoEncoder *ve = new VideoEncoder(L"C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Images\\paper-stained-3-texture.bmp", L"C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Audio\\sorry_dave.wav", L"output.wmv", vs);
            //VideoEncoder *ve = new VideoEncoder(L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Images\\paper-stained-3-texture.bmp", L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Audio\\sorry_dave.wav", L"output.wmv", vs);

            //const string imageFilePath = "C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Images\\paper-stained-3-texture.bmp";
            //const string audioFilePath = "C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Audio\\sorry_dave.wav";
            const string imageFilePath = "C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\test.bmp";
            const string audioFilePath = "C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\sorry_dave.wav";
            const string videoOutputPath = "output.wmv";

            if (File.Exists(videoOutputPath))
                File.Delete(videoOutputPath);

            var encoder = new VideoEncoder();
            encoder.ImageFilePath = imageFilePath;
            encoder.AudioFilePath = audioFilePath;
            encoder.VideoOutputPath = videoOutputPath;

            encoder.Encode();

            Assert.True(File.Exists(videoOutputPath));

            var fi = new FileInfo(videoOutputPath);

            Assert.NotEqual(0, fi.Length);
        }
    }
}
