using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MusicianHelper.Infrastructure.Models
{
    public class AudioUoW
    {
        public string Title { get; set; }
        public string Description { get; set; }
        public List<string> Tags { get; set; }
        public string Path { get; set; }

        public AudioUoW()
        {
            Tags = new List<string>();
        }

    }
}
