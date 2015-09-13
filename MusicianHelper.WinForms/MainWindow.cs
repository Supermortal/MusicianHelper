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

        private List<AudioUoW> _audios;
        private bool _videoCredentialsSet = false;
        private bool _audioMetadataSet = false;

        public MainWindow() : this(
            IoCHelper.Instance.GetService<IVideoManagementService>(), 
            IoCHelper.Instance.GetService<IVideoNetworkService>(), 
            IoCHelper.Instance.GetService<IStorageService>(),
            IoCHelper.Instance.GetService<IMasterService>()
            )
        {
            
        }

        public MainWindow(IVideoManagementService vms, IVideoNetworkService vns, IStorageService ss, IMasterService ms)
        {
            _vms = vms;
            _vns = vns;
            _ss = ss;
            _ms = ms;

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
                AppendToLog("Beginning video rendering (this will probably take a while)...");
                _vms.CreateAllVideos(audios, ImagesDirectory.Text, VideoDirectory.Text, AllVideosRendered, VideoRendered, AppendToLog);
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
                        ytm.UpdateStorageModel(sm);
                        _ss.Save(sm);
                        AppendToLog("YouTube credentials refresh complete!");
                    }
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
                RenderVideosButton.Enabled = true;
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
                return;
            }

            StartAudioMetadataProcessing();
        }

        private void RenderVideosButton_Click(object sender, EventArgs e)
        {
            try
            {
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
                AppendToLog("Video rendering complete!");
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }
        #endregion

    }
}
