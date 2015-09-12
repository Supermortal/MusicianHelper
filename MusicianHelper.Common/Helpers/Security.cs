using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using log4net;
using MusicianHelper.Common.Helpers.Log;

namespace MusicianHelper.Common.Helpers
{
    public class Security
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (Security));

        private static byte[] _iv = new byte[] { 138,
		68,
		204,
		7,
		198,
		219,
		11,
		133};

        private static byte[] _key = new byte[] { 216,
		12,
		40,
		118,
		44,
		49,
		125,
		149,
		57,
		69,
		170,
		118,
		122,
		178,
		18,
		62,
		140,
		2,
		103,
		59,
		190,
		39,
		129,
		135};

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

    }
}
