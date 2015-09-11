using System;
using System.Collections.Generic;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IVideoManagementService
    {
        string GetRenderDirectory();
        void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputFilename, EventHandler renderCompleted = null);
        void Configure(VideoManagmentSettings settings);
        void SetRenderDirectory(string path);
        void SetAudioDirectory(string path);
        void SetImagesDirectory(string path);
        string GetAudioDirectory();
        string GetImagesDirectory();
    }
}
