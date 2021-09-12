using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpenThermServer.tools;

namespace OpenThermServer.tools
{
    class OpenThermMessage<T>
    {
        public byte Id { get; set; }
        public OpenThermMessageTypes.DATA_TYPES DataType { get; protected set; }
        public byte HB { get; set; }
        public byte LB { get; set; }
        public OpenThermMessageTypes.READ_WRITE READ_WRITE { get; private set; }

        public T Value { get; }

        public OpenThermMessage(string dataString)
        {
            string[] dataSet = dataString.Split(' ');
            if(dataSet.Length != 5)
            {
                Console.Error.WriteLine("Not 5 items in dataString"); 
            }

            READ_WRITE = ParseType(dataSet[1]);

            Id = Byte.Parse(dataSet[2]);
            HB = Byte.Parse(dataSet[3], System.Globalization.NumberStyles.HexNumber);
            LB = Byte.Parse(dataSet[4], System.Globalization.NumberStyles.HexNumber);
        }


        private OpenThermMessageTypes.READ_WRITE ParseType(string type)
        {
            switch (type)
            {
                case "ReadData":
                    return OpenThermMessageTypes.READ_WRITE.READ_DATA;
                case "ReadAck":
                    return OpenThermMessageTypes.READ_WRITE.READ_ACK;
                case "WriteData":
                    return OpenThermMessageTypes.READ_WRITE.WRITE_DATA;
                case "WriteAck":
                    return OpenThermMessageTypes.READ_WRITE.WRITE_ACK;
                case "InvalidData":
                    return OpenThermMessageTypes.READ_WRITE.INVALID_DATA;
                case "DataInvalid":
                    return OpenThermMessageTypes.READ_WRITE.DATA_INVALID;

                default:
                    return OpenThermMessageTypes.READ_WRITE.UNKNOWNID;
            }
        }

    }

    class OpenThermMessageF88: OpenThermMessage<float>
    {

        public OpenThermMessageF88(string dataString) : base(dataString)
        {               
            DataType = OpenThermMessageTypes.DATA_TYPES.F88;
        }

        public new float Value
        {
            get
            {
                sbyte a = unchecked((sbyte)HB); // Cast the higher bit to an 
                return (float)(a + LB / 256.0);
            }
        }
    }
    class OpenThermMessageS16 : OpenThermMessage<short>
    {

        public OpenThermMessageS16(string dataString) : base(dataString)
        {
            DataType = OpenThermMessageTypes.DATA_TYPES.S16;
        }

        public new short Value
        {
            get
            {
                sbyte a = unchecked((sbyte)HB); // Cast the higher bit to an 
                return (short)(a << 8 + LB);
            }
        }
    }
}
