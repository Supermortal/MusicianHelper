using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using log4net;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Models.Enums;
using MusicianHelper.Infrastructure.Services.Abstract;
using MusicianHelper.NativeVideoEncoderProcess;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class NativeVideoEncoderProcessVideoProcessingService : IVideoProcessingService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (NativeVideoEncoderProcessVideoProcessingService));

        public void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputPath, VideoQuality vq,
            List<VideoRenderedEventHandler> renderCompleted = null)
        {
            throw new NotImplementedException();
        }

        public string GetCorrectFilename(string outputFilename)
        {
            try
            {
                if (outputFilename.EndsWith(".wmv"))
                    return outputFilename;

                var parts = outputFilename.Split('.');

                if (parts.Length > 1)
                    outputFilename = parts[0];

                var sb = new StringBuilder();

                sb.Append(outputFilename);
                sb.Append(".wmv");

                return sb.ToString();
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                throw;
            }
        }

        public void CreateVideoFromImages(List<string> imagePaths, AudioUoW audio, string outputPath, VideoQuality vq,
            List<VideoRenderedEventHandler> renderCompleted = null)
        {
            try
            {
                var vew = new VideoEncoderWrapper();
                vew.Encode(imagePaths[0], audio.AudioPath, outputPath);

                var t = "t";
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                throw;
            }
        }

    }
}
