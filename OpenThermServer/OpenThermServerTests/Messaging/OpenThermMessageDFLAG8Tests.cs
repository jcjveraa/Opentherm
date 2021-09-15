using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace OpenThermServer.Messaging.Tests
{
    [TestClass()]
    public class OpenThermMessageDFLAG8Tests
    {
        [TestMethod()]
        public void IsHBBitSetTest()
        {
            OpenThermMessageDFLAG8 messageDFLAG8 = new OpenThermMessageDFLAG8("<- ReadAck 0 2 0");
            Assert.AreEqual(messageDFLAG8.IsHBBitSet(0), false);
            Assert.AreEqual(messageDFLAG8.IsHBBitSet(1), true);
            Assert.AreEqual(messageDFLAG8.IsHBBitSet(2), false);

            messageDFLAG8 = new OpenThermMessageDFLAG8("<- ReadAck 0 7 0");
            Assert.AreEqual(messageDFLAG8.IsHBBitSet(0), true);
            Assert.AreEqual(messageDFLAG8.IsHBBitSet(1), true);
            Assert.AreEqual(messageDFLAG8.IsHBBitSet(2), true);

            messageDFLAG8 = new OpenThermMessageDFLAG8("<- ReadAck 0 7 7");
            Assert.AreEqual(messageDFLAG8.IsLBBitSet(0), true);
            Assert.AreEqual(messageDFLAG8.IsLBBitSet(1), true);
            Assert.AreEqual(messageDFLAG8.IsLBBitSet(2), true);

            Assert.AreEqual(messageDFLAG8.IsLBBitSet(7), false);
        }
    }
}