using System;

namespace MusicianHelper.Infrastructure.Models
{
    public class OauthTokenModel
    {
        public string AccessToken { get; set; }
        public string RefreshToken { get; set; }
        public string TokenType { get; set; }
        public DateTime? AccessTokenExpiry { get; set; }

        public bool? AccessTokenExpired
        {
            get
            {
                if (AccessTokenExpiry == null)
                    return null;

                return DateTime.UtcNow >= AccessTokenExpiry;
            }
        }

        public void UpdateYouTubeStorage(StorageModel sm)
        {
            sm.YouTubeAccessToken = AccessToken;
            sm.YouTubeTokenType = TokenType;
            sm.YouTubeRefreshToken = RefreshToken;
            sm.YouTubeAccessTokenExpiry = AccessTokenExpiry;
        }

        public void UpdateSoundCloudStorage(StorageModel sm)
        {
            sm.SoundCloudAccessToken = AccessToken;
            sm.SoundCloudTokenType = TokenType;
            sm.SoundCloudRefreshToken = RefreshToken;
            sm.SoundCloudAccessTokenExpiry = AccessTokenExpiry;
        }

        //public SCAccessToken ToSCAccessToken()
        //{
        //    return new SCAccessToken() { AccessToken = AccessToken, ExpiresIn = (DateTime)AccessTokenExpiry - DateTime.Now, RefreshToken = RefreshToken, Scope = SCScope.NonExpiring };
        //}
    }
}
