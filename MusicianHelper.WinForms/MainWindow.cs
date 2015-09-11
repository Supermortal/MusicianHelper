using System;
using System.Windows.Forms;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.WinForms
{
    public partial class MainWindow : Form
    {

        private readonly IVideoManagementService _vms;

        public MainWindow() : this(IoCHelper.Instance.GetService<IVideoManagementService>())
        {
            InitializeComponent();
        }

        public MainWindow(IVideoManagementService vms)
        {
            _vms = vms;
        }

        private void SetAudioDirectoryButton_Click(object sender, EventArgs e)
        {
            var result = FolderBrowserDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                AudioDirectory.Text = FolderBrowserDialog.SelectedPath;
            }
        }

        private void SetVideoDirectoryButton_Click(object sender, EventArgs e)
        {
            var result = FolderBrowserDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                VideoDirectory.Text = FolderBrowserDialog.SelectedPath;
            }
        }

        private void SetImagesDirectoryButton_Click(object sender, EventArgs e)
        {
            var result = FolderBrowserDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                ImagesDirectory.Text = FolderBrowserDialog.SelectedPath;
            }
        }

        private void RunButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(ImagesDirectory.Text) || string.IsNullOrEmpty(VideoDirectory.Text) ||
                string.IsNullOrEmpty(AudioDirectory.Text))
                return;

            _vms.CreateAllVideos(ImagesDirectory.Text, AudioDirectory.Text, VideoDirectory.Text);
        }

        private void YouTubeCredentialsButton_Click(object sender, EventArgs e)
        {
            var ytw = new YouTubeWindow();
            ytw.Show(this);
        }
    }
}
