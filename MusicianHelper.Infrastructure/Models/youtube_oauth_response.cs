﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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

        public OauthTokenModel ToOauthTokenModel()
        {
            return new OauthTokenModel() { AccessToken = access_token, RefreshToken = refresh_token };
        }
    }
}
