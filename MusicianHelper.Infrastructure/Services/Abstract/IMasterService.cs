using System.Collections.Generic;
using MusicianHelper.Infrastructure.Models;

namespace MusicianHelper.Infrastructure.Services.Abstract
{
    public interface IMasterService
    {
        List<AudioUoW> CreateAudioUnitsOfWork(string audioDirectory);
        string GetAudioUnitOfWorkName(string audioPath);
        string GenerateTagsString(List<string> tags, char delimiter = ',');
        List<string> GetTagsFromTagString(string tagsString);
    }
}
