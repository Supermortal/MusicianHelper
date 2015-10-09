using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Net;
using System.Text;
using log4net;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;
using Newtonsoft.Json;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class SoundCloudAudioNetworkService : IAudioNetworkService
    {
        private static readonly ILog Log = LogHelper.GetLogger(typeof(SoundCloudAudioNetworkService));

        private const string SOUNDCLOUD_URL = "https://soundcloud.com/connect";
        private const string OAUTH_URL = "https://api.soundcloud.com/oauth2/token";
        private const string REDIRECT_URI = "musician-helper://soundcloud/callback";

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
            try
            {
                var authUrl = CreateRequestUri();
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        public Uri CreateRequestUri()
        {
            try
            {
                var akm = _aks.GetAPIKeys();

                var dict = new Dictionary<string, object>()
                {
                    {"client_id", akm.SoundCloudClientId},
                    {"redirect_uri", REDIRECT_URI},
                    {"response_type", "code"},
                    {"scope", "non-expiring"}
                };

                var sb = new StringBuilder();
                sb.Append(SOUNDCLOUD_URL);
                sb.Append(WebHelper.ToQueryString(dict));

                Uri uri;
                Uri.TryCreate(sb.ToString(), UriKind.Absolute, out uri);

                return uri;
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public string GetRedirectUrl()
        {
            try
            {
                return REDIRECT_URI;
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public string ExtractAuthToken(string returnString)
        {
            try
            {
                var parts = returnString.Split('=');
                return parts[1].Replace("#", string.Empty);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public OauthTokenModel GetRequestTokens(string authToken)
        {
            try
            {
                if (string.IsNullOrEmpty(authToken))
                    return null;

                var akm = _aks.GetAPIKeys();

                using (var wb = new WebClient())
                {
                    var data = new NameValueCollection();
                    data["client_id"] = akm.SoundCloudClientId;
                    data["client_secret"] = akm.SoundCloudClientSecret;
                    data["redirect_uri"] = REDIRECT_URI;
                    data["grant_type"] = "authorization_code";
                    data["code"] = authToken;

                    var response = wb.UploadValues(OAUTH_URL, "POST", data);
                    var responseStr = Encoding.ASCII.GetString(response);
                    var oauthResponse = JsonConvert.DeserializeObject<soundcloud_oauth_response>(responseStr);

                    return oauthResponse.ToOauthTokenModel();
                }

            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public void SaveOauthResponse(OauthTokenModel otm)
        {
            try
            {
                var sm = _ss.Load();
                otm.UpdateSoundCloudStorage(sm);
                _ss.Save(sm);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        public bool? HasCredentials()
        {
            try
            {
                var sm = _ss.Load();
                return (sm.SoundCloudAccessToken != null);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

    }
}
