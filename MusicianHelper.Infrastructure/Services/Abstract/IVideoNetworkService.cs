using System;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IVideoNetworkService
    {
        string ExtractAuthToken(string returnString);
        Uri CreateRequestUri();
        YouTubeOauthTokenModel GetRequestTokens(string authToken);
        void SaveOauthResponse(YouTubeOauthTokenModel otm);
        void UploadVideo(string videoPath, YouTubeOauthTokenModel otm);
    }
}
