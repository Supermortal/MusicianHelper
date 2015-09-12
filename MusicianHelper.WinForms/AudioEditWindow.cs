using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using log4net;
using MusicianHelper.Common.Helpers;
using MusicianHelper.Common.Helpers.Log;
using MusicianHelper.Infrastructure.Models;
using MusicianHelper.Infrastructure.Services.Abstract;

namespace MusicianHelper.WinForms
{
    public partial class AudioEditWindow : Form
    {

        private static readonly ILog Log = LogHelper.GetLogger(typeof (AudioEditWindow));

        private readonly List<AudioUoW> _audios;
        private readonly IMasterService _ms;

        private int _currentPage = 1;
        private AudioUoW _currentAudio;

        public AudioEditWindow(List<AudioUoW> audios) : this(audios, IoCHelper.Instance.GetService<IMasterService>())
        {

        }

        public AudioEditWindow(List<AudioUoW> audios, IMasterService ms)
        {
            _ms = ms;
            _audios = audios;
            InitializeComponent();
            Setup();
        }

        private void Setup()
        {
            try
            {
                if (_audios == null || _audios.Count == 0)
                    ShowNoAudiosMessage();

                _currentAudio = _audios[0];
                SetFields(_currentAudio);

                CalculateLimits();
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void CalculateLimits()
        {
            try
            {
                NextButton.Enabled = true;
                PreviousButton.Enabled = true;

                if (_currentPage == _audios.Count)
                    NextButton.Enabled = false;
                if (_currentPage == 1)
                    PreviousButton.Enabled = false;
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void ShowNoAudiosMessage()
        {
            throw new NotImplementedException();
            //try
            //{

            //}
            //catch (Exception ex)
            //{
            //    Log.Error(ex.Message, ex);
            //}
        }

        private void SetFields(AudioUoW audio)
        {
            try
            {
                TitleTextBox.Text = audio.Title;
                DescriptionTextBox.Text = audio.Description;
                TagsTextBox.Text = _ms.GenerateTagsString(audio.Tags);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void UpdateAudio(AudioUoW audio)
        {
            try
            {
                audio.Title = TitleTextBox.Text;
                audio.Description = DescriptionTextBox.Text;
                audio.Tags = _ms.GetTagsFromTagString(TagsTextBox.Text);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        #region Events
        private void PreviousButton_Click(object sender, EventArgs e)
        {
            try
            {
                if (_currentPage == 1) return;

                UpdateAudio(_currentAudio);

                _currentPage--;
                CalculateLimits();

                _currentAudio = _audios[_currentPage - 1];
                SetFields(_currentAudio);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void DoneButton_Click(object sender, EventArgs e)
        {
            try
            {
                Close();
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }

        private void NextButton_Click(object sender, EventArgs e)
        {
            try
            {
                if (_currentPage == _audios.Count) return;

                UpdateAudio(_currentAudio);

                _currentPage++;
                CalculateLimits();

                _currentAudio = _audios[_currentPage - 1];
                SetFields(_currentAudio);
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message, ex);
            }
        }
        #endregion

    }
}
