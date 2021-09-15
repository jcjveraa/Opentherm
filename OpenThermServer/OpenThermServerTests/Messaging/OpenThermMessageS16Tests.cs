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
    public class OpenThermMessageS16Tests
    {
        [TestMethod()]
        public void OpenThermMessageS16Test()
        {
            OpenThermMessageS16 messageS16 = new OpenThermMessageS16("-> WriteData 1 AA AA");
            Assert.AreEqual(messageS16.Value, -21846);
        }
    }
}