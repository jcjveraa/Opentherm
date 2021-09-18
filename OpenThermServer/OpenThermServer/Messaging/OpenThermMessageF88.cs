namespace OpenThermServer.Messaging
{
    public class OpenThermMessageF88 : OpenThermMessageBase
    {

        public OpenThermMessageF88(string dataString) : base(dataString)
        {

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
