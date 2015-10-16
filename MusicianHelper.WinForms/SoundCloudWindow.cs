using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using log4net;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.WinForms
{
    public partial class SoundCloudWindow : Form
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (SoundCloudWindow));

        private readonly IAudioNetworkService _ans;

        public SoundCloudWindow() : this(IoCHelper.Instance.GetService<IAudioNetworkService>())
        {
            
        }

        public SoundCloudWindow(IAudioNetworkService ans)
        {
            _ans = ans;
            InitializeComponent();
            SetWebBrowserUrl();
        }

        private void SetWebBrowserUrl()
        {
            WebBrowser.Url = _ans.CreateRequestUri();
        }

        private void WebBrowser_Navigated(object sender, WebBrowserNavigatedEventArgs e)
        {
            if (WebBrowser.DocumentText == "<HTML></HTML>\0" || WebBrowser.Url.AbsoluteUri.Contains("popup"))
            {
                WebBrowser.Url = _ans.CreateRequestUri();
                return;
            }

            if (!WebBrowser.Url.AbsoluteUri.Contains(_ans.GetRedirectUrl())) return;

            var authToken = _ans.ExtractAuthToken(WebBrowser.Url.AbsoluteUri);
            var oauthResponse = _ans.GetRequestTokens(authToken);
            _ans.SaveOauthResponse(oauthResponse);
            Close();
        }
    }
}
