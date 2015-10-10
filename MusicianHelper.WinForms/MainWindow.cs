using System;
using System.Collections.Generic;
using System.Windows.Forms;
using log4net;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;
using MusicianHelper.WinForms.Properties;

namespace MusicianHelper.WinForms
{
    public partial class MainWindow : Form
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (MainWindow));

        private readonly IVideoManagementService _vms;
        private readonly IVideoNetworkService _vns;
        private readonly IStorageService _ss;
        private readonly IMasterService _ms;
        private readonly IAudioNetworkService _ans;
        private readonly IAudioManagementService _ams;

        private List<AudioUoW> _audios;
        private bool _videoCredentialsSet = false;
        private bool _audioMetadataSet = false;
        private bool _videosRendered = false;
        private bool _videosUploaded = false;
        private bool _audioCredentialsSet = false;
        private bool _audiosUploaded = false;

        public MainWindow() : this(
            IoCHelper.Instance.GetService<IVideoManagementService>(), 
            IoCHelper.Instance.GetService<IVideoNetworkService>(), 
            IoCHelper.Instance.GetService<IStorageService>(),
            IoCHelper.Instance.GetService<IMasterService>(),
            IoCHelper.Instance.GetService<IAudioNetworkService>(),
            IoCHelper.Instance.GetService<IAudioManagementService>()
            )
        {
            
        }

        public MainWindow(IVideoManagementService vms, IVideoNetworkService vns, IStorageService ss, IMasterService ms, IAudioNetworkService ans, IAudioManagementService ams)
        {
            _vms = vms;
            _vns = vns;
            _ss = ss;
            _ms = ms;
            _ans = ans;
            _ams = ams;

            InitializeComponent();
            SetFolderDefaults();
            CheckCredentials();
        }

        private void SetFolderDefaults()
        {
            try
            {
                var sm = _ss.Load();

                AudioDirectory.Text = sm.LastAudioDirectory;
                VideoDirectory.Text = sm.LastVideoDirectory;
                ImagesDirectory.Text = sm.LastImagesDirectory;
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void StartAudioMetadataProcessing()
        {
            try
            {
                AppendToLog("Starting audio configuration...");

                if (_audios == null)
                    _audios = _ms.CreateAudioUnitsOfWork(AudioDirectory.Text);
                
                var aew = new AudioEditWindow(_audios);
                aew.Closed += AudioEditWindow_Closed;
                aew.Show(this);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void StartVideoRendering(List<AudioUoW> audios)
        {
            try
            {
                AppendToLog("Cleaning rendering directory...");
                _vms.DeleteAllRenderedVideos(VideoDirectory.Text);
                AppendToLog("Beginning video rendering (this will probably take a while)...");             
                _vms.CreateAllVideos(audios, ImagesDirectory.Text, VideoDirectory.Text, AllVideosRendered, VideoRendered, AppendToLog);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void StartVideoUpload()
        {
            try
            {
                var otm = _ss.Load().ToYouTubeOauthTokenModel();
                AppendToLog("Beginning video uploads (this will probably take a while)...");  
                _vms.UploadAllVideos(_audios, otm, AllVideosUploaded, VideoUploaded, AppendToLog);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void StartAudioUpload()
        {
            try
            {
                var otm = _ss.Load().ToSoundCloudOauthTokenModel();
                AppendToLog("Beginning audio uploads (this will probably take a while)...");  
                _ams.UploadAllAudios(_audios, otm, AllAudiosUploaded, AudioUploaded, AppendToLog);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void CheckCredentials()
        {
            try
            {

                if (_vns.HasCredentials() == true)
                {
                    YouTubeCredentialsButton.Text = Resources.MainWindow_CheckCredentials_Reset_YouTube_Credentials;
                    _videoCredentialsSet = true;
                    ConfigureAudioButton.Enabled = true;
                    AppendToLog("YouTube credentials already set!");

                    var sm = _ss.Load();
                    var ytm = sm.ToYouTubeOauthTokenModel();
                    if (ytm.AccessTokenExpired == true)
                    {
                        AppendToLog("Starting YouTube credentials refresh...");
                        ytm = _vns.RefreshRequestTokens(ytm);
                        ytm.UpdateYouTubeStorage(sm);
                        _ss.Save(sm);
                        AppendToLog("YouTube credentials refresh complete!");
                    }
                }

                if (_ans.HasCredentials() == true)
                {
                    SoundCloudCredentialsButton.Text = Resources.MainWindow_CheckCredentials_Reset_SoundCloud_Credentials;
                    _audioCredentialsSet = true;
                    ConfigureAudioButton.Enabled = true;
                    AppendToLog("SoundCloud credentials already set!");
                }

            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void AppendToLog(string text)
        {
            try
            {
                var textToAppend = DateTime.Now + " | " + text + "\n";
                try
                { 
                    ConsoleTextBox.AppendText(textToAppend);
                }
                catch (InvalidOperationException ex)
                {
                    ConsoleTextBox.Invoke(new Action(() => ConsoleTextBox.AppendText(textToAppend)));
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                throw;
            }
        }

        #region Events
        private void SetAudioDirectoryButton_Click(object sender, EventArgs e)
        {
            var result = FolderBrowserDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                AudioDirectory.Text = FolderBrowserDialog.SelectedPath;
                var sm = _ss.Load();
                sm.LastAudioDirectory = FolderBrowserDialog.SelectedPath;
                _ss.Save(sm);
            }
        }

        private void SetVideoDirectoryButton_Click(object sender, EventArgs e)
        {
            var result = FolderBrowserDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                VideoDirectory.Text = FolderBrowserDialog.SelectedPath;
                var sm = _ss.Load();
                sm.LastVideoDirectory = FolderBrowserDialog.SelectedPath;
                _ss.Save(sm);
            }
        }

        private void SetImagesDirectoryButton_Click(object sender, EventArgs e)
        {
            var result = FolderBrowserDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                ImagesDirectory.Text = FolderBrowserDialog.SelectedPath;
                var sm = _ss.Load();
                sm.LastImagesDirectory = FolderBrowserDialog.SelectedPath;
                _ss.Save(sm);
            }
        }

        private void YouTubeCredentialsButton_Click(object sender, EventArgs e)
        {
            AppendToLog("Starting YouTube credentials set...");
            var ytw = new YouTubeWindow();
            ytw.Closed += YouTubeWindow_Closed;
            ytw.Show(this);
        }

        private void AudioEditWindow_Closed(object sender, EventArgs e)
        {
            try
            {
                AppendToLog("Audio configuration complete!");
                _audioMetadataSet = true;

                if (_videoCredentialsSet)
                    RenderVideosButton.Enabled = true;

                if (_audioCredentialsSet)
                    UploadAudiosButton.Enabled = true;
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void YouTubeWindow_Closed(object sender, EventArgs e)
        {
            try
            {
                AppendToLog("YouTube credentials set!");
                _videoCredentialsSet = true;
                ConfigureAudioButton.Enabled = true;

                if (_audioMetadataSet)
                    RenderVideosButton.Enabled = true;
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void SoundCloudWindow_Closed(object sender, EventArgs e)
        {
            try
            {
                AppendToLog("SoundCloud credentials set!");
                _audioCredentialsSet = true;
                ConfigureAudioButton.Enabled = true;

                if (_audioMetadataSet)
                    UploadAudiosButton.Enabled = true;
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void ConfigureAudioButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(AudioDirectory.Text) || string.IsNullOrEmpty(VideoDirectory.Text) ||
                string.IsNullOrEmpty(VideoDirectory.Text))
            {
                AppendToLog("Audio directory, images directory, and video directory must be set to configure audio!");
                return;
            }

            StartAudioMetadataProcessing();
        }

        private void RenderVideosButton_Click(object sender, EventArgs e)
        {
            try
            {
                RenderVideosButton.Enabled = false;
                ConfigureAudioButton.Enabled = false;
                StartVideoRendering(_audios);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void VideoRendered(object sender, VideoRenderedEventArgs e)
        {
            try
            {
                AppendToLog("Rendering completed! " + e.Audio.Title);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void AllVideosRendered(object sender, EventArgs e)
        {
            try
            {
                _videosRendered = true;
                UploadVideosButton.Invoke(new Action(() => UploadVideosButton.Enabled = true));
                AppendToLog("Video rendering complete!");
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void UploadVideosButton_Click(object sender, EventArgs e)
        {
            try
            {
                UploadVideosButton.Enabled = false;
                StartVideoUpload();
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
                AppendToLog("Upload complete! " + e.Audio.Title);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void AllVideosUploaded(object sender, EventArgs e)
        {
            try
            {
                _videosUploaded = true;
                AppendToLog("Video uploads complete!");
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void AudioUploaded(object sender, AudioUploadedEventArgs e)
        {
            try
            {
                AppendToLog("Upload complete! " + e.Audio.Title);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void AllAudiosUploaded(object sender, EventArgs e)
        {
            try
            {
                _audiosUploaded = true;
                AppendToLog("Audio uploads complete!");
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void SoundCloudCredentials_Click(object sender, EventArgs e)
        {
            try
            {
                AppendToLog("Starting SoundCloud credentials set...");
                var scw = new SoundCloudWindow();
                scw.Closed += SoundCloudWindow_Closed;
                scw.Show(this);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void UploadAudiosButton_Click(object sender, EventArgs e)
        {
            UploadAudiosButton.Enabled = false;
            StartAudioUpload();
        }
        #endregion

    }
}
