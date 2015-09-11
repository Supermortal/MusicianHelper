using System;
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
    public class YouTubeVideoNetworkService : IVideoNetworkService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof(YouTubeVideoNetworkService));

        private const string YOUTUBE_URL = "https://accounts.google.com/o/oauth2/auth?client_id=392497808537-qddvd51c0qt055749pggpjrp45qi4s0b.apps.googleusercontent.com&redirect_uri=urn:ietf:wg:oauth:2.0:oob&response_type=code&scope=https://www.googleapis.com/auth/youtube.upload";
        private const string OAUTH_URL = "https://accounts.google.com/o/oauth2/token";

        private readonly IStorageService _ss;

        private OauthTokenModel _otm = null;

        public YouTubeVideoNetworkService()
            : this(IoCHelper.Instance.GetService<IStorageService>())
        {

        }

        public YouTubeVideoNetworkService(IStorageService ss)
        {
            _ss = ss;
        }

        public string ExtractAuthToken(string returnString)
        {
            try
            {
                var parts = returnString.Split('=');
                return parts[1];
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public Uri CreateRequestUri()
        {
            try
            {
                Uri uri;
                Uri.TryCreate(YOUTUBE_URL, UriKind.Absolute, out uri);
                return uri;
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

                //code	The authorization code that Google returned to your redirect_uri in step 4.
                //client_id	The OAuth 2.0 client ID for your application. This value is displayed in the Google APIs console.
                //client_secret	The client secret associated with your client ID. This value is displayed in the Google APIs console.
                //redirect_uri	A registered redirect_uri for your client ID.
                //grant_type	Set this value to authorization_code.

                using (var wb = new WebClient())
                {
                    var data = new NameValueCollection();
                    data["code"] = authToken;
                    data["client_id"] = "392497808537-qddvd51c0qt055749pggpjrp45qi4s0b.apps.googleusercontent.com";
                    data["client_secret"] = "bpf3zMCJwwdjiUK6pW9mzs-X";
                    data["redirect_uri"] = "urn:ietf:wg:oauth:2.0:oob";
                    data["grant_type"] = "authorization_code";

                    var response = wb.UploadValues(OAUTH_URL, "POST", data);
                    var responseStr = Encoding.ASCII.GetString(response);
                    var oauthResponse = JsonConvert.DeserializeObject<youtube_oauth_response>(responseStr);

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

                sm.AccessToken = otm.AccessToken;
                sm.RefreshToken = otm.RefreshToken;

                _ss.Save(sm);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        public void Get(OauthTokenModel otm)
        {
            try
            {
                using (var wb = new WebClient())
                {
                    var response = wb.DownloadData("https://www.googleapis.com/youtube/v3/channels?part=id&mine=true&access_token=" + otm.AccessToken);
                    var responseStr = Encoding.ASCII.GetString(response);
                    var t = "t";
                    //var oauthResponse = JsonConvert.DeserializeObject<youtube_oauth_response>(responseStr);

                    //return oauthResponse.ToOauthTokenModel();
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        public OauthTokenModel GetOauthTokenModel()
        {
            try
            {
                return _otm ?? (_otm = _ss.Load().ToOauthTokenModel());
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

    }
}
