namespace MusicianHelper.Infrastructure.Models
{
    public class soundcloud_user
    {
        public long id { get; set; }
        public string kind{get; set; }
        public string permalink { get; set; }
        public string username { get; set; }
        public string last_modified { get; set; }
        public string uri { get; set; }
        public string permalink_url { get; set; }
        public string avatar_url { get; set; }
    }
}
