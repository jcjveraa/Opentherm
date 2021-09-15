namespace OpenThermServer.Messaging
{
    public class OpenThermMessageF88 : OpenThermMessageBase, IHasValue<float>
    {

        public OpenThermMessageF88(string dataString) : base(dataString)
        {
            DataType = OpenThermMessageTypes.DATA_TYPES.F88;
        }

        public float Value
        {
            get
            {
                sbyte a = unchecked((sbyte)HB); // Cast the higher bit to an 
                return (float)(a + LB / 256.0);
            }
        }
    }
}
