using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.Infrastructure.Models
{
    public class AudioUploadSession
    {
        public int Count { get; set; }
        public AllAudiosUploadedEventHandler AllAudiosUploaded { get; set; }
    }
}
