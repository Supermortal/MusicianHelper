using System;
using System.Collections.Generic;
using System.IO;
using MusicianHelper.Common;
using MusicianHelper.Common.Concrete;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Models.Enums;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;
using MusicianHelper.Infrastructure.Services.Concrete;

ram
    {
        private static string BaseDir
        {
            get
            {
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
            //var vs = IoCHelper.Instance.GetService<IVideoProcessingService>();

            //System.Console.WriteLine("Rendering started...");
            //vs.CreateVideoFromImages(new List<string>() { GetPath("SeedData\\paper-stained-3-texture.jpg") }, GetPath("SeedData\\sorry_dave.wav"), "TestFile.wmv", VideoQuality.HD2_NTSC,
            //    (sender, args) =>
            //    {
            //        System.Console.WriteLine("Rendering completed!");
            //    });

            var vms = IoCHelper.Instance.GetService<IVideoManagementService>();
            vms.Configure(new VideoManagmentSettings() { RenderedVideoDirectory = "RenderedVideos" });

            System.Console.WriteLine("Rendering started...");
            vms.CreateVideoFromImages(new List<string>() { GetPath("SeedData\\paper-stained-3-texture.jpg") }, GetPath("SeedData\\sorry_dave.wav"), "TestFile", string.Empty,
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
            IoCHelper.Instance.BindService<IVideoProcessingService, SplicerWMVVideoProcessingService>();
            IoCHelper.Instance.BindService<IVideoManagementService, DefaultVideoManagementService>();
            IoCHelper.Instance.BindService<IVideoNetworkService, YouTubeVideoNetworkService>(IoCBindingType.Normal, "YouTubeNetwork");
        }
    }
}
