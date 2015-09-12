namespace MusicianHelper.Infrastructure.Models
{
    public class youtube_oauth_response
    {
        public string access_token { get; set; }
        public string token_type { get; set; }
        public string refresh_token { get; set; }

        public void UpdateStorageModel(StorageModel sm)
        {
            sm.AccessToken = access_token;
            sm.RefreshToken = access_token;
        }

        public YouTubeOauthTokenModel ToOauthTokenModel()
        {
            return new YouTubeOauthTokenModel() { AccessToken = access_token, RefreshToken = refresh_token };
        }
    }
}
