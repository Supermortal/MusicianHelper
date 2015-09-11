using System;
using System.Windows.Forms;

namespace MusicianHelper.WinForms
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
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
    }
}
