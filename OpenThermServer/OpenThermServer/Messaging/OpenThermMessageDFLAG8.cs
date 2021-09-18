namespace OpenThermServer.Messaging
{
    public class OpenThermMessageDFLAG8 : OpenThermMessageBase
    {

        public OpenThermMessageDFLAG8(string dataString) : base(dataString)
        {
        }

        public ushort Value
        {
            get
            {
                return (ushort)(HB << 8 + LB);
            }
        }

        public bool IsHBBitSet(byte bitnumber)
        {
            return IsBitSet(HB, bitnumber);
        }
        public bool IsLBBitSet(byte bitnumber)
        {
            return IsBitSet(LB, bitnumber);
        }

        private static bool IsBitSet(byte b, int pos)
        {
            return (b & (1 << pos)) != 0;
        }
    }
}
