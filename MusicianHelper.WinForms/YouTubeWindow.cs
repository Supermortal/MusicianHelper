using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MusicianHelper.WinForms
{
    public partial class YouTubeWindow : Form
    {

        private const string YOUTUBE_URL = "https://accounts.google.com/o/oauth2/auth?client_id=392497808537-qddvd51c0qt055749pggpjrp45qi4s0b.apps.googleusercontent.com&redirect_uri=urn:ietf:wg:oauth:2.0:oob&response_type=code&scope=https://www.googleapis.com/auth/youtube.upload";

        public YouTubeWindow()
        {            
            InitializeComponent();

            Uri uri;
            Uri.TryCreate(YOUTUBE_URL, UriKind.Absolute, out uri);
            WebBrowser.Url = uri;
        }

        private void WebBrowser_Navigated(object sender, WebBrowserNavigatedEventArgs e)
        {
            var t = "t";
        }
    }
}
