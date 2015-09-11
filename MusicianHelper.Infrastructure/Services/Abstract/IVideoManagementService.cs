using System;
using System.Collections.Generic;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IVideoManagementService
    {
        string GetRenderDirectory();

        void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputFilename,
            string renderedDirectory, EventHandler renderCompleted = null);
        void Configure(VideoManagmentSettings settings);
        void SetRenderDirectory(string path);
        void SetAudioDirectory(string path);
        void SetImagesDirectory(string path);
        string GetAudioDirectory();
        string GetImagesDirectory();
        void CreateVideo(string imagesDirectoryPath, string audioPath, string renderDirectory, EventHandler renderCompleted = null);
        void CreateAllVideos(string imagesDirectory, string audioDirectory, string renderDirectory);
        string CreateRenderedVideoName(string audioPath);
    }
}
