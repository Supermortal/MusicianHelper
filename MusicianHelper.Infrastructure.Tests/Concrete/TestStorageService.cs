using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.Infrastructure.Tests.Concrete
{
    public class TestStorageService : IStorageService
    {

        private static StorageModel _sm = new StorageModel();

        public void Save(StorageModel obj)
        {
            _sm = obj;
        }

        public StorageModel Load()
        {
            return _sm;
        }

        public string GetBaseDirectory()
        {
            return string.Empty;
        }
    }
}
