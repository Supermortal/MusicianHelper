using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using log4net;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class DefaultAudioManagementService : IAudioManagementService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (DefaultAudioManagementService));

        private readonly Dictionary<string, AudioUploadSession> _sessions = new Dictionary<string, AudioUploadSession>();
        private readonly IAudioNetworkService _ans;

        public DefaultAudioManagementService(IAudioNetworkService ans)
        {
            _ans = ans;
        }

        public void UploadAllAudios(List<AudioUoW> audios, OauthTokenModel otm, AllAudiosUploadedEventHandler allAudiosUploaded = null,
            AudioUploadedEventHandler audioUploaded = null, Action<string> feedbackMethod = null)
        {
            try
            {
                var sessionId = Guid.NewGuid().ToString();
                _sessions.Add(sessionId, new AudioUploadSession() { Count = 0, AllAudiosUploaded = allAudiosUploaded });
                foreach (var audio in audios)
                {
                    audio.SessionId = sessionId;

                    var list = new List<AudioUploadedEventHandler>();
                    if (audioUploaded != null)
                        list.Add(audioUploaded);
                    list.Add(AudioUploaded);

                    if (feedbackMethod != null)
                        feedbackMethod("Upload started... " + audio.Title);

                    _sessions[sessionId].Count++;
                    _ans.UploadAudio(audio, otm, list);
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void AudioUploaded(object sender, AudioUploadedEventArgs e)
        {
            try
            {
                var session = _sessions[e.Audio.SessionId];
                session.Count--;

                if (session.Count > 0) return;

                session.AllAudiosUploaded(this, EventArgs.Empty);
                //_sessions.Remove(e.Audio.SessionId);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

    }
}
