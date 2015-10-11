using System;
using System.IO;
using System.Windows.Forms;
using MusicianHelper.Common;
using MusicianHelper.Common.Concrete;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Models.Enums;
using MusicianHelper.Infrastructure.Services.Abstract;
using MusicianHelper.Infrastructure.Services.Concrete;

namespace MusicianHelper.WinForms
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            BootstrapCommon();

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainWindow());
        }

        private static void BootstrapCommon()
        {
            string appDomain = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Musician Helper");
            string configPath = Path.Combine(appDomain, "log4net.Release.config");
#if DEBUG
            appDomain = AppDomain.CurrentDomain.BaseDirectory;
            configPath = Path.Combine(appDomain, "log4net.Debug.config");
#endif

            Bootstraper.Start(new NinjectIoCHelper(), configPath);

            AddBindings();
        }

        private static void AddBindings()
        {
            IoCHelper.Instance.BindService<IVideoProcessingService, SplicerWMVVideoProcessingService>();
            IoCHelper.Instance.BindService<IVideoManagementService, DefaultVideoManagementService>();
            IoCHelper.Instance.BindService<IVideoNetworkService, YouTubeVideoNetworkService>(IoCBindingType.Normal, "YouTubeNetwork");
            IoCHelper.Instance.BindService<IStorageService, JsonNetStorageService>(IoCBindingType.Singleton);
            IoCHelper.Instance.BindService<IAPIKeyService, DefaultAPIKeyService>(IoCBindingType.Singleton);
            IoCHelper.Instance.BindService<IMasterService, DefaultMasterService>();
            IoCHelper.Instance.BindService<IAudioNetworkService, SoundCloudAudioNetworkService>(IoCBindingType.Normal, "SoundCloudNetwork");
            IoCHelper.Instance.BindService<IAudioManagementService, DefaultAudioManagementService>();
            IoCHelper.Instance.BindService<ISocialMediaService, FacebookSocialMediaService>(IoCBindingType.Normal, "FacebookSocialMedia");
        }
    }
}
