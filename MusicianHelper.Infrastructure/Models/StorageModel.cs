using System;

namespace MusicianHelper.Infrastructure.Models
{
    public class StorageModel
    {
        public string YouTubeAccessToken { get; set; }
        public string YouTubeRefreshToken { get; set; }
        public string YouTubeTokenType { get; set; }
        public DateTime? YouTubeAccessTokenExpiry { get; set; }
        public string SoundCloudAccessToken { get; set; }
        public string SoundCloudRefreshToken { get; set; }
        public string SoundCloudTokenType { get; set; }
        public DateTime? SoundCloudAccessTokenExpiry { get; set; }
        public string FacebookAccessToken { get; set; }
        public string FacebookRefreshToken { get; set; }
        public string FacebookTokenType { get; set; }
        public DateTime? FacebookAccessTokenExpiry { get; set; }
        public string YouTubeClientId { get; set; }
        public string YouTubeClientSecret { get; set; }
        public string LastAudioDirectory { get; set; }
        public string LastVideoDirectory { get; set; }
        public string LastImagesDirectory { get; set; }
        public string SoundCloudClientId { get; set; }
        public string SoundCloudClientSecret { get; set; }
        public string FacebookClientId { get; set; }
        public string FacebookClientSecret { get; set; }

        public OauthTokenModel ToYouTubeOauthTokenModel()
        {
            return new OauthTokenModel() { AccessToken = YouTubeAccessToken, RefreshToken = YouTubeRefreshToken, AccessTokenExpiry = YouTubeAccessTokenExpiry, TokenType = YouTubeTokenType };
        }

        public OauthTokenModel ToSoundCloudOauthTokenModel()
        {
            return new OauthTokenModel() { AccessToken = SoundCloudAccessToken, AccessTokenExpiry = SoundCloudAccessTokenExpiry, RefreshToken = SoundCloudRefreshToken, TokenType = SoundCloudTokenType};
        }

        public OauthTokenModel ToFacebookOauthTokenModel()
        {
            return new OauthTokenModel() { AccessToken = FacebookAccessToken, AccessTokenExpiry = FacebookAccessTokenExpiry, RefreshToken = FacebookRefreshToken, TokenType = FacebookTokenType };
        }

        public APIKeysModel ToAPIKeysModel()
        {
            return new APIKeysModel() { YouTubeClientId = YouTubeClientId, YouTubeClientSecret = YouTubeClientSecret, SoundCloudClientId = SoundCloudClientId, SoundCloudClientSecret = SoundCloudClientSecret, FacebookClientId = FacebookClientId, FacebookClientSecret = FacebookClientSecret };
        }
    }
}
