namespace MusicianHelper.Infrastructure.Models
{
    public class APIKeysModel
    {
        public string YouTubeClientId { get; set; }
        public string YouTubeClientSecret { get; set; }
        public string SoundCloudClientId { get; set; }
        public string SoundCloudClientSecret { get; set; }

        public void UpdateStorageModel(StorageModel sm)
        {
            sm.YouTubeClientId = YouTubeClientId;
            sm.YouTubeClientSecret = YouTubeClientSecret;
            sm.SoundCloudClientId = SoundCloudClientId;
            sm.SoundCloudClientSecret = SoundCloudClientSecret;
        }
    }
}
