using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Splicer.Renderer;

namespace MusicianHelper.Infrastructure.Models
{
    public class VideoRenderedEventArgs : EventArgs
    {
        public AudioUoW Audio { get; set; }
    }
}
