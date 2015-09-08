using NUnit.Framework;

namespace Splicer.Utilities.Audio.Tests
{
    [TestFixture]
    public class AudioCompressorFactoryFixture
    {
        [Test]
        public void Create()
        {
            using (
                AudioCompressor compressor =
                    AudioCompressorFactory.Create(AudioFormat.CompactDiscQualityStereoPcm))
            {
                Assert.IsNotNull(compressor.Filter);
                Assert.IsNotNull(compressor.MediaType);
            }
        }

        [Test]
        [ExpectedException(typeof (SplicerException))]
        public void CreateForInvalidBitrate()
        {
            var format = new AudioFormat(AudioFormat.PcmFriendlyName, true, 99, 88);
            AudioCompressorFactory.Create(format);
        }
    }
}