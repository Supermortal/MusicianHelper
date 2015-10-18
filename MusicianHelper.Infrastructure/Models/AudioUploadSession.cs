using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.Infrastructure.Models
{
    public class AudioUploadSession
    {
        public int Count { get; set; }
        public AllAudiosUploadedEventHandler AllAudiosUploaded { get; set; }
    }
}
