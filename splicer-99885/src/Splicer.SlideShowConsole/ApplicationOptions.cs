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

using CommandLine;

namespace Splicer.SlideShowConsole
{
  public class ApplicationOptions
  {
    [Argument(ArgumentType.Multiple,HelpText = "The source image files", ShortName = "img")]
    public string[] SourceImage;
    [Argument(ArgumentType.Required, HelpText = "The audio file to accompany the video", ShortName = "snd")]
    public string SoundtrackFile;
    [Argument(ArgumentType.AtMostOnce, HelpText = "The width of the video clip", ShortName = "x", DefaultValue = 176)]
    public int Width;
    [Argument(ArgumentType.AtMostOnce, HelpText = "The height of the video clip", ShortName = "y", DefaultValue = 144)]
    public int Height;
    [Argument(ArgumentType.AtMostOnce, HelpText = "The filename to output to", ShortName = "out",DefaultValue = "output.wmv")]
    public string OutputVideo;
    [Argument(ArgumentType.AtMostOnce, HelpText = "Output format", ShortName = "fmt", DefaultValue = OutputRenderer.WMV)] 
    public OutputRenderer Renderer;
    [Argument(ArgumentType.AtMostOnce, HelpText = "Frames Per Second", ShortName = "fps", DefaultValue = 25)] 
    public int FPS;
    [Argument(ArgumentType.AtMostOnce, HelpText = "FFMPEG Path", ShortName = "ffmpeg", DefaultValue = "ffmpeg.exe")]
    public string FfmpegPath;
    [Argument(ArgumentType.AtMostOnce, HelpText = "Encode to 3GP With FFMPEG?", ShortName = "3gp", DefaultValue = false)] 
    public bool EncodeTo3gpWithFfmpeg;
  }

  public enum OutputRenderer
  {
    AVI,
    WMV
  }
}