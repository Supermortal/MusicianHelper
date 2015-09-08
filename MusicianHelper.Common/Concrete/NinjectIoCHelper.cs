using MusicianHelper.Common.Abstract;
using MusicianHelper.Common.Models.Enums;
using Ninject;

namespace MusicianHelper.Common.Concrete
{
    public class NinjectIoCHelper : IIoCHelper
    {

        private IKernel _kernel = new StandardKernel();

        public NinjectIoCHelper()
        {
        }

        public T GetService<T>()
        {
            return (T)_kernel.TryGet(typeof(T));
        }

        public void BindService<T1, T2>(IoCBindingType bindingType = IoCBindingType.Normal, string name = null) where T2 : T1
        {
            var binding = _kernel.Bind<T1>().To<T2>();

            if (!string.IsNullOrEmpty(name))
                binding.Named(name);

            if (bindingType == IoCBindingType.Singleton)
                binding.InSingletonScope();
        }

        public T GetNamedBinding<T>(string name)
        {
            return (T)_kernel.TryGet(typeof(T), name, null);
        }

    }
}

