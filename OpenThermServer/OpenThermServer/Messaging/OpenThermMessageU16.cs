namespace OpenThermServer.Messaging
{
    public class OpenThermMessageU16 : OpenThermMessageBase, IHasValue<ushort>
    {
        public OpenThermMessageU16(string dataString) : base(dataString)
        {
            DataType = OpenThermMessageTypes.DATA_TYPES.U16;
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
