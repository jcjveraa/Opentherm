
using Microsoft.Extensions.Configuration;
using OpenThermServer.Messaging;
using System;
using System.IO;

namespace OpenThermServer
{
    class Program
    {
        static void Main(string[] args)
        {
            var config =
    new ConfigurationBuilder()
        //.SetBasePath(Directory.GetCurrentDirectory())
        .AddJsonFile("appsettings.json", true)
        .AddEnvironmentVariables()
        .Build();

            Console.WriteLine("Hello World!");

            Console.WriteLine(OpenThermMessageLookupTable.GetMessageDescriptions(1).Description);
        }
    }
}
