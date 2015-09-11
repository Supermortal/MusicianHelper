using System.Windows.Forms;
using log4net;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.WinForms
{
    public partial class YouTubeWindow : Form
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (YouTubeWindow));

        private readonly IVideoNetworkService _vns;

        public YouTubeWindow() : this(IoCHelper.Instance.GetService<IVideoNetworkService>())
        {            
            InitializeComponent();
            SetWebBrowserUrl();
        }

        public YouTubeWindow(IVideoNetworkService vns)
        {
            _vns = vns;           
        }

        private void SetWebBrowserUrl()
        {
            WebBrowser.Url = _vns.CreateRequestUri();
        }

        private void WebBrowser_Navigated(object sender, WebBrowserNavigatedEventArgs e)
        {
            if (!WebBrowser.DocumentTitle.Contains("Success code")) return;

            var authToken = _vns.ExtractAuthToken(WebBrowser.DocumentTitle);
            var oauthResponse = _vns.GetRequestTokens(authToken);
            _vns.SaveOauthResponse(oauthResponse);
        }

    }
}
