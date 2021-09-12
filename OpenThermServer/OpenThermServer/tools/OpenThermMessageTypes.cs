using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OpenThermServer.tools
{

    public static class OpenThermMessageTypes
    {
        //flag8 byte composed of 8 single-bit flags
        //u8 unsigned 8-bit integer 0 .. 255
        //s8 signed 8-bit integer -128 .. 127 (two’s compliment)
        //f8.8 signed fixed point value : 1 sign bit, 7 integer bit, 8 fractional bits(two’s compliment ie.the
        //LSB of the 16bit binary number represents 1 / 256th of a unit).
        //u16 unsigned 16 - bit integer 0..65535
        //s16 signed 16 - bit integer - 32768..32767
        //Example : A temperature of 21.5°C in f8.8 format is represented by the 2 - byte value 1580 hex
        //(1580hex = 5504dec, dividing by 256 gives 21.5)
        //A temperature of - 5.25°C in f8.8 format is represented by the 2 - byte value FAC0 hex
        //(FAC0hex = -(10000hex - FACOhex) = -0540hex = -1344dec, dividing by 256 gives - 5.25)
        public enum DATA_TYPES
        {
            F88, U16, S16, U8, S8, DU8, DS8, DFLAG8, FLAG8U8, SPECIALU8
        }

        public enum READ_WRITE
        {
            READ_DATA, READ_ACK, WRITE_DATA, WRITE_ACK, UNKNOWNID, INVALID_DATA, DATA_INVALID
        }

        //bit: description[clear / 0, set / 1]
        //0: CH enable[CH is disabled, CH is enabled]
        //1: DHW enable[DHW is disabled, DHW is enabled]
        //2: Cooling enable[Cooling is disabled, Cooling is
        //enabled]
        //3: OTC active[OTC not active, OTC is active]
        //4: CH2 enable[CH2 is disabled, CH2 is enabled]
        //5: reserved
        //6: reserved
        //7: reserved
        public enum MASTER_STATUS_BITMASK
        {
            CH_ENABLE = 0b_0000_0001,  // 0
            CENTRAL_HEATING_ENABLE = CH_ENABLE,  // 0
            DHW_ENABLE = 0b_0000_0010,  // 1
            DOMESTIC_HOT_WATER_ENABLE = DHW_ENABLE,
            COOLING_ENABLE = 0b_0000_0100,  // 2
            OTC_ACTIVE = 0b_0000_1000,  // 4
            CH2_ENABLE = 0b_0001_0000,  // 8
            RESERVED_5 = 0b_0010_0000,  // 16
            RESERVED_6 = 0b_0100_0000,  // 32
            RESERVED_7 = 0b_1000_0000,  // 64
        }

        //bit: description[clear / 0, set / 1]
        //0: Service request[service not req’d, service required]
        //1: Lockout-reset[remote reset disabled, rr enabled]
        //2: Low water press[no WP fault, water pressure fault]
        //3: Gas/flame fault[no G / F fault, gas / flame fault]
        //4: Air press fault[no AP fault, air pressure fault]
        //5: Water over-temp[no OvT fault, over - temperat.Fault]
        //6: reserved
        //7: reserved
        public enum SLAVE_STATUS_BITMASK
        {
            FAULT_INDICATION = 0b_0000_0001,  // 0
            CH_MODE = 0b_0000_0010,  // 1
            CENTRAL_HEATING_MODE = CH_MODE,
            DOMESTIC_HOT_WATER_MODE = DHW_MODE,
            DHW_MODE = 0b_0000_0100,  // 2
            FLAME_STATUS = 0b_0000_1000,  // 4
            COOLING_STATUS = 0b_0001_0000,  // 8
            CH2_MODE = 0b_0010_0000,  // 16
            DIAGNOSTIC_INDICATION = 0b_0100_0000,  // 32
            RESERVED_7 = 0b_1000_0000,  // 64
        }


    }
}
