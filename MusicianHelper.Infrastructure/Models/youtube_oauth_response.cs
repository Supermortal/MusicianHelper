using System;

namespace MusicianHelper.Infrastructure.Models
{
    public class youtube_oauth_response
    {
        public string access_token { get; set; }
        public string token_type { get; set; }
        public string refresh_token { get; set; }
        public string expires_in { get; set; }

        public OauthTokenModel ToOauthTokenModel()
        {
            var ytm = new OauthTokenModel
            {
                AccessToken = access_token,
                TokenType = token_type,
                RefreshToken = refresh_token,
                AccessTokenExpiry = DateTime.UtcNow.AddSeconds(int.Parse(expires_in))
            };

            return ytm;
        }
    }
}
