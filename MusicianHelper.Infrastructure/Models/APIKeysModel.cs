namespace MusicianHelper.Infrastructure.Models
{
    public class APIKeysModel
    {
        public string YouTubeClientId { get; set; }
        public string YouTubeClientSecret { get; set; }

        public void UpdateStorageModel(StorageModel sm)
        {
            sm.YouTubeClientId = YouTubeClientId;
            sm.YouTubeClientSecret = YouTubeClientSecret;
        }
    }
}
