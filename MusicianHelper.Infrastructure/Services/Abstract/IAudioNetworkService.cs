using System;
using System.Collections.Generic;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{

    public delegate void AudioUploadedEventHandler(object sender, AudioUploadedEventArgs e);

    public interface IAudioNetworkService
    {
        Uri CreateRequestUri();
        string GetRedirectUrl();
        string ExtractAuthToken(string returnString);
        OauthTokenModel GetRequestTokens(string authToken);
        void SaveOauthResponse(OauthTokenModel otm);
        bool? HasCredentials();
        void UploadAudio(AudioUoW audio, OauthTokenModel otm, List<AudioUploadedEventHandler> audioUploaded = null);
    }
}
