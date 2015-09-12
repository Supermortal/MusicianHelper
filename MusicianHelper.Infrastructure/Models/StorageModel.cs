namespace MusicianHelper.Infrastructure.Models
{
    public class StorageModel
    {
        public string AccessToken { get; set; }
        public string RefreshToken { get; set; }
        public string YouTubeClientId { get; set; }
        public string YouTubeClientSecret { get; set; }

        public YouTubeOauthTokenModel ToOauthTokenModel()
        {
            return new YouTubeOauthTokenModel() { AccessToken = AccessToken, RefreshToken = RefreshToken };
        }

        public APIKeysModel ToAPIKeysModel()
        {
            return new APIKeysModel() { YouTubeClientId = YouTubeClientId, YouTubeClientSecret = YouTubeClientSecret };
        }

        public void UpdateAPIKeys(APIKeysModel akm)
        {
            YouTubeClientId = akm.YouTubeClientId;
            YouTubeClientSecret = akm.YouTubeClientSecret;
        }
    }
}
