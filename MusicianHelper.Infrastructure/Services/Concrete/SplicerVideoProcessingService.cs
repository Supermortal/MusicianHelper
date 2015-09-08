using System;
using System.Collections.Generic;
using log4net;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models.Enums;
using MusicianHelper.Infrastructure.Services.Abstract;
using Splicer.Renderer;
using Splicer.Timeline;
using Splicer.WindowsMedia;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class SplicerVideoProcessingService : IVideoProcessingService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (SplicerVideoProcessingService));

        public void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputPath, VideoQuality vq, EventHandler renderCompleted = null)
        {
            try
            {
                using (ITimeline timeline = new DefaultTimeline())
                {
                    var profile = GetProfile(vq);

                    var group = timeline.AddVideoGroup(32, profile.Width, profile.Height);
                    var videoTrack = group.AddTrack();
                    var audioTrack = timeline.AddAudioGroup().AddTrack();

                    var audio = audioTrack.AddAudio(audioPath);

                    var imageDuration = audio.Duration/imagePaths.Count;

                    foreach (var imagePath in imagePaths)
                    {
                        videoTrack.AddImage(imagePath, 0, 0, 0, imageDuration);
                    }

                    //IRenderer renderer = new AviFileRenderer(timeline, outputPath);
                    IRenderer renderer = new WindowsMediaRenderer(timeline, outputPath, profile.Profile);

                    if (renderCompleted != null)
                        renderer.RenderCompleted += renderCompleted;

                    renderer.Render();
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                if (renderCompleted != null)
                    renderCompleted(ex, EventArgs.Empty);
            }
        }

        private static WMVProfile GetProfile(VideoQuality vq)
        {
            var profile = new WMVProfile();

            switch (vq)
            {
                case VideoQuality.FULLHD2_NTSC:
                    profile.Profile = WindowsMediaProfiles.FULLHD2_NTSC;
                    profile.Width = 1920;
                    profile.Height = 1080;
                    break;
                case VideoQuality.FullHD:
                    profile.Profile = WindowsMediaProfiles.FullHD;
                    profile.Width = 1920;
                    profile.Height = 1080;
                    break;
                case VideoQuality.HD2_NTSC:
                    profile.Profile = WindowsMediaProfiles.HD2_NTSC;
                    profile.Width = 1280;
                    profile.Height = 720;
                    break;
                case VideoQuality.HighQuality2_NTSC:
                    profile.Profile = WindowsMediaProfiles.HighQuality2_NTSC;
                    profile.Width = 640;
                    profile.Height = 360;
                    break;
                case VideoQuality.HighQuality_NTSC:
                    profile.Profile = WindowsMediaProfiles.HighQuality_NTSC;
                    profile.Width = 1440;
                    profile.Height = 1080;
                    break;
                case VideoQuality.HighQuality_PAL:
                    profile.Profile = WindowsMediaProfiles.HighQuality_PAL;
                    profile.Width = 1440;
                    profile.Height = 1080;
                    break;
                case VideoQuality.SplicerDefaultHighQualityVideo:
                    profile.Profile = WindowsMediaProfiles.HighQualityVideo;
                    profile.Width = 320;
                    profile.Height = 240;
                    break;
                case VideoQuality.SplicerDefaultLowQualityAudio:
                    profile.Profile = WindowsMediaProfiles.LowQualityAudio;
                    profile.Width = 0;
                    profile.Height = 0;
                    break;
                case VideoQuality.SplicerDefaultLowQualityVideo:
                    profile.Profile = WindowsMediaProfiles.LowQualityVideo;
                    profile.Width = 0;
                    profile.Height = 0;
                    break;
                case VideoQuality.SplicerDefaultMediumQualityAudio:
                    profile.Profile = WindowsMediaProfiles.MediumQualityAudio;
                    profile.Width = 0;
                    profile.Height = 0;
                    break;
                case VideoQuality.StandardQuality:
                    profile.Profile = WindowsMediaProfiles.StandardQuality;
                    profile.Width = 1280;
                    profile.Height = 720;
                    break;
            }

            return profile;
        }
    }

    class WMVProfile
    {
        public string Profile { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
    }
}
