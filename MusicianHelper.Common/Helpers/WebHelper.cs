using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace MusicianHelper.Common.Helpers
{
    public class WebHelper
    {
        public WebHelper()
        {
        }

        public async static Task<T> Get<T>(string url)
        {
            return await Get<T>(url, null);
        }

        public async static Task<T> Get<T>(string url, Dictionary<string, object> data)
        {
            return await Get<T>(url, data, null);
        }

        public async static Task<T> Get<T>(string url, Dictionary<string, object> data, Dictionary<string, string> headers)
        {
            if (data != null && data.Count > 0)
                url += ToQueryString(data);

            using (var client = new HttpClient())
            {
                //                    client.BaseAddress = new Uri(url);
                client.DefaultRequestHeaders.Accept.Clear();
                client.DefaultRequestHeaders.Accept.Add(new System.Net.Http.Headers.MediaTypeWithQualityHeaderValue("application/json"));

                if (headers != null)
                {
                    foreach (var key in headers.Keys)
                    {
                        client.DefaultRequestHeaders.Add(key, headers[key]);
                    }
                }

                var response = await client.GetAsync(url).ConfigureAwait(false);
                if (response.IsSuccessStatusCode)
                {
                    var responseStr = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
                    return JsonConvert.DeserializeObject<T>(responseStr);
                }
            }

            return default(T);
        }

        public async static Task<T> Post<T>(string url, object data)
        {
            return await Post<T>(url, data, null);
        }

        public async static Task<T> Post<T>(string url, object data, Dictionary<string, string> headers)
        {
            try
            {

                using (var client = new HttpClient())
                {
                    //                    client.BaseAddress = new Uri(url);
                    //client.DefaultRequestHeaders.Accept.Clear();
                    //client.DefaultRequestHeaders.Accept.Add(new System.Net.Http.Headers.MediaTypeWithQualityHeaderValue("application/json"));

                    if (headers != null)
                    {
                        foreach (var key in headers.Keys)
                        {
                            client.DefaultRequestHeaders.Add(key, headers[key]);
                        }
                    }


                    var response = await client.PostAsync(url, new StringContent(JsonConvert.SerializeObject(data), Encoding.UTF8, "application/json")).ConfigureAwait(false);
                    if (response.IsSuccessStatusCode)
                    {
                        var responseStr = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
                        return JsonConvert.DeserializeObject<T>(responseStr);
                    }
                }

                return default(T);

            }
            catch (Exception ex)
            {
                return default(T);
            }
        }

        public async static Task<T> PostFile<T>(string url, Dictionary<string, object> formData, Dictionary<string, byte[]> fileData)
        {
            try
            {
                return await PostFile<T>(url, formData, fileData, null);
            }
            catch (Exception ex)
            {
                return default(T);
            }
        }

        public async static Task<T> PostFile<T>(string url, Dictionary<string, object> formData, Dictionary<string, byte[]> fileData, Dictionary<string, string> headers)
        {
            try
            {
                using (var client = new HttpClient())
                {
                    if (headers != null)
                    {
                        foreach (var key in headers.Keys)
                        {
                            client.DefaultRequestHeaders.Add(key, headers[key]);
                        }
                    }

                    var fd = new MultipartFormDataContent();

                    foreach (var key in formData.Keys)
                    {
                        var value = formData[key];
                        if (value == null) continue;

                        var sc = new StringContent(value.ToString());
                        sc.Headers.ContentType = null;
                        fd.Add(sc, key);
                    }
                    
                    //var bc = new ByteArrayContent(fileData);
                    //bc.Headers.ContentType = new MediaTypeHeaderValue("application/octet-stream");
                    //fd.Add(bc, fileDataKey);

                    foreach (var key in fileData.Keys)
                    {
                        var value = fileData[key];
                        if (value == null) continue;

                        var bc = new ByteArrayContent(value);
                        bc.Headers.ContentType = new MediaTypeHeaderValue("application/octet-stream");
                        fd.Add(bc, key);
                    }

                    var response = await client.PostAsync(url, fd).ConfigureAwait(false);
                    if (response.IsSuccessStatusCode)
                    {
                        var responseStr = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
                        return JsonConvert.DeserializeObject<T>(responseStr);
                    }
                }

                return default(T);

            }
            catch (Exception ex)
            {
                return default(T);
            }
        }

        public async static Task<T> Post<T>(string url, string data, Dictionary<string, string> headers)
        {
            try
            {

                using (var client = new HttpClient())
                {
                    //                    client.BaseAddress = new Uri(url);
                    //client.DefaultRequestHeaders.Accept.Clear();
                    //client.DefaultRequestHeaders.Accept.Add(new System.Net.Http.Headers.MediaTypeWithQualityHeaderValue("application/json"));

                    if (headers != null)
                    {
                        foreach (var key in headers.Keys)
                        {
                            client.DefaultRequestHeaders.Add(key, headers[key]);
                        }
                    }

                    var response = await client.PostAsync(url, new StringContent(data, Encoding.UTF8, "application/json")).ConfigureAwait(false);
                    if (response.IsSuccessStatusCode)
                    {
                        var responseStr = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
                        return JsonConvert.DeserializeObject<T>(responseStr);
                    }
                }

                return default(T);

            }
            catch (Exception ex)
            {
                return default(T);
            }
        }

        public async static Task<HttpResponseMessage> GetRawResponsePost(string url, string data, Dictionary<string, string> headers)
        {
            try
            {

                using (var client = new HttpClient())
                {
                    //                    client.BaseAddress = new Uri(url);
                    //client.DefaultRequestHeaders.Accept.Clear();
                    //client.DefaultRequestHeaders.Accept.Add(new System.Net.Http.Headers.MediaTypeWithQualityHeaderValue("application/json"));

                    if (headers != null)
                    {
                        foreach (var key in headers.Keys)
                        {
                            client.DefaultRequestHeaders.Add(key, headers[key]);
                        }
                    }

                    return await client.PostAsync(url, new StringContent(data, Encoding.UTF8, "application/json")).ConfigureAwait(false);
                }

            }
            catch (Exception ex)
            {
                return null;
            }
        }

        public async static Task<T> Post<T>(string url, Stream contentStream, Dictionary<string, string> headers)
        {
            try
            {

                using (var client = new HttpClient())
                {
                    //                    client.BaseAddress = new Uri(url);
                    //client.DefaultRequestHeaders.Accept.Clear();
                    //client.DefaultRequestHeaders.Accept.Add(new System.Net.Http.Headers.MediaTypeWithQualityHeaderValue("application/json"));

                    if (headers != null)
                    {
                        foreach (var key in headers.Keys)
                        {
                            client.DefaultRequestHeaders.Add(key, headers[key]);
                        }
                    }

                    var response = await client.PostAsync(url, new StreamContent(contentStream)).ConfigureAwait(false);
                    if (response.IsSuccessStatusCode)
                    {
                        var responseStr = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
                        return JsonConvert.DeserializeObject<T>(responseStr);
                    }
                }

                return default(T);

            }
            catch (Exception ex)
            {
                return default(T);
            }
        }

        public static string ToQueryString(Dictionary<string, object> data)
        {
            StringBuilder sb = new StringBuilder();

            sb.Append("?");

            foreach (var key in data.Keys)
            {
                var value = data[key];
                sb.Append(key);
                sb.Append('=');

                var encodedValue = string.Empty;
                if (value != null)
                {
                    encodedValue = Uri.EscapeDataString(value.ToString());
                }

                sb.Append(encodedValue);
                sb.Append('&');
            }

            return sb.ToString().TrimEnd('&');
        }
    }
}
