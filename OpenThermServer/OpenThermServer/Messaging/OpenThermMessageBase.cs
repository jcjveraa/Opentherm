using System;

namespace OpenThermServer.Messaging
{
    public class OpenThermMessageBase
    {
        public byte Nr { get; private set; }
        public OpenThermMessageTypes.DATA_TYPES DataType { get; private set; }
        public byte HB { get; private set; }
        public byte LB { get; private set; }
        public OpenThermMessageTypes.DATA_DIRECTION READ_WRITE { get; private set; }

        public OpenThermMessageBase(string dataString)
        {
            string[] dataSet = ParseMessageString(dataString);

            Nr = byte.Parse(dataSet[2]);

            READ_WRITE = ParseType(dataSet[1]);

            HB = byte.Parse(dataSet[3], System.Globalization.NumberStyles.HexNumber);
            LB = byte.Parse(dataSet[4], System.Globalization.NumberStyles.HexNumber);
            DataType = OpenThermMessageLookupTable.GetMessageDescriptions(Nr).DataType;
        }

        public static string[] ParseMessageString(string dataString)
        {
            string[] dataSet = dataString.Split(' ');
            if (dataSet.Length != 5)
            {
                Console.Error.WriteLine("Not 5 items in dataString");
                return null;
            }

            return dataSet;
        }

        public static byte GetMessageNrFromDataString(string dataString)
        {
            return byte.Parse(ParseMessageString(dataString)[2]);
        }

        //public dynamic Value { get; set; }

        private OpenThermMessageTypes.DATA_DIRECTION ParseType(string type)
        {
            switch (type)
            {
                case "ReadData":
                    return OpenThermMessageTypes.DATA_DIRECTION.READ_DATA;
                case "ReadAck":
                    return OpenThermMessageTypes.DATA_DIRECTION.READ_ACK;
                case "WriteData":
                    return OpenThermMessageTypes.DATA_DIRECTION.WRITE_DATA;
                case "WriteAck":
                    return OpenThermMessageTypes.DATA_DIRECTION.WRITE_ACK;
                case "InvalidData":
                    return OpenThermMessageTypes.DATA_DIRECTION.INVALID_DATA;
                case "DataInvalid":
                    return OpenThermMessageTypes.DATA_DIRECTION.DATA_INVALID;

                default:
                    return OpenThermMessageTypes.DATA_DIRECTION.UNKNOWNID;
            }
        }

    }
}
