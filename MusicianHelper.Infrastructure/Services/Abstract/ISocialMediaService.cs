using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface ISocialMediaService
    {
        Uri CreateRequestUri();
        string GetRedirectUrl();
        string ExtractAuthToken(string returnString);
        OauthTokenModel GetRequestTokens(string authToken);
        void SaveOauthResponse(OauthTokenModel otm);
        bool? HasCredentials();
        void MakePost(OauthTokenModel otm, AudioUoW audio);
    }
}
