# TcpPing
Simple tool to check connectivity of TCP/IP sockets.

![alt text](https://raw.githubusercontent.com/proxytype/tcpping/main/tcpping.gif)

Use sockets to check connectivity of remote server on specific port, WIN32 use ws2_32.lib with exported functions:

## WIN32 Objects And Structures
[- WSADATA](https://docs.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-wsadata)<br>
[- SOCKET](https://docs.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock)<br>
[- addrinfo](https://docs.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-addrinfoa)<br>

## Win32 Functions
- WSAStartup
- WSACleanup
- getaddrinfo
