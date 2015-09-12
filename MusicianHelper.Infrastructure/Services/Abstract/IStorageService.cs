using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IStorageService
    {
        void Save(StorageModel obj);
        StorageModel Load();
        string GetBaseDirectory();
    }
}
