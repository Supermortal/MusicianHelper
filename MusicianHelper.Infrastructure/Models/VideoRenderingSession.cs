using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.Infrastructure.Models
{
    public class VideoRenderingSession
    {
        public int Count { get; set; }
        public AllVideosRenderedEventHandler AllVideosRendered { get; set; }
        public AllVideosUploadedEventHandler AllVideosUploaded { get; set; }
    }
}
