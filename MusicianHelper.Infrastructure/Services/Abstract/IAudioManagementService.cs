using System;
using System.Collections.Generic;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{

    public delegate void AllAudiosUploadedEventHandler(object sender, EventArgs e);

    public interface IAudioManagementService
    {
        void UploadAllAudios(List<AudioUoW> audios, OauthTokenModel otm, AllAudiosUploadedEventHandler allAudiosUploaded = null, AudioUploadedEventHandler audioUploaded = null, Action<string> feedbackMethod = null);
    }
}
