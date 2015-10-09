using System;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IAudioNetworkService
    {
        Uri CreateRequestUri();
        string GetRedirectUrl();
        string ExtractAuthToken(string returnString);
        OauthTokenModel GetRequestTokens(string authToken);
        void SaveOauthResponse(OauthTokenModel otm);
        bool? HasCredentials();
    }
}
