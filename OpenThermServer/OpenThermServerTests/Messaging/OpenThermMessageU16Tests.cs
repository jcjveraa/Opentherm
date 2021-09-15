using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenThermServer.Messaging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OpenThermServer.Messaging.Tests
{
    [TestClass()]
    public class OpenThermMessageU16Tests
    {
        [TestMethod()]
        public void OpenThermMessageU16Test()
        {
            OpenThermMessageU16 messageU16 = new OpenThermMessageU16("-> WriteData 1 AA aa");
            Assert.AreEqual((ushort)43690, messageU16.Value);
        }
    }
}