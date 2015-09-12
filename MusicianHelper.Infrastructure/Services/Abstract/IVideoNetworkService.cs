using System;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IVideoNetworkService
    {
        string ExtractAuthToken(string returnString);
        Uri CreateRequestUri();
        OauthTokenModel GetRequestTokens(string authToken);
        void SaveOauthResponse(OauthTokenModel otm);
        void UploadVideo(string videoPath, OauthTokenModel otm);
    }
}
