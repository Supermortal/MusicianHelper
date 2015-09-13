using System.Collections.Generic;

namespace MusicianHelper.Infrastructure.Models
{
    public class AudioUoW
    {
        public string Title { get; set; }
        public string Description { get; set; }
        public List<string> Tags { get; set; }
        public string AudioPath { get; set; }
        public string VideoPath { get; set; }
        public string SessionId { get; set; }

        public AudioUoW()
        {
            Tags = new List<string>();
        }

    }
}
