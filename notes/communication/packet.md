# Consideration of packet structures


## HEADER

* preamble [00110011]
* address start tag [EE]
* chain of addresses
    * Source Device
    * Destination Device
* address end tag [FF]


[1       ][2]                     ][ n                            ][2                    ] == 5+n bytes for header
[00110011][START OF ADDRESS TAG|EE][chain of single byte addresses][END OF ADDRESS TAG|FF]

- How many devices do I anticipate to be on this bus?
    - 1 byte:
        - 8 bits = 256 + 0
        - 7 bits = 128 + 1
        - 6 bits = 64  + 2
        - 5 bits = 32  + 3
        - 4 bits = 16  + 4
    - So if we use a 4 bit nibble for the address, that is 16 combinations.
        - 0000 - special case
        - 1111 - special case
        - 0001 through 1110 = address bits = 14 addresses (this should be plenty for a single bus control node)

- So... change of addresses...
    - [number of addresses in chain]
    - [addr1][addr2][addr3][addr4]*n 
    

## PAYLOAD

* number of bytes to transfer
* actual bytes.
* End of packet.
* CRC for packet (not including the CRC itself)
