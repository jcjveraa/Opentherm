namespace OpenThermServer.Messaging
{
    public class OpenThermMessageS16 : OpenThermMessageBase
    {

        public OpenThermMessageS16(string dataString) : base(dataString)
        {

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
