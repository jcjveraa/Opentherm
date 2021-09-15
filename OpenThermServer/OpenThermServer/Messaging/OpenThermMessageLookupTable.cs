using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OpenThermServer.Messaging
{
    static class OpenThermMessageLookupTable
    {
        // Loop over the OpenThermMEssageTable.csv & make all the descriptions
    }

    class MessageDescription
    {
        public byte Nr { get; private set; }
        public string DataObject { get; private set; }
        public OpenThermMessageTypes.READ_WRITE_ALLOWED READ_WRITE_ALLOWED { get; private set; }
        public string Description { get; private set; }
        public OpenThermMessageTypes.DATA_TYPES DataType { get; private set; }

        public MessageDescription(string input)
        {
            string[] split_input = input.Split(';');
            if (split_input.Length != 5)
            {
                Console.Error.WriteLine("Not 5 items in dataString");
                return;
            }
            
            Nr = byte.Parse(split_input[0]);
            DataObject = split_input[2];
            Description = split_input[4];

            switch (split_input[1])
            {
                case "R W":
                    READ_WRITE_ALLOWED = OpenThermMessageTypes.READ_WRITE_ALLOWED.READ_WRITE;
                    break;
                case "R -":
                    READ_WRITE_ALLOWED = OpenThermMessageTypes.READ_WRITE_ALLOWED.READ;
                    break;
                case "- W":
                    READ_WRITE_ALLOWED = OpenThermMessageTypes.READ_WRITE_ALLOWED.READ;
                    break;
                default:
                    Console.Error.WriteLine("Error parsing RW");
                    Console.Error.WriteLine(input);
                    return;
            }

            switch (split_input[3])
            {
                case "f8.8":
                    DataType = OpenThermMessageTypes.DATA_TYPES.F88;
                    break;
                case "u16":
                    DataType = OpenThermMessageTypes.DATA_TYPES.U16;
                    break;
                case "s16":
                    DataType = OpenThermMessageTypes.DATA_TYPES.S16;
                    break;
                case "u8 / u8":
                    DataType = OpenThermMessageTypes.DATA_TYPES.DU8;
                    break;
                case "s8 / s8":
                    DataType = OpenThermMessageTypes.DATA_TYPES.DS8;
                    break;
                case "flag8 / flag8":
                    DataType = OpenThermMessageTypes.DATA_TYPES.DFLAG8;
                    break;
                case "flag8 / u8":
                    DataType = OpenThermMessageTypes.DATA_TYPES.FLAG8U8;
                    break;
                case "flag8 / -":
                    DataType = OpenThermMessageTypes.DATA_TYPES.SFLAG8;
                    break;
                case "special / u8":
                    DataType = OpenThermMessageTypes.DATA_TYPES.SPECIALU8;
                    break;
                default:
                    Console.Error.WriteLine("Error parsing Datatype");
                    Console.Error.WriteLine(input);
                    return;
            }
        }
    }
}
