namespace OpenThermServer.Messaging
{
    public class OpenThermMessageU16 : OpenThermMessageBase
    {
        public OpenThermMessageU16(string dataString) : base(dataString)
        {
        }

        public ushort Value
        {
            get
            {
                return (ushort)((HB << 8) + LB);
            }
        }
    }
}
