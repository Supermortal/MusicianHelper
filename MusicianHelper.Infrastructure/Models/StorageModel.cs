﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
