using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MusicianHelper.Infrastructure.Models.Enums;
using Splicer.WindowsMedia;

namespace MusicianHelper.Infrastructure.Models
{
    public class SplicerWMVProfile
    {
        public string Profile { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }

        public static SplicerWMVProfile GetProfile(VideoQuality vq)
        {
            var profile = new SplicerWMVProfile();

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
}
