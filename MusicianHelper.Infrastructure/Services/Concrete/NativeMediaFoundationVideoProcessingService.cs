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
using MusicianHelperNativeVideoEncoderLib;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class NativeMediaFoundationVideoProcessingService : IVideoProcessingService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof(NativeMediaFoundationVideoProcessingService));

        private readonly VideoEncoder _ve;

        public NativeMediaFoundationVideoProcessingService()
        {
            _ve = new VideoEncoder();
        }

        public void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputPath, VideoQuality vq,
            List<VideoRenderedEventHandler> renderCompleted = null)
        {
            try
            {
                //_ve.ImageFilePath = imagePaths[0];
                ////_ve.AudioFilePath = GetCorrectFilename(audioPath);
                ////_ve.VideoOutputPath = outputPath;

              System.Diagnostics.Debug.WriteLine("Testttt");
              var t = _ve.ImageFilePath;
                System.Diagnostics.Debug.WriteLine(t);

                //_ve.Encode();

                var s = "s";
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
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
                //_ve.SetImageFilePath(imagePaths[0]);
                //_ve.SetAudioFilePath(GetCorrectFilename(audio.AudioPath));
                //_ve.SetVideoOutputPath(outputPath);

                //_ve.Encode();
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

    }
}
