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

using System;
using NUnit.Framework;
using Splicer.Timeline;

namespace Splicer.Tests.Timeline
{
    [TestFixture]
    public class EffectDefinitionFixture
    {
        [Test]
        public void Constructor1()
        {
            var definition = new EffectDefinition();
            Assert.AreEqual(Guid.Empty, definition.EffectId);
            Assert.AreEqual(0, definition.Parameters.Count);
        }

        [Test]
        public void Constructor2()
        {
            var definition = new EffectDefinition(StandardEffects.MatrixEffect);
            Assert.AreEqual(StandardEffects.MatrixEffect, definition.EffectId);
            Assert.AreEqual(0, definition.Parameters.Count);
        }

        [Test]
        public void SetValues()
        {
            var definition = new EffectDefinition();
            definition.EffectId = StandardEffects.AudioMixerEffect;
            Assert.AreEqual(definition.EffectId, StandardEffects.AudioMixerEffect);
        }
    }
}