using System;
using System.Collections.Generic;
using System.Windows.Forms;
using log4net;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;

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

        public MainWindow() : this(
            IoCHelper.Instance.GetService<IVideoManagementService>(), 
            IoCHelper.Instance.GetService<IVideoNetworkService>(), 
            IoCHelper.Instance.GetService<IStorageService>(),
            IoCHelper.Instance.GetService<IMasterService>()
            )
        {
            InitializeComponent();
            SetFolderDefaults();
        }

        public MainWindow(IVideoManagementService vms, IVideoNetworkService vns, IStorageService ss, IMasterService ms)
        {
            _vms = vms;
            _vns = vns;
            _ss = ss;
            _ms = ms;
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

        private void StartProcessing()
        {
            try
            {
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

        private void RunButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(ImagesDirectory.Text) || string.IsNullOrEmpty(VideoDirectory.Text) ||
                string.IsNullOrEmpty(AudioDirectory.Text))
                return;

            StartProcessing();
        }

        private void YouTubeCredentialsButton_Click(object sender, EventArgs e)
        {
            var ytw = new YouTubeWindow();
            ytw.Show(this);
        }

        private void AudioEditWindow_Closed(object sender, EventArgs e)
        {
            try
            {
                var t = "t";
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }
        #endregion

    }
}
