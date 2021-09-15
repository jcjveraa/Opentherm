namespace OpenThermServer.Messaging
{
    public class OpenThermMessageS16 : OpenThermMessageBase, IHasValue<short>
    {

        public OpenThermMessageS16(string dataString) : base(dataString)
        {
            DataType = OpenThermMessageTypes.DATA_TYPES.S16;
        }

        public short Value
        {
            get
            {
                //sbyte a = unchecked((sbyte)HB); // Cast the higher bit to an 
                return (short)((HB << 8) + LB);
            }
        }
    }
}
