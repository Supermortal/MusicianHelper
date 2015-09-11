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
    public class JsonNetStorageService : IStorageService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (JsonNetStorageService));

        private const string SETTINGS_FILE = "settings.json";

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
                var jsonStr = File.ReadAllText(GetSettingsFilePath());
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
            return Path.Combine(GetCurrentDirectory(), SETTINGS_FILE);
        }

    }
}
