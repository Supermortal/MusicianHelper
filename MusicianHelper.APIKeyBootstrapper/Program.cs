using System;
using System.IO;
using System.Text;
using MusicianHelper.Common;
using MusicianHelper.Common.Concrete;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Helpers.Log;
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

      var path = Path.Combine(GetBaseDirectory(), "api_keys.json");
      var json = File.ReadAllText(path);
      var apiKeys = JsonConvert.DeserializeObject<APIKeysModel>(json);

      aks.BootstrapAPIKeys(apiKeys);

#if DEBUG
      return;
#endif

      File.Delete(path);
    }

    private static void BootstrapCommon()
    {
      string appDomain = Environment.CurrentDirectory;
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
      IoCHelper.Instance.BindService<IStorageService, BootstrapperStorageService>(IoCBindingType.Singleton);
      IoCHelper.Instance.BindService<IAPIKeyService, DefaultAPIKeyService>();
    }

    private static string GetBaseDirectory()
    {
#if DEBUG
                    var currentDir = Environment.CurrentDirectory;
                    var parts = currentDir.Split('\\');
                    var sb = new StringBuilder();
                    for (int i = 0; i < parts.Length; i++)
                    {
                        var part = parts[i];

                        sb.Append(part);
                        sb.Append('\\');
                        if (part == Names.TopLevelDirectory)
                        {
                            break;
                        }
                    }

                    return sb.ToString().TrimEnd('\\');
#endif

      return Environment.CurrentDirectory;
    }

  }
}
