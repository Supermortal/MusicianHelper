using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Permissions;
using System.Text;
using System.Threading.Tasks;
using MusicianHelper.Common;
using MusicianHelper.Common.Concrete;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Infrastructure.Services.Abstract;
using MusicianHelper.Infrastructure.Services.Concrete;

namespace MusicianHelper.GenerateNewKeyAndIV
{
    class Program
    {
        static void Main(string[] args)
        {
            BootstrapCommon();
            var ss = IoCHelper.Instance.GetService<IStorageService>();

            var baseDir = ss.GetBaseDirectory();

            Security.GenerateNewIVAndKey(Path.Combine(baseDir, "Key_IV.txt"));
        }

        public static void BootstrapCommon()
        {
            Bootstraper.Start(new NinjectIoCHelper(), null);

            IoCHelper.Instance.BindService<IStorageService, BootstrapperStorageService>();
        }
    }
}
