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
            this.RunButton = new System.Windows.Forms.Button();
            this.YouTubeCredentialsButton = new System.Windows.Forms.Button();
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
            // RunButton
            // 
            this.RunButton.Location = new System.Drawing.Point(447, 671);
            this.RunButton.Name = "RunButton";
            this.RunButton.Size = new System.Drawing.Size(147, 23);
            this.RunButton.TabIndex = 6;
            this.RunButton.Text = "Run";
            this.RunButton.UseVisualStyleBackColor = true;
            this.RunButton.Click += new System.EventHandler(this.RunButton_Click);
            // 
            // YouTubeCredentialsButton
            // 
            this.YouTubeCredentialsButton.Location = new System.Drawing.Point(447, 608);
            this.YouTubeCredentialsButton.Name = "YouTubeCredentialsButton";
            this.YouTubeCredentialsButton.Size = new System.Drawing.Size(147, 23);
            this.YouTubeCredentialsButton.TabIndex = 7;
            this.YouTubeCredentialsButton.Text = "Set YouTube Credentials";
            this.YouTubeCredentialsButton.UseVisualStyleBackColor = true;
            this.YouTubeCredentialsButton.Click += new System.EventHandler(this.YouTubeCredentialsButton_Click);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1042, 784);
            this.Controls.Add(this.YouTubeCredentialsButton);
            this.Controls.Add(this.RunButton);
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
        private System.Windows.Forms.Button RunButton;
        private System.Windows.Forms.Button YouTubeCredentialsButton;
    }
}

