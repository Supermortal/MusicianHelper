using System;
using System.Collections.Generic;
using MusicianHelper.Infrastructure.Models.Enums;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IVideoProcessingService
    {
        void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputPath, VideoQuality vq, EventHandler renderCompleted = null);
    }
}
