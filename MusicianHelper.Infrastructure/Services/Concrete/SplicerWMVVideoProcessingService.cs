using System;
using System.Collections.Generic;
using System.Text;
using log4net;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Models.Enums;
using MusicianHelper.Infrastructure.Services.Abstract;
using Splicer.Renderer;
using Splicer.Timeline;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class SplicerWMVVideoProcessingService : IVideoProcessingService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (SplicerWMVVideoProcessingService));

        public void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputPath, VideoQuality vq, List<VideoRenderedEventHandler> renderCompleted = null)
        {
            try
            {
                CreateVideoFromImages(imagePaths, new AudioUoW() {Path = audioPath}, outputPath, vq, renderCompleted);
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
                using (ITimeline timeline = new DefaultTimeline())
                {
                    var profile = SplicerWMVProfile.GetProfile(vq);

                    var group = timeline.AddVideoGroup(32, profile.Width, profile.Height);
                    var videoTrack = group.AddTrack();
                    var audioTrack = timeline.AddAudioGroup().AddTrack();

                    var a = audioTrack.AddAudio(audio.Path);

                    var imageDuration = a.Duration/imagePaths.Count;

                    foreach (var imagePath in imagePaths)
                    {
                        videoTrack.AddImage(imagePath, InsertPosition.Relative, 0, 0, imageDuration);
                    }

                    IRenderer renderer = new WindowsMediaRenderer(timeline, outputPath, profile.Profile);

                    renderer.BeginRender(RenderingCompleted, new VideoRenderedAsyncState() { Renderer = renderer, Audio = audio, RenderCompleted = renderCompleted });
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                if (renderCompleted != null)
                {
                    foreach (var rc in renderCompleted)
                        rc(ex, new VideoRenderedEventArgs());
                }
            }
        }

        private void RenderingCompleted(IAsyncResult ar)
        {
            try
            {
                var vras = (VideoRenderedAsyncState) ar.AsyncState;

                var disposable = (IDisposable) vras.Renderer;
                disposable.Dispose();

                if (vras.RenderCompleted == null) return;

                foreach (var rc in vras.RenderCompleted)
                {
                    rc(this, new VideoRenderedEventArgs() {Audio = vras.Audio});
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

    }

    class VideoRenderedAsyncState
    {
        public AudioUoW Audio { get; set; }
        public object Renderer { get; set; }
        public List<VideoRenderedEventHandler> RenderCompleted { get; set; }
    }

}
