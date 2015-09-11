using System;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IVideoNetworkService
    {
        string ExtractAuthToken(string returnString);
        Uri CreateRequestUri();
        void GetRequestTokens();
    }
}
