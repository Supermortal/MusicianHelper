using System;
using System.Collections.Generic;
using System.IO;
using MusicianHelper.Common;
using MusicianHelper.Common.Concrete;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Infrastructure.Models.Enums;
using MusicianHelper.Infrastructure.Services.Abstract;
using MusicianHelper.Infrastructure.Services.Concrete;

namespace MusicianHelper.Console
{
    class Program
    {

        private const string OutputFile = "FadeBetweenImages.wmv";

        private static string BaseDir
        {
            get
            {
                //return AppDomain.CurrentDomain.BaseDirectory;
                return Environment.CurrentDirectory;
            }
        }

        private static string GetPath(string filename)
        {
            return Path.Combine(BaseDir, filename);
        }

        static void Main()
        {
            BootstrapCommon();
            Start();
        }

        public static void Start()
        {
            //using (ITimeline timeline = new DefaultTimeline())
            //{
            //    IGroup group = timeline.AddVideoGroup(32, 160, 100);

            //    ITrack videoTrack = group.AddTrack();
            //    //IClip clip1 = videoTrack.AddImage(GetPath("SeedData\\test.jpg")); // play first image for a little while
            //    //IClip clip2 = videoTrack.AddImage(GetPath("SeedData\\test_location.jpg")); // and the next
            //    //IClip clip3 = videoTrack.AddImage("image3.jpg", 0, 2); // and finally the last
            //    //IClip clip4 = videoTrack.AddImage("image4.jpg", 0, 2); // and finally the last

            //    double halfDuration = 0.5;

            //    // fade out and back in
            //    //group.AddTransition(clip2.Offset - halfDuration, halfDuration, StandardTransitions.CreateFade(), true);
            //    //group.AddTransition(clip2.Offset, halfDuration, StandardTransitions.CreateFade(), false);

            //    // again
            //    //group.AddTransition(clip3.Offset - halfDuration, halfDuration, StandardTransitions.CreateFade(), true);
            //    //group.AddTransition(clip3.Offset, halfDuration, StandardTransitions.CreateFade(), false);

            //    //// and again
            //    //group.AddTransition(clip4.Offset - halfDuration, halfDuration, StandardTransitions.CreateFade(), true);
            //    //group.AddTransition(clip4.Offset, halfDuration, StandardTransitions.CreateFade(), false);

            //    // add some audio
            //    ITrack audioTrack = timeline.AddAudioGroup().AddTrack();

            //    IClip audio =
            //    audioTrack.AddAudio(GetPath("SeedData\\sorry_dave.wav"));

            //    videoTrack.AddImage(GetPath("SeedData\\test.jpg"), 0, 0, 0, audio.Duration);

            //    // create an audio envelope effect, this will:
            //    // fade the audio from 0% to 100% in 1 second.
            //    // play at full volume until 1 second before the end of the track
            //    // fade back out to 0% volume
            //    //audioTrack.AddEffect(0, audio.Duration,
            //    //               StandardEffects.CreateAudioEnvelope(1.0, 1.0, 1.0, audio.Duration));               

            //    // render our slideshow out to a windows media file
            //    //using (
            //    //IRenderer renderer = new AviFileRenderer(timeline, OutputFile, StandardFilters.);
            //    IRenderer renderer =
            //        new WindowsMediaRenderer(timeline, OutputFile, WindowsMediaProfiles.HighQualityVideo);
            //    //{
            //        renderer.Render();

            //        System.Console.ReadLine();
            //}

            var vs = IoCHelper.Instance.GetService<IVideoProcessingService>();

            System.Console.WriteLine("Rendering started...");
            vs.CreateVideoFromImages(new List<string>() { GetPath("SeedData\\paper-stained-3-texture.jpg") }, GetPath("SeedData\\Jack_s_Four_Deep_in_Love_250k.mp3"), OutputFile, VideoQuality.HD2_NTSC,
                (sender, args) =>
                {
                    System.Console.WriteLine("Rendering completed!");
                });
            
            System.Console.ReadLine();
        }

        private static void BootstrapCommon()
        {
            string appDomain = AppDomain.CurrentDomain.BaseDirectory;
            string configPath = Path.Combine(appDomain, "log4net.Release.config");
#if DEBUG
            configPath = Path.Combine(appDomain, "log4net.Debug.config");
#endif

            Bootstraper.Start(new NinjectIoCHelper(), configPath);
            System.Console.Clear();

            AddBindings();
        }

        private static void AddBindings()
        {
            IoCHelper.Instance.BindService<IVideoProcessingService, SplicerVideoProcessingService>();
        }
    }
}
