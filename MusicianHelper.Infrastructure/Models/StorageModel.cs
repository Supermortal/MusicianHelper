namespace MusicianHelper.Infrastructure.Models
{
    public class StorageModel
    {
        public string AccessToken { get; set; }
        public string RefreshToken { get; set; }

        public OauthTokenModel ToOauthTokenModel()
        {
            return new OauthTokenModel() { AccessToken = AccessToken, RefreshToken = RefreshToken };
        }
    }
}
