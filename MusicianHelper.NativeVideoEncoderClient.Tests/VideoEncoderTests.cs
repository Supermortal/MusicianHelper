using System;
using System.Collections.Generic;
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
    }
}
