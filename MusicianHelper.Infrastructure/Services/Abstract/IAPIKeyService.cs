using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IAPIKeyService
    {
        APIKeysModel GetAPIKeys();
        void BootstrapAPIKeys(APIKeysModel akm);
    }
}
