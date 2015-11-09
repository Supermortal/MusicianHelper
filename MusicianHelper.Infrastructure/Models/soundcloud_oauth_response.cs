using System;

namespace MusicianHelper.Infrastructure.Models
{
    public class soundcloud_oauth_response
    {
        public string access_token {get; set; }
        public string scope { get; set; }

        public OauthTokenModel ToOauthTokenModel()
        {
            var ytm = new OauthTokenModel
            {
                AccessToken = access_token,
                TokenType = scope,
                AccessTokenExpiry = DateTime.MaxValue
            };

            return ytm;
        }
    }
}
