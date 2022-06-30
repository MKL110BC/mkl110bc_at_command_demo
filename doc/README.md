# INTRODUCTION	(MK110-BC)

​	This project is currently a demo, which has completed some lorawan configuration items, network access and regular data sending functions, among which the sent data is temperature and humidity.

​	In data transmission, the transmission of lorawan module can be in string format or hexadecimal format. But in the AT command, the data format sent by the serial port can only be in the string format. If the data sent by lorawan is in string format, the data sent by the serial port is the same as the data sent by lorawan. If it is in hexadecimal format, one byte of lorawan data needs to send two characters from the serial port. The specific demonstration is as follows:

 

| Format/type | Lorawan                         | Uart(send string type) |
| ----------- | ------------------------------- | ---------------------- |
| String      | Data: 0x77 0x6F 0x77 0x6F(wowo) | Send: wowo             |
| Hex         | Data: 0x23 0x25                 | Send: 2325             |

 

Example of temperature and humidity data sent periodically:

| type                      | data                      |
| ------------------------- | ------------------------- |
| temperature               | 2731(27.31℃)              |
| humidity                  | 5587(55.87%)              |
| Uart to send(string type) | Send: 27315587            |
| Lorawan to send           | Data: 0x27 0x31 0x55 0x87 |

 

