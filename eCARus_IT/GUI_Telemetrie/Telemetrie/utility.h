#include <QString>

namespace utility
{
    int	HexStringToCharArray (std::string &string, unsigned char* array, int string_size, int array_size);
    int CharArrayToHexString (wchar_t* string, unsigned char* array, int array_size);
    int CharArrayToHexString (std::string &string, unsigned char* array, int array_size);
    int UnsignedIntToSignedInt (int UnsignedInt, int MSB);
    /**
            This function replaces everything left of the MSB with 0 and returns
            the result.
            The first bit is 0 and the bits are numbered from right to left.

            example:
            original=0x...FF MSB=3
            Return Value: 0x00...0F
        **/
    int CutOffDataFromInt (int original, int MSB);
    /**
        Decodes an CanOverEthernet message into null terminated strings.
        Size of Strings
        ID:		7
        CONFIG:	3
        START:	3
        CRC:	3
        DATA:	17

        This function will always return 0
    **/
    int DecodeDataToString (unsigned char* array, wchar_t* ID, wchar_t* CONFIG, wchar_t* START, wchar_t* CRC, wchar_t* DATA);
    /**
     * @brief SendCanMessage
     * @param ID
     * @param Data char array
     * @return
     *   Enqueues a CAN Message with a specified ID and Message.
     *   The Message will be sent as soon as it is processed by TCPThread. If you call
     *   this function while there is no connection, the message will be sent after the
     *   next reconnect.
     *   Data should point to a char array with a size of 8.

     *   Return Values:
     *   0	SUCESS
     *   1	WRONG ID	The ID is out of the range from 0 to 0xFFFFFF

     */
    int SendCanMessage (int ID, char* Data);
    /**
     * @brief ggT returns the greatest common divisor of parameter a and b
     */

    int ggT(int a, int b);
}

