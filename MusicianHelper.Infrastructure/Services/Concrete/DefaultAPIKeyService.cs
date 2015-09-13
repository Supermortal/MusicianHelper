using System;
using System.Text;
using log4net;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class DefaultAPIKeyService : IAPIKeyService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (DefaultAPIKeyService));

        private readonly IStorageService _ss;

        private APIKeysModel _akm = null;
        private string basePath = null;

        public DefaultAPIKeyService() : this(IoCHelper.Instance.GetService<IStorageService>())
        {
            
        }

        public DefaultAPIKeyService(IStorageService ss)
        {
            _ss = ss;
        }

        public APIKeysModel GetAPIKeys()
        {
            try
            {
                if (_akm != null) return _akm;

                _akm = _ss.Load().ToAPIKeysModel();

                _akm.YouTubeClientSecret =
                    Encoding.Default.GetString(Security.SymmetricDecrypt(_akm.YouTubeClientSecret));
                _akm.YouTubeClientId = Encoding.Default.GetString(Security.SymmetricDecrypt(_akm.YouTubeClientId));

                _akm.YouTubeClientSecret = _akm.YouTubeClientSecret.Replace("\0", string.Empty);
                _akm.YouTubeClientId = _akm.YouTubeClientId.Replace("\0", string.Empty);

                return _akm;
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public void BootstrapAPIKeys(APIKeysModel akm)
        {
            try
            {
                akm.YouTubeClientSecret = Security.SymmetricEncypt(Encoding.Default.GetBytes(akm.YouTubeClientSecret));
                akm.YouTubeClientId = Security.SymmetricEncypt(Encoding.Default.GetBytes(akm.YouTubeClientId));

                var sm = _ss.Load();
                akm.UpdateStorageModel(sm);
                _ss.Save(sm);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

    }
}
