// Copyright 2006-2008 Splicer Project - http://www.codeplex.com/splicer/
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

using NUnit.Framework;

namespace Splicer.Timeline.Tests
{
    [TestFixture]
    public class StandardEffectsFixture
    {
        [Test]
        public void CreateAlphaSetter()
        {
            StandardEffects.CreateAlphaSetter(0x99);
        }

        [Test]
        public void CreateAlphaSetterRamp()
        {
            StandardEffects.CreateAlphaSetterRamp(0.5);
        }

        [Test]
        [ExpectedException(ExceptionType = typeof (SplicerException),
            ExpectedMessage = "The alpha ramp value must be a percentage between 0 and 1")]
        public void CreateAlphaSetterRampAboveOne()
        {
            StandardEffects.CreateAlphaSetterRamp(2);
        }

        [Test]
        [ExpectedException(ExceptionType = typeof (SplicerException),
            ExpectedMessage = "The alpha ramp value must be a percentage between 0 and 1")]
        public void CreateAlphaSetterRampLowerThenZero()
        {
            StandardEffects.CreateAlphaSetterRamp(-1);
        }

        [Test]
        public void CreateAudioEnvelope()
        {
            StandardEffects.CreateAudioEnvelope(0.5);
        }

        [Test]
        [ExpectedException(ExceptionType = typeof (SplicerException),
            ExpectedMessage = "Volume percentage must be between 0 and 1")]
        public void CreateAudioEnvelopeVolumeTooHigh1()
        {
            StandardEffects.CreateAudioEnvelope(2, 1, 1, 5);
        }

        [Test]
        [ExpectedException(ExceptionType = typeof (SplicerException),
            ExpectedMessage = "Volume percentage must be between 0 and 1")]
        public void CreateAudioEnvelopeVolumeTooHigh2()
        {
            StandardEffects.CreateAudioEnvelope(2, 1, 1, 5);
        }

        [Test]
        [ExpectedException(ExceptionType = typeof (SplicerException),
            ExpectedMessage = "Volume percentage must be between 0 and 1")]
        public void CreateAudioEnvelopeVolumeTooLow1()
        {
            StandardEffects.CreateAudioEnvelope(-1);
        }

        [Test]
        [ExpectedException(ExceptionType = typeof (SplicerException),
            ExpectedMessage = "Volume percentage must be between 0 and 1")]
        public void CreateAudioEnvelopeVolumeTooLow2()
        {
            StandardEffects.CreateAudioEnvelope(-1);
        }

        [Test]
        public void CreateAudioEnvelopeWithFades()
        {
            StandardEffects.CreateAudioEnvelope(0.5, 1, 1, 5);
            StandardEffects.CreateAudioEnvelope(0.2, 0, 1, 5);
            StandardEffects.CreateAudioEnvelope(0.7, 1, 0, 5);
            StandardEffects.CreateAudioEnvelope(0.15, 0, 0, 5);
        }

        [Test]
        [
            ExpectedException(ExceptionType = typeof (SplicerException),
                ExpectedMessage = "Sum of the fade-in and fade-out durations must not exceed the total duration")]
        public void CreateAudioEnvelopeWithInvalidFadeDurations()
        {
            StandardEffects.CreateAudioEnvelope(0.5, 4, 4, 2);
        }
    }
}