using OpenThermServer.Messaging;
using System;

namespace OpenThermServer
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            Console.WriteLine(OpenThermMessageLookupTable.GetMessageDescriptions(1).Description);
        }
    }
}
