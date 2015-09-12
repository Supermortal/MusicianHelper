using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MusicianHelper.Common;
using MusicianHelper.Common.Concrete;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Models.Enums;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;
using MusicianHelper.Infrastructure.Services.Concrete;
using Newtonsoft.Json;

namespace MusicianHelper.APIKeyBootstrapper
{
    class Program
    {
        static void Main(string[] args)
        {
            BootstrapCommon();

            var aks = IoCHelper.Instance.GetService<IAPIKeyService>();
            var ss = IoCHelper.Instance.GetService<IStorageService>();

            var path = Path.Combine(ss.GetBaseDirectory(), "api_keys.json");
            var json = File.ReadAllText(path);
            var apiKeys = JsonConvert.DeserializeObject<APIKeysModel>(json);

            aks.BootstrapAPIKeys(apiKeys);

            File.Delete(path);
        }

        private static void BootstrapCommon()
        {
            string appDomain = AppDomain.CurrentDomain.BaseDirectory;
            string configPath = Path.Combine(appDomain, "log4net.Release.config");
#if DEBUG
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
            IoCHelper.Instance.BindService<IAPIKeyService, DefaultAPIKeyService>();
        }
    }
}
