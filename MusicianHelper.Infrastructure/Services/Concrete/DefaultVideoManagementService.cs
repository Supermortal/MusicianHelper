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
        private readonly Dictionary<string, VideoRenderingSession> _sessions = new Dictionary<string, VideoRenderingSession>();
        private readonly IVideoNetworkService _vns;

        private string _renderDirectory = null;

        private string AudioDirectory { get; set; }
        private string ImagesDirectory { get; set; }

        public DefaultVideoManagementService(IVideoProcessingService vps, IVideoNetworkService vns)
        {
            _vps = vps;
            _vns = vns;
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

        public void CreateVideo(string imagesDirectoryPath, string audioPath, string renderDirectory, VideoRenderedEventHandler renderCompleted = null)
        {
            throw new NotImplementedException();
        }

        public void CreateAllVideos(List<string> imagePaths, List<string> audioPaths, string renderDirectory)
        {
            try
            {
                foreach (var audioPath in audioPaths)
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

        public List<string> GetImagePaths(string imageDirectory)
        {
            try
            {
                return Directory.GetFiles(imageDirectory).ToList();
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public void CreateAllVideos(List<AudioUoW> audios, string imageDirectory, string renderDirectory, AllVideosRenderedEventHandler allVideosRendered = null, VideoRenderedEventHandler renderCompleted = null, Action<string> feedbackMethod = null)
        {
            try
            {
                var imagePaths = Directory.GetFiles(imageDirectory).ToList();

                var sessionId = Guid.NewGuid().ToString();
                _sessions.Add(sessionId, new VideoRenderingSession() {Count = 0, AllVideosRendered = allVideosRendered});
                foreach (var audio in audios)
                {
                    audio.SessionId = sessionId;

                    var filename = MakeValidFileName(audio.Title);
                    var correctedFilename = _vps.GetCorrectFilename(filename);
                    audio.VideoPath = Path.Combine(renderDirectory, correctedFilename);

                    var list = new List<VideoRenderedEventHandler>();
                    if (renderCompleted != null)
                        list.Add(renderCompleted);
                    list.Add(RenderCompleted);

                    if (feedbackMethod != null)
                        feedbackMethod("Rendering started... " + audio.Title);

                    _sessions[sessionId].Count++;
                    _vps.CreateVideoFromImages(imagePaths, audio, audio.VideoPath, VideoQuality.FULLHD2_NTSC, list);
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        public void CreateVideoFromImages(List<string> imagePaths, string audioPath, string outputFilename, string renderedDirectory, VideoRenderedEventHandler renderCompleted = null)
        {
            try
            {
                var list = new List<VideoRenderedEventHandler>() { RenderCompleted };
                if (renderCompleted != null)
                    list.Add(renderCompleted);
       
                 _vps.CreateVideoFromImages(imagePaths, audioPath, Path.Combine(renderedDirectory, _vps.GetCorrectFilename(outputFilename)), VideoQuality.HD2_NTSC, list);            
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                throw;
            }
        }

        private void RenderCompleted(object sender, VideoRenderedEventArgs e)
        {
            try
            {
                var session = _sessions[e.Audio.SessionId];
                session.Count--;

                if (session.Count > 0) return;

                session.AllVideosRendered(this, EventArgs.Empty);
                //_sessions.Remove(e.Audio.SessionId);
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

        public string MakeValidFileName(string name)
        {
            var invalidChars = System.Text.RegularExpressions.Regex.Escape(new string(System.IO.Path.GetInvalidFileNameChars()));
            var invalidRegStr = string.Format(@"([{0}]*\.+$)|([{0}]+)", invalidChars);

            return System.Text.RegularExpressions.Regex.Replace(name, invalidRegStr, "_");
        }

        public List<string> GetAllVideoPaths(string renderedDirectory)
        {
            try
            {
                return Directory.GetFiles(renderedDirectory).ToList();
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public void DeleteAllRenderedVideos(string renderedDirectory)
        {
            try
            {
                var di = new DirectoryInfo(renderedDirectory);
                foreach (var file in di.GetFiles())
                {
                    file.Delete();
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        public void UploadVideo(AudioUoW audio, OauthTokenModel otm, VideoUploadedEventArgs videoUploaded = null)
        {
            try
            {
                _vns.UploadVideo(audio, otm, new List<VideoUploadedEventHandler>());
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        public void UploadAllVideos(List<AudioUoW> audios, OauthTokenModel otm, AllVideosUploadedEventHandler allVideosUploaded = null,
            VideoUploadedEventHandler videoUploaded = null, Action<string> feedbackMethod = null)
        {
            try
            {
                var sessionId = audios.First().SessionId;
                var session = _sessions[sessionId];
                session.AllVideosUploaded = allVideosUploaded;
                session.Count = 0;
                foreach (var audio in audios)
                {
                    var list = new List<VideoUploadedEventHandler>();
                    if (videoUploaded != null)
                        list.Add(videoUploaded);
                    list.Add(VideoUploaded);

                    if (feedbackMethod != null)
                        feedbackMethod("Upload started... " + audio.Title);

                    _sessions[sessionId].Count++;
                    _vns.UploadVideo(audio, otm, list);
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void VideoUploaded(object sender, VideoUploadedEventArgs e)
        {
            try
            {
                var session = _sessions[e.Audio.SessionId];
                session.Count--;

                if (session.Count > 0) return;

                session.AllVideosUploaded(this, EventArgs.Empty);
                //_sessions.Remove(e.Audio.SessionId);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

    }
}
