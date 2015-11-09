namespace MusicianHelper.Infrastructure.Models
{
    public class soundcloud_track
    {
        public string kind { get; set; }
        public long id { get; set; }
        public string created_at { get; set; }
        public string user_id { get; set; }
        public long duration{ get; set; }
        public bool commentable{ get; set; }
        public string state { get; set; }
        public long? original_content_size { get; set; }
        public string last_modified { get; set; }
        public string sharing { get; set; }
        public string tag_list { get; set; }
        public string permalink{ get; set; }
        public bool streamable{ get; set; }
        public string embeddable_by { get; set; }
        public bool downloadable { get; set; }
        public string purchase_url { get;set; }
        public long? label_id { get; set; }
        public string purchase_title { get; set; }
        public string genre { get; set; }
        public string title { get; set; }
        public string description { get; set; }
        public string label_name { get; set; }
        public string release { get; set; }
        public string track_type { get; set; }
        public string key_signature { get; set; }
        public string isrc { get; set; }
        public string video_url { get; set; }
        public int? bpm { get; set; }
        public int? release_year { get; set; }
        public int? release_month { get; set; }
        public int? release_day { get; set; }
        public string original_format { get; set; }
        public string license { get; set; }
        public string uri { get; set; }
        public soundcloud_user user { get; set; }
        public long user_playback_count { get; set; }
        public bool user_favorite { get; set; }
        public string permalink_url { get; set; }
        public string artwork_url { get; set; }
        public string waveform_url { get; set; }
        public string stream_url { get; set; }
        public string download_url { get; set; }
        public long playback_count { get; set; }
        public long download_count { get; set; }
        public long favoritings_count { get; set; }
        public long comment_count { get; set; }
        public string attachments_uri { get; set; }
        public string secret_token { get; set; }
        public string secret_uri { get; set; }
        public int downloads_remaining { get; set; }
    }
}
