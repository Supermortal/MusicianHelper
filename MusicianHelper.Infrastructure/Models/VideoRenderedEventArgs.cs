using System;

namespace MusicianHelper.Infrastructure.Models
{
    public class VideoRenderedEventArgs : EventArgs
    {
        public AudioUoW Audio { get; set; }
    }
}
