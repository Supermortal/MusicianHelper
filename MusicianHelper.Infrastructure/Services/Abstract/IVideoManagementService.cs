using System;
using System.Collections.Generic;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{

    public delegate void AllVideosRenderedEventHandler(object sender, EventArgs e);
    public delegate void AllVideosUploadedEventHandler(object sender, EventArgs e);

    public interface IVideoManagementService
    {
        string GetRenderDirectory();
        void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputFilename,
            string renderedDirectory, VideoRenderedEventHandler renderCompleted = null);
        void Configure(VideoManagmentSettings settings);
        void SetRenderDirectory(string path);
        void SetAudioDirectory(string path);
        void SetImagesDirectory(string path);
        string GetAudioDirectory();
        string GetImagesDirectory();
        void CreateVideo(string imagesDirectoryPath, string audioPath, string renderDirectory, VideoRenderedEventHandler renderCompleted = null);
        void CreateAllVideos(List<string> imagePaths, List<string> audioPaths, string renderDirectory);
        string CreateRenderedVideoName(string audioPath);
        List<string> GetImagePaths(string imageDirectory);
        void CreateAllVideos(List<AudioUoW> audios, string imageDirectory, string renderDirectory, AllVideosRenderedEventHandler allVideosRendered = null, VideoRenderedEventHandler renderCompleted = null, Action<string> feedbackMethod = null);
        string MakeValidFileName(string name);
        List<string> GetAllVideoPaths(string renderedDirectory);
        void DeleteAllRenderedVideos(string renderedDirectory);
        void UploadVideo(AudioUoW audio, OauthTokenModel otm, VideoUploadedEventArgs videoUploaded = null);
        void UploadAllVideos(List<AudioUoW> audios, OauthTokenModel otm, AllVideosUploadedEventHandler allVideosUploaded = null, VideoUploadedEventHandler videoUploaded = null, Action<string> feedbackMethod = null);
    }
}
