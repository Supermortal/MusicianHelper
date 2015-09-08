using System;
using System.Collections.Generic;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Models.Enums;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IVideoManagementService
    {
        string GetRenderDirectory();
        void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputFilename, EventHandler renderCompleted = null);
        void Configure(VideoManagmentSettings settings);
    }
}
