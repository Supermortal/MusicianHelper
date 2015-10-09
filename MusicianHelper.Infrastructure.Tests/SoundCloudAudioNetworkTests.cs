using MusicianHelper.Infrastructure.Services.Concrete;
using MusicianHelper.Infrastructure.Tests.Concrete;
using Xunit;

namespace MusicianHelper.Infrastructure.Tests
{
    public class SoundCloudAudioNetworkTests
    {
        [Fact]
        public void soundcloud_authentication()
        {
            var ss = new JsonNetStorageService();
            var aks = new DefaultAPIKeyService(ss);

            var testSs = new TestStorageService();
            
            var service = new SoundCloudAudioNetworkService(testSs, aks);

            service.Authenticate();
        }
    }
}
