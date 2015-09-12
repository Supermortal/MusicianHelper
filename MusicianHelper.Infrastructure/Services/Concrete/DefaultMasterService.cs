using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using log4net;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.Infrastructure.Services.Concrete
{
    public class DefaultMasterService : IMasterService
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (DefaultMasterService));

        public List<AudioUoW> CreateAudioUnitsOfWork(string audioDirectory)
        {
            try
            {
                var files = Directory.GetFiles(audioDirectory);
                var list = files.Select(audioPath => new AudioUoW() {Path = audioPath, Title = GetAudioUnitOfWorkName(audioPath)}).ToList();
                return list;
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public string GetAudioUnitOfWorkName(string audioPath)
        {
            try
            {
                var parts = audioPath.Split('\\');
                var fileName = parts[parts.Length - 1];
                parts = fileName.Split('.');

                return parts[0];
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public string GenerateTagsString(List<string> tags)
        {
            try
            {
                var sb = new StringBuilder();
                foreach (var tag in tags)
                {
                    sb.Append(tag);
                    sb.Append(' ');
                }

                return sb.ToString().TrimEnd(' ');
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public List<string> GetTagsFromTagString(string tagsString)
        {
            try
            {
                var parts = tagsString.Split(',');
                return parts.Select(i => i.Trim()).ToList();
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

    }
}
