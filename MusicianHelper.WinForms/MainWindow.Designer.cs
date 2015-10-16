namespace MusicianHelper.WinForms
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.FolderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.SetAudioDirectoryButton = new System.Windows.Forms.Button();
            this.AudioDirectory = new System.Windows.Forms.TextBox();
            this.VideoDirectory = new System.Windows.Forms.TextBox();
            this.SetVideoDirectoryButton = new System.Windows.Forms.Button();
            this.ImagesDirectory = new System.Windows.Forms.TextBox();
            this.SetImagesDirectoryButton = new System.Windows.Forms.Button();
            this.ConfigureAudioButton = new System.Windows.Forms.Button();
            this.YouTubeCredentialsButton = new System.Windows.Forms.Button();
            this.ConsoleTextBox = new System.Windows.Forms.TextBox();
            this.RenderVideosButton = new System.Windows.Forms.Button();
            this.UploadVideosButton = new System.Windows.Forms.Button();
            this.SoundCloudCredentialsButton = new System.Windows.Forms.Button();
            this.UploadAudiosButton = new System.Windows.Forms.Button();
            this.FacebookCredentialsButton = new System.Windows.Forms.Button();
            this.PostSocialMediaButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // SetAudioDirectoryButton
            // 
            this.SetAudioDirectoryButton.Location = new System.Drawing.Point(447, 99);
            this.SetAudioDirectoryButton.Name = "SetAudioDirectoryButton";
            this.SetAudioDirectoryButton.Size = new System.Drawing.Size(147, 23);
            this.SetAudioDirectoryButton.TabIndex = 0;
            this.SetAudioDirectoryButton.Text = "Set Audio Directory";
            this.SetAudioDirectoryButton.UseVisualStyleBackColor = true;
            this.SetAudioDirectoryButton.Click += new System.EventHandler(this.SetAudioDirectoryButton_Click);
            // 
            // AudioDirectory
            // 
            this.AudioDirectory.Enabled = false;
            this.AudioDirectory.Location = new System.Drawing.Point(94, 50);
            this.AudioDirectory.Name = "AudioDirectory";
            this.AudioDirectory.Size = new System.Drawing.Size(869, 20);
            this.AudioDirectory.TabIndex = 1;
            // 
            // VideoDirectory
            // 
            this.VideoDirectory.Enabled = false;
            this.VideoDirectory.Location = new System.Drawing.Point(94, 163);
            this.VideoDirectory.Name = "VideoDirectory";
            this.VideoDirectory.Size = new System.Drawing.Size(869, 20);
            this.VideoDirectory.TabIndex = 2;
            // 
            // SetVideoDirectoryButton
            // 
            this.SetVideoDirectoryButton.Location = new System.Drawing.Point(447, 207);
            this.SetVideoDirectoryButton.Name = "SetVideoDirectoryButton";
            this.SetVideoDirectoryButton.Size = new System.Drawing.Size(147, 23);
            this.SetVideoDirectoryButton.TabIndex = 3;
            this.SetVideoDirectoryButton.Text = "Set Video Directory";
            this.SetVideoDirectoryButton.UseVisualStyleBackColor = true;
            this.SetVideoDirectoryButton.Click += new System.EventHandler(this.SetVideoDirectoryButton_Click);
            // 
            // ImagesDirectory
            // 
            this.ImagesDirectory.Enabled = false;
            this.ImagesDirectory.Location = new System.Drawing.Point(94, 275);
            this.ImagesDirectory.Name = "ImagesDirectory";
            this.ImagesDirectory.Size = new System.Drawing.Size(869, 20);
            this.ImagesDirectory.TabIndex = 4;
            // 
            // SetImagesDirectoryButton
            // 
            this.SetImagesDirectoryButton.Location = new System.Drawing.Point(447, 321);
            this.SetImagesDirectoryButton.Name = "SetImagesDirectoryButton";
            this.SetImagesDirectoryButton.Size = new System.Drawing.Size(147, 23);
            this.SetImagesDirectoryButton.TabIndex = 5;
            this.SetImagesDirectoryButton.Text = "Set Images Directory";
            this.SetImagesDirectoryButton.UseVisualStyleBackColor = true;
            this.SetImagesDirectoryButton.Click += new System.EventHandler(this.SetImagesDirectoryButton_Click);
            // 
            // ConfigureAudioButton
            // 
            this.ConfigureAudioButton.Enabled = false;
            this.ConfigureAudioButton.Location = new System.Drawing.Point(263, 671);
            this.ConfigureAudioButton.Name = "ConfigureAudioButton";
            this.ConfigureAudioButton.Size = new System.Drawing.Size(147, 23);
            this.ConfigureAudioButton.TabIndex = 6;
            this.ConfigureAudioButton.Text = "Configure Audio";
            this.ConfigureAudioButton.UseVisualStyleBackColor = true;
            this.ConfigureAudioButton.Click += new System.EventHandler(this.ConfigureAudioButton_Click);
            // 
            // YouTubeCredentialsButton
            // 
            this.YouTubeCredentialsButton.Enabled = false;
            this.YouTubeCredentialsButton.Location = new System.Drawing.Point(55, 671);
            this.YouTubeCredentialsButton.Name = "YouTubeCredentialsButton";
            this.YouTubeCredentialsButton.Size = new System.Drawing.Size(163, 23);
            this.YouTubeCredentialsButton.TabIndex = 7;
            this.YouTubeCredentialsButton.Text = "Set YouTube Credentials";
            this.YouTubeCredentialsButton.UseVisualStyleBackColor = true;
            this.YouTubeCredentialsButton.Click += new System.EventHandler(this.YouTubeCredentialsButton_Click);
            // 
            // ConsoleTextBox
            // 
            this.ConsoleTextBox.Enabled = false;
            this.ConsoleTextBox.Location = new System.Drawing.Point(94, 384);
            this.ConsoleTextBox.Multiline = true;
            this.ConsoleTextBox.Name = "ConsoleTextBox";
            this.ConsoleTextBox.Size = new System.Drawing.Size(869, 250);
            this.ConsoleTextBox.TabIndex = 8;
            // 
            // RenderVideosButton
            // 
            this.RenderVideosButton.Enabled = false;
            this.RenderVideosButton.Location = new System.Drawing.Point(447, 671);
            this.RenderVideosButton.Name = "RenderVideosButton";
            this.RenderVideosButton.Size = new System.Drawing.Size(147, 23);
            this.RenderVideosButton.TabIndex = 9;
            this.RenderVideosButton.Text = "Render Videos";
            this.RenderVideosButton.UseVisualStyleBackColor = true;
            this.RenderVideosButton.Click += new System.EventHandler(this.RenderVideosButton_Click);
            // 
            // UploadVideosButton
            // 
            this.UploadVideosButton.Enabled = false;
            this.UploadVideosButton.Location = new System.Drawing.Point(645, 671);
            this.UploadVideosButton.Name = "UploadVideosButton";
            this.UploadVideosButton.Size = new System.Drawing.Size(147, 23);
            this.UploadVideosButton.TabIndex = 10;
            this.UploadVideosButton.Text = "Upload Videos";
            this.UploadVideosButton.UseVisualStyleBackColor = true;
            this.UploadVideosButton.Click += new System.EventHandler(this.UploadVideosButton_Click);
            // 
            // SoundCloudCredentialsButton
            // 
            this.SoundCloudCredentialsButton.Enabled = false;
            this.SoundCloudCredentialsButton.Location = new System.Drawing.Point(55, 709);
            this.SoundCloudCredentialsButton.Name = "SoundCloudCredentialsButton";
            this.SoundCloudCredentialsButton.Size = new System.Drawing.Size(163, 23);
            this.SoundCloudCredentialsButton.TabIndex = 11;
            this.SoundCloudCredentialsButton.Text = "Set SoundCloud Credentials";
            this.SoundCloudCredentialsButton.UseVisualStyleBackColor = true;
            this.SoundCloudCredentialsButton.Click += new System.EventHandler(this.SoundCloudCredentials_Click);
            // 
            // UploadAudiosButton
            // 
            this.UploadAudiosButton.Enabled = false;
            this.UploadAudiosButton.Location = new System.Drawing.Point(645, 709);
            this.UploadAudiosButton.Name = "UploadAudiosButton";
            this.UploadAudiosButton.Size = new System.Drawing.Size(147, 23);
            this.UploadAudiosButton.TabIndex = 12;
            this.UploadAudiosButton.Text = "Upload Audios";
            this.UploadAudiosButton.UseVisualStyleBackColor = true;
            this.UploadAudiosButton.Click += new System.EventHandler(this.UploadAudiosButton_Click);
            // 
            // FacebookCredentialsButton
            // 
            this.FacebookCredentialsButton.Enabled = false;
            this.FacebookCredentialsButton.Location = new System.Drawing.Point(55, 749);
            this.FacebookCredentialsButton.Name = "FacebookCredentialsButton";
            this.FacebookCredentialsButton.Size = new System.Drawing.Size(163, 23);
            this.FacebookCredentialsButton.TabIndex = 13;
            this.FacebookCredentialsButton.Text = "Set Facebook Credentials";
            this.FacebookCredentialsButton.UseVisualStyleBackColor = true;
            this.FacebookCredentialsButton.Click += new System.EventHandler(this.FacebookCredentialsButton_Click);
            // 
            // PostSocialMediaButton
            // 
            this.PostSocialMediaButton.Enabled = false;
            this.PostSocialMediaButton.Location = new System.Drawing.Point(645, 749);
            this.PostSocialMediaButton.Name = "PostSocialMediaButton";
            this.PostSocialMediaButton.Size = new System.Drawing.Size(147, 23);
            this.PostSocialMediaButton.TabIndex = 14;
            this.PostSocialMediaButton.Text = "Post Social Media";
            this.PostSocialMediaButton.UseVisualStyleBackColor = true;
            this.PostSocialMediaButton.Click += new System.EventHandler(this.PostSocialMediaButton_Click);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1042, 784);
            this.Controls.Add(this.PostSocialMediaButton);
            this.Controls.Add(this.FacebookCredentialsButton);
            this.Controls.Add(this.UploadAudiosButton);
            this.Controls.Add(this.SoundCloudCredentialsButton);
            this.Controls.Add(this.UploadVideosButton);
            this.Controls.Add(this.RenderVideosButton);
            this.Controls.Add(this.ConsoleTextBox);
            this.Controls.Add(this.YouTubeCredentialsButton);
            this.Controls.Add(this.ConfigureAudioButton);
            this.Controls.Add(this.SetImagesDirectoryButton);
            this.Controls.Add(this.ImagesDirectory);
            this.Controls.Add(this.SetVideoDirectoryButton);
            this.Controls.Add(this.VideoDirectory);
            this.Controls.Add(this.AudioDirectory);
            this.Controls.Add(this.SetAudioDirectoryButton);
            this.Name = "MainWindow";
            this.Text = "Musician Helper";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.FolderBrowserDialog FolderBrowserDialog;
        private System.Windows.Forms.Button SetAudioDirectoryButton;
        private System.Windows.Forms.TextBox AudioDirectory;
        private System.Windows.Forms.TextBox VideoDirectory;
        private System.Windows.Forms.Button SetVideoDirectoryButton;
        private System.Windows.Forms.TextBox ImagesDirectory;
        private System.Windows.Forms.Button SetImagesDirectoryButton;
        private System.Windows.Forms.Button ConfigureAudioButton;
        private System.Windows.Forms.Button YouTubeCredentialsButton;
        private System.Windows.Forms.TextBox ConsoleTextBox;
        private System.Windows.Forms.Button RenderVideosButton;
        private System.Windows.Forms.Button UploadVideosButton;
        private System.Windows.Forms.Button SoundCloudCredentialsButton;
        private System.Windows.Forms.Button UploadAudiosButton;
        private System.Windows.Forms.Button FacebookCredentialsButton;
        private System.Windows.Forms.Button PostSocialMediaButton;
    }
}

