﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IAPIKeyService
    {
        APIKeysModel GetAPIKeys();
        void BootstrapAPIKeys(APIKeysModel akm);
    }
}
