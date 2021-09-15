using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace OpenThermServer.Messaging.Tests
{
    [TestClass()]
    public class OpenThermMessageF88Tests
    {
        [TestMethod()]
        public void OpenThermMessageF88Test()
        {
            OpenThermMessageF88 messageF88 = new OpenThermMessageF88("-> WriteData 1 15 80");
            Assert.AreEqual(messageF88.Value, 21.5);
            messageF88 = new OpenThermMessageF88("-> WriteData 1 fa c0");
            Assert.AreEqual(messageF88.Value, -5.25);

            Assert.IsTrue(messageF88.READ_WRITE == OpenThermMessageTypes.DATA_DIRECTION.WRITE_DATA);
            Assert.AreEqual(messageF88.Id, 1);
        }
    }
}