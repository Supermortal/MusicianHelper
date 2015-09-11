using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using log4net;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Models.Enums;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class DefaultVideoManagementService : IVideoManagementService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (DefaultVideoManagementService));

        private VideoManagmentSettings _vms = null;
        private readonly IVideoProcessingService _vps = null;

        private string _renderDirectory = null;

        private string AudioDirectory { get; set; }
        private string ImagesDirectory { get; set; }

        public DefaultVideoManagementService(IVideoProcessingService vps)
        {
            _vps = vps;
        }

        private static string BaseDir
        {
            get
            {
                return Environment.CurrentDirectory;
            }
        }

        public string GetRenderDirectory()
        {
            if (_renderDirectory != null) return _renderDirectory;

            var path = Path.Combine(BaseDir, _vms.RenderedVideoDirectory);
            if (!Directory.Exists(path))
                Directory.CreateDirectory(path);

            return (_renderDirectory = path);
        }

        public void SetRenderDirectory(string path)
        {
            _renderDirectory = path;
        }

        public void SetAudioDirectory(string path)
        {
            AudioDirectory = path;
        }

        public void SetImagesDirectory(string path)
        {
            ImagesDirectory = path;
        }

        public string GetAudioDirectory()
        {
            return AudioDirectory;
        }

        public string GetImagesDirectory()
        {
            return ImagesDirectory;
        }

        public void CreateVideo(string imagesDirectoryPath, string audioPath, string renderDirectory, EventHandler renderCompleted = null)
        {
            throw new NotImplementedException();
        }

        public void CreateAllVideos(string imagesDirectory, string audioDirectory, string renderDirectory)
        {
            try
            {
                var audios = Directory.GetFiles(audioDirectory);
                var imagePaths = Directory.GetFiles(imagesDirectory).ToList();

                foreach (var audioPath in audios)
                {
                    CreateVideoFromImages(imagePaths, audioPath, CreateRenderedVideoName(audioPath), renderDirectory);
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        public string CreateRenderedVideoName(string audioPath)
        {
            try
            {
                var parts = audioPath.Split('\\');
                var fileName = parts[parts.Length - 1];
                parts = fileName.Split('.');

                return parts[0];
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputFilename, string renderedDirectory, EventHandler renderCompleted = null)
        {
            try
            {
                var ehList = new List<EventHandler>();

                if (renderCompleted != null)
                    ehList.Add(renderCompleted);

                ehList.Add(RenderCompleted);

                _vps.CreateVideoFromImages(imagePaths, audioPath, Path.Combine(renderedDirectory, _vps.GetCorrectFilename(outputFilename)), VideoQuality.HD2_NTSC, ehList);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                throw;
            }
        }

        private void RenderCompleted(object sender, EventArgs e)
        {
            try
            {
                var t = "t";
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                throw;
            }
        }

        public void Configure(VideoManagmentSettings settings)
        {
            _vms = settings;
        }

    }
}
