using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using log4net;
using MusicianHelper.Common.Helpers.Log;

namespace MusicianHelper.Common.Helpers
{
    public class Security
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof(Security));

        private static byte[] _iv = new byte[] { 46, 48, 182, 51, 145, 0, 105, 200 };

        private static byte[] _key = new byte[] { 145, 206, 185, 51, 21, 224, 82, 188, 49, 189, 235, 72, 217, 42, 81, 217, 38, 88, 103, 122, 28, 15, 93, 227 };

        public static byte[] IV
        {
            private get { return _iv; }
            set { _iv = value; }
        }

        public static byte[] Key
        {
            private get { return _key; }
            set { _key = value; }
        }

        public static string SymmetricEncypt(byte[] raw)
        {
            try
            {
                var TDES = new TripleDESCryptoServiceProvider { IV = IV, Key = Key };
                var encryptor = TDES.CreateEncryptor();
                var msEncrypt = new MemoryStream();
                var csEncrypt = new CryptoStream(msEncrypt, encryptor, CryptoStreamMode.Write);

                //encryption
                csEncrypt.Write(raw, 0, raw.Length);
                csEncrypt.FlushFinalBlock();

                return Convert.ToBase64String(msEncrypt.ToArray());
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
                return null;
            }
        }

        public static byte[] SymmetricDecrypt(string encrypted)
        {
            var TDES = new TripleDESCryptoServiceProvider();
            TDES.IV = IV;
            TDES.Key = Key;

            //symmetric decryptor
            var decryptor = TDES.CreateDecryptor();
            var msDecrypt = new MemoryStream(Convert.FromBase64String(encrypted));
            //decryption stream
            var csDecrypt = new CryptoStream(msDecrypt, decryptor, CryptoStreamMode.Read);

            //byte array for encrypted BLOB
            var fromEncrypt = new byte[encrypted.Length];
            //decrypt the BLOB
            csDecrypt.Read(fromEncrypt, 0, fromEncrypt.Length);

            return fromEncrypt;
        }

        public static void Configure(byte[] iv, byte[] key)
        {
            IV = iv;
            Key = key;
        }

        public static void GenerateNewIVAndKey(string path)
        {
            try
            {
                var encrypt = new TripleDESCryptoServiceProvider();

                encrypt.GenerateIV();
                encrypt.GenerateKey();

                var sb = new StringBuilder();
                sb.Append("private static byte[] _iv = new byte[] {");
                foreach (var b in encrypt.IV)
                {
                    sb.Append(b);
                    sb.Append(',');
                }

                var ivFront = sb.ToString().TrimEnd(',');
                sb = new StringBuilder();

                sb.Append(ivFront);
                sb.Append("};");

                var ivStr = sb.ToString();

                sb = new StringBuilder();
                sb.Append("private static byte[] _key = new byte[] {");
                foreach (var b in encrypt.Key)
                {
                    sb.Append(b);
                    sb.Append(',');
                }

                var keyFront = sb.ToString().TrimEnd(',');
                sb = new StringBuilder();

                sb.Append(keyFront);
                sb.Append("};");

                var keyStr = sb.ToString();

                var lines = new string[] { ivStr, "\n", keyStr };
                File.WriteAllLines(path, lines);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

    }
}
