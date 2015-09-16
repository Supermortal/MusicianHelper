using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using log4net;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;
using Newtonsoft.Json;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
  public class BootstrapperStorageService : IStorageService
  {

    private static readonly ILog Log = LogHelper.GetLogger(typeof(JsonNetStorageService));

    private const string SETTINGS_FILE = "settings.json";

    private string _basePath = null;

    public void Save(StorageModel obj)
    {
      try
      {
        var jsonStr = JsonConvert.SerializeObject(obj);
        File.WriteAllText(GetSettingsFilePath(), jsonStr);
      }
      catch (Exception ex)
      {
        Log.Error(ex.Message, ex);
      }
    }

    public StorageModel Load()
    {
      try
      {
        var path = GetSettingsFilePath();
        if (!File.Exists(path))
          return new StorageModel();

        var jsonStr = File.ReadAllText(path);
        return JsonConvert.DeserializeObject<StorageModel>(jsonStr);
      }
      catch (Exception ex)
      {
        Log.Error(ex.Message, ex);
        return null;
      }
    }

    private static string GetCurrentDirectory()
    {
      return Environment.CurrentDirectory;
    }

    private string GetSettingsFilePath()
    {
      return Path.Combine(GetBaseDirectory(), SETTINGS_FILE);
    }

    public string GetBaseDirectory()
    {
      try
      {
#if DEBUG
                if (_basePath == null)
                {
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

                    _basePath = sb.ToString().TrimEnd('\\');
                }

                return _basePath;
#endif

        return Environment.CurrentDirectory;
      }
      catch (Exception ex)
      {
        Log.Error(ex.Message, ex);
        return null;
      }
    }

  }
}
