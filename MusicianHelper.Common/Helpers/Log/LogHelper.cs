using System;
using System.IO;
using log4net;

namespace MusicianHelper.Common.Helpers.Log
{
    public static class LogHelper
    {

        private static volatile bool _configured = false;
        public static bool Configured
        {
            get { return _configured; }
            private set { _configured = value; }
        }

        private static volatile bool _triedToConfigure = false;
        public static bool TriedToConfigure
        {
            get { return _triedToConfigure; }
            private set { _triedToConfigure = value; }
        }

        private static volatile bool _usingEncryption = false;
        public static bool UsingEncryption
        {
            get { return _usingEncryption; }
            private set { _usingEncryption = value; }
        }

        private static bool _autoConfigure = true;
        public static bool AutoConfigure
        {
            get { return _autoConfigure; }
            set { _autoConfigure = value; }
        }

        public static string ConfigFilePath { get; set; }

        private static readonly object Lock = new object();

        public static ILog GetLogger(Type type)
        {
            try
            {
                if (!Configured && !TriedToConfigure)
                {
                    Configure();
                }

                return (Configured) ? LogManager.GetLogger(type) : new Log();
            }
            catch (Exception ex)
            {
                Configured = false;
                return null;
            }
        }

        public static void Configure(string configFilePath, bool useEncryption = false)
        {
            try
            {
                UsingEncryption = useEncryption;
                ConfigFilePath = configFilePath;
                Configure();
            }
            catch (Exception ex)
            {
                Configured = false;
            }
        }

        private static void Configure()
        {
            try
            {

                lock (Lock)
                {
                    //reduce lock overhead with 2 checks of _configured
                    if (!Configured)
                    {
                        TriedToConfigure = true;

                        var havePath = true;
                        if (string.IsNullOrEmpty(ConfigFilePath) && AutoConfigure)
                            havePath = SetDefaultPath();

                        if (!havePath)
                        {
                            Configured = false;
                            return;
                        }

                        var fi = new FileInfo(ConfigFilePath);
                        if (fi.Exists)
                        {
                            log4net.Config.XmlConfigurator.Configure(fi);

                            Configured = true;
                        }
                        else
                        {
                            Configured = false;
                        }

                    }
                }

            }
            catch (Exception ex)
            {
                Configured = false;
            }
        }

        private static bool SetDefaultPath()
        {
            try
            {
                var appDomain = AppDomain.CurrentDomain.BaseDirectory;
                ConfigFilePath = Path.Combine(appDomain, "log4net.config");

                return File.Exists(ConfigFilePath);
            }
            catch (Exception ex)
            {
                ConfigFilePath = null;
                return false;
            }
        }

    }
}