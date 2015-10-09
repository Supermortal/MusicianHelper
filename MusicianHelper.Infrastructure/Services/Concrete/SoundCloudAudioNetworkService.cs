using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using log4net;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class SoundCloudAudioNetworkService : IAudioNetworkService
    {
        private static readonly ILog Log = LogHelper.GetLogger(typeof(SoundCloudAudioNetworkService));

        private const string SOUNDCLOUD_URL = "https://soundcloud.com/connect";
        private const string OAUTH_URL = "https://accounts.google.com/o/oauth2/token";
        private const string REDIRECT_URI = "my-app://soundcloud/callback";

        private readonly IStorageService _ss;
        private readonly IAPIKeyService _aks;

        private OauthTokenModel _otm = null;

        public SoundCloudAudioNetworkService(IStorageService ss, IAPIKeyService aks)
        {
            _ss = ss;
            _aks = aks;
        }

        public void Authenticate()
        {
            
        }
    }
}
