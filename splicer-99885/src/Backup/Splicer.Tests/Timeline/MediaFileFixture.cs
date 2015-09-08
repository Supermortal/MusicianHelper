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

using System.IO;
using NUnit.Framework;

namespace Splicer.Timeline.Tests
{
    [TestFixture]
    public class MediaFileFixture
    {
        [Test]
        public void ConstructAndDispose()
        {
            var file = new MediaFile("transitions.wmv");
            string filePath = Path.GetFullPath("transitions.wmv");
            Assert.AreEqual(filePath, file.FileName);
            Assert.AreEqual(7.999, file.Length);
            Assert.AreEqual(TimelineBuilder.ToUnits(7.999), file.LengthInUnits);
            Assert.AreEqual(-1, file.LengthInFrames); // not assigned till later

            file.Dispose();
            Assert.IsTrue(File.Exists("transitions.wmv"));
        }

        [Test]
        public void ConstructWithManagedLifespanAndDispose()
        {
            File.Copy("image1.jpg", "temp.jpg");
            var file = new MediaFile("temp.jpg", true);
            Assert.IsTrue(File.Exists(file.FileName));
            file.Dispose();
            Assert.IsFalse(File.Exists(file.FileName));
        }

        [Test]
        public void SetLength()
        {
            var file = new MediaFile("transitions.wmv");
            file.LengthInUnits = TimelineBuilder.ToUnits(2);
            Assert.AreEqual(2, file.Length);
        }

        [Test]
        [ExpectedException(ExceptionType = typeof (SplicerException), ExpectedMessage = "Invalid length specified")]
        public void SetLengthToNegative()
        {
            var file = new MediaFile("transitions.wmv");
            file.LengthInUnits = -1;
        }

        [Test]
        [ExpectedException(ExceptionType = typeof (SplicerException), ExpectedMessage = "Invalid length specified")]
        public void SetLengthTooLong()
        {
            var file = new MediaFile("transitions.wmv");
            file.LengthInUnits = TimelineBuilder.ToUnits(20);
        }

        [Test]
        public void SetLengthToZero()
        {
            var file = new MediaFile("transitions.wmv");
            file.LengthInUnits = 0;
        }
    }
}