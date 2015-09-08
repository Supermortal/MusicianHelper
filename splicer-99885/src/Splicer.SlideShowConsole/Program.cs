// Copyright 2006-2008 Splicer Project - http://www.codeplex.com/splicer/
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

using System;
using System.Diagnostics;
using System.IO;
using CommandLine;
using Splicer.Renderer;
using Splicer.Timeline;
using Splicer.WindowsMedia;

namespace Splicer.SlideShowConsole
{
  internal class Program
  {
    private static void Main(string[] args)
    {
      Console.WriteLine("Splicer.SlideShowConsole version: {0}", typeof (Program).Assembly.GetName().Version);

      try
      {
        var options = new ApplicationOptions();

        if (Parser.ParseArgumentsWithUsage(args, options) == false)
          Environment.Exit(2);

        ValidateApplicationOptions(options);
        RenderVideo(options);
      }
      catch (Exception ex)
      {
        Console.Error.WriteLine("Exception occured while generating video, trace follows");
        Console.Error.WriteLine(ex.ToString());

        Environment.Exit(3);
      }
    }

    private static void RenderVideo(ApplicationOptions options)
    {
      using (ITimeline timeline = new DefaultTimeline(options.FPS))
      {
        PopulateTimeline(options, timeline);

        string timelineOutputFile = (options.EncodeTo3gpWithFfmpeg) ? GenerateIntermediateFileName(options) : options.OutputVideo;
          
        var renderer = (options.Renderer == OutputRenderer.WMV)
                                      ? (AbstractRenderer) new WindowsMediaRenderer(timeline, timelineOutputFile, WindowsMediaProfiles.HighQualityVideo)
                                      : new AviFileRenderer(timeline, timelineOutputFile);

        Stopwatch watch = Stopwatch.StartNew();

        RenderVideoUsingSplicer(renderer);

        EncodeTo3gpIfRequired(options, timelineOutputFile);

        Console.WriteLine("Render completed in {0}ms", watch.ElapsedMilliseconds);
      }
    }

    private static void PopulateTimeline(ApplicationOptions options, ITimeline timeline)
    {
      IGroup group = timeline.AddVideoGroup(32, options.Width, options.Height);

      ITrack audioTrack = timeline.AddAudioGroup().AddTrack();
      IClip audioClip = audioTrack.AddAudio(options.SoundtrackFile);

      ITrack videoTrack = group.AddTrack();

      double step = audioClip.Duration/options.SourceImage.Length;

      for (int i = 0; i < options.SourceImage.Length; i++)
      {
        videoTrack.AddImage(options.SourceImage[i], 0, step);
      }
    }

    private static void RenderVideoUsingSplicer(IRenderer renderer)
    {
      try
      {
        Console.WriteLine("Beginning render");
        renderer.Render();          
      }
      finally
      {
        var disposable = renderer as IDisposable;
        if (disposable != null) disposable.Dispose();
      }
    }

    private static void EncodeTo3gpIfRequired(ApplicationOptions options, string timelineOutputFile)
    {
      if (options.EncodeTo3gpWithFfmpeg)
      {
        if (!File.Exists(options.FfmpegPath))
        {
          throw new Exception("Can not find ffmpeg, please supply full path to executable if not in current directory");
        }

        string arguments = string.Format("-i {0} -s qcif -vcodec h263 -acodec aac -ac 1 -ar 8000 -r {1} -ab 32 -y {2}", timelineOutputFile, options.FPS,
                                         options.OutputVideo);

        Console.WriteLine("Converting file: {0} to 3gp file: {1} using FFMPEG", timelineOutputFile, options.OutputVideo);

        var info = new ProcessStartInfo(options.FfmpegPath, arguments)
                     {
                       UseShellExecute = false,
                       CreateNoWindow = true,
                       RedirectStandardOutput = true,
                       RedirectStandardError = true,
                       WindowStyle = ProcessWindowStyle.Hidden
                     };

        Process.Start(info).WaitForExit();

        File.Delete(timelineOutputFile);
      }
    }

    private static string GenerateIntermediateFileName(ApplicationOptions options)
    {
      return ("raw_" + Guid.NewGuid() + ((options.Renderer == OutputRenderer.WMV) ? ".wmv" : ".avi"));
    }

    private static void ValidateApplicationOptions(ApplicationOptions options)
    {
      if (!File.Exists(options.SoundtrackFile))
      {
        throw new Exception(string.Format("Could not find sound track file: {0}", options.SoundtrackFile));
      }

      if (options.SourceImage == null || options.SourceImage.Length == 0)
        throw new Exception("No source images were supplied, you must supply at least one source image");

      foreach (string imageFile in options.SourceImage)
      {
        if (!File.Exists(imageFile))
        {
          throw new Exception(string.Format("No image file could be find with the path: {0}", imageFile));
        }
      }
    }
  }
}