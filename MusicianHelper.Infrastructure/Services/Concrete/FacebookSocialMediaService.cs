using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using Facebook;
using log4net;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;
using Newtonsoft.Json;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class FacebookSocialMediaService : ISocialMediaService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (FacebookSocialMediaService));

        private const string FACEBOOK_URL = "https://www.facebook.com/dialog/oauth?client_id={0}&redirect_uri={1}";
        private const string REDIRECT_URI = "https://www.facebook.com/connect/login_success.html";
        private const string OAUTH_URL =
            "https://graph.facebook.com/v2.3/oauth/access_token?client_id={0}&redirect_uri={1}&client_secret={2}&code={3}";

        private readonly IAPIKeyService _aks;
        private readonly IStorageService _ss;

        public FacebookSocialMediaService() : this(IoCHelper.Instance.GetService<IAPIKeyService>(), IoCHelper.Instance.GetService<IStorageService>())
        {
            
        }

        public FacebookSocialMediaService(IAPIKeyService aks, IStorageService ss)
        {
            _aks = aks;
            _ss = ss;
        }

        public void Authenticate()
        {
            var apiKeys = _aks.GetAPIKeys();
            var fc = new FacebookClient();

            fc.AppId = apiKeys.FacebookClientId;
            fc.AppSecret = apiKeys.FacebookClientSecret;
            //https://www.facebook.com/connect/login_success.html
        }

        public Uri CreateRequestUri()
        {
            try
            {
                var apiKeys = _aks.GetAPIKeys();

                Uri uri;
                Uri.TryCreate(string.Format(FACEBOOK_URL, apiKeys.FacebookClientId,
                    REDIRECT_URI), UriKind.Absolute, out uri);

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
            return REDIRECT_URI;
        }

        public string ExtractAuthToken(string returnString)
        {
            try
            {
                var parts = returnString.Split('=');
                var workingToken = parts[1];
                parts = workingToken.Split('#');
                return parts[0];
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
                    var response = wb.DownloadData(string.Format(OAUTH_URL, akm.FacebookClientId, REDIRECT_URI, akm.FacebookClientSecret, authToken));
                    var responseStr = Encoding.ASCII.GetString(response);
                    var oauthResponse = JsonConvert.DeserializeObject<facebook_oauth_response>(responseStr);

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
                otm.UpdateFacebookStorage(sm);
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
                var otm = sm.ToFacebookOauthTokenModel();
                return (sm.FacebookAccessToken != null && otm.AccessTokenExpired != null && !(bool)otm.AccessTokenExpired);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

    }
}
