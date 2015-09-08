using MusicianHelper.Common.Models.Enums;

namespace MusicianHelper.Common.Abstract
{
  public interface IIoCHelper
  {
      T GetService<T>();
      void BindService<T1, T2>(IoCBindingType bindingType = IoCBindingType.Normal, string name = null) where T2 : T1;
      T GetNamedBinding<T>(string name);
  }
}