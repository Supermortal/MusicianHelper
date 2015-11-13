using System;
using System.Windows.Forms;
using log4net;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.WinForms
{
    public partial class FacebookWindow : Form
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (FacebookWindow));

        public event CredentialsSet CredentialsSet;

        private readonly ISocialMediaService _sms;

        private bool _credentialsSet;

        public FacebookWindow() : this(IoCHelper.Instance.GetService<ISocialMediaService>())
        {
        }

        public FacebookWindow(ISocialMediaService sms)
        {
            InitializeComponent();
            _sms = sms;
            SetWebBrowserUrl();
        }

        private void SetWebBrowserUrl()
        {
            WebBrowser.Url = _sms.CreateRequestUri();
        }

        private void WebBrowser_Navigated(object sender, WebBrowserNavigatedEventArgs e)
        {
            if (_credentialsSet)
                return;

            if (!WebBrowser.Url.AbsoluteUri.Contains("code")) return;

            var authToken = _sms.ExtractAuthToken(WebBrowser.Url.AbsoluteUri);
            var oauthResponse = _sms.GetRequestTokens(authToken);
            _sms.SaveOauthResponse(oauthResponse);
            //Close();
            Hide();

            _credentialsSet = true;

            if (CredentialsSet != null)
                CredentialsSet(this, EventArgs.Empty);
        }

    }
}
