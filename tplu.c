/*
 * MIT License
 *
 * Copyright (c) 2020 Nullsson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define TP_LINK_PORT 9999
#define MAX_SOCKS 1024
#define STATUS_NONE 1
#define STATUS_CONNECTING 2

enum tp_link_type
{
    TP_LINK_TYPE_UNKNOWN,
    TP_LINK_TYPE_PLUG,
    TP_LINK_TYPE_LAMP,
    TP_LINK_TYPE_MAX,
};

struct tp_link_device
{
    char *IP;
    uint16_t Port;
    enum tp_link_type Type;
};

// TODO(Oskar): Functions & structures library naming in order to not clash for users.

struct connection {
    int Socket;
    int Status;
    time_t ConnectionTime;
    struct sockaddr_in ConnectionAddress;
};

struct connection Connections[MAX_SOCKS];
unsigned int Timeout = 5;
unsigned int SocketNumber = 256;
unsigned long Found = 0;

struct in_addr FoundAddresses[255] = {0};

// TODO(Oskar): This is for the user to make something of. Remove later.
struct tp_link_device TPLinkDevices[10] = {0};

// NOTE(Oskar): Has to be length 4 buffer.
// TODO(Oskar): Add validation.
void serializeUint32(char *buf, uint32_t val)
{
    buf[0] = (val >> 24) & 0xff;
    buf[1] = (val >> 16) & 0xff;
    buf[2] = (val >> 8) & 0xff;
    buf[3] = val & 0xff;
}

void decrypt(char *input, uint16_t length)
{
    uint8_t key = 171;
    uint8_t next_key;
    for (uint16_t i = 0; i < length; i++)
    {
        next_key = input[i];
        input[i] = key ^ input[i];
        key = next_key;
    }
}

void encrypt(char *data, uint16_t length)
{
    uint8_t key = 171;
    for (uint16_t i = 0; i < length + 1; i++)
    {
        data[i] = key ^ data[i];
        key = data[i];
    }
}

void encryptWithHeader(char *out, char *data, uint16_t length)
{
    char serialized[4];
    serializeUint32(serialized, length);
    encrypt(data, length);
    memcpy(out, &serialized, 4);
    memcpy(out + 4, data, length);
}

// clean connection structure
void CleanConnection(struct connection *Connection) {
    if (Connection->Socket) {
        shutdown(Connection->Socket, SHUT_RDWR);
        close(Connection->Socket);
        Connection->Socket = 0;
    }
    Connection->Status = STATUS_NONE;
    Connection->ConnectionTime = 0;
    memset(&(Connection->ConnectionAddress), 0, sizeof(struct sockaddr));
}

int Connect(struct connection *Connection) {
    int Sock; // TODO(Oskar): Naming.
    int Flags; 
    int FlagsOld;

    // create socket
    Sock = socket(AF_INET, SOCK_STREAM, 0);
    if (Sock == -1) {
        perror("Cannot create socket");
        return -1;
    }

    // set non-blocking mode
    FlagsOld = fcntl(Sock, F_GETFL, 0);
    Flags = FlagsOld;
    Flags |= O_NONBLOCK;
    if (fcntl(Sock, F_SETFL, Flags) == -1) {
        perror("Cannot set non-blocking socket");
        return -1;
    }

    // connect to given host
    connect(Sock, (struct sockaddr *)&(Connection->ConnectionAddress), sizeof(struct sockaddr));
    Connection->Socket = Sock;
    Connection->Status = STATUS_CONNECTING;
    Connection->ConnectionTime = time(0);

    return 0;
}

void VerifyConnection(struct connection *Connection) {
    int ConnectionResult;
    // Timeout for connecting socket
    if ((Connection->Status == STATUS_CONNECTING) &&
        ((time(0) - Connection->ConnectionTime) >= Timeout)) {
        CleanConnection(&(*Connection));
        return;
    }

    // connect again, parse errors and log the result
    ConnectionResult = connect(Connection->Socket, (struct sockaddr *)&(Connection->ConnectionAddress),
                     sizeof(struct sockaddr));
    if ((ConnectionResult == -1) && (errno != EALREADY) && (errno != EINPROGRESS))
        CleanConnection(&(*Connection));
    else if (((ConnectionResult == -1) && (errno == EISCONN)) || (ConnectionResult == 0)) {
        // TODO(Oskar): Store Found addresses somewhere in order to ping them later.
        printf("Open %s:%u    \n", inet_ntoa(Connection->ConnectionAddress.sin_addr),
            ntohs(Connection->ConnectionAddress.sin_port));
        FoundAddresses[Found++] = Connection->ConnectionAddress.sin_addr;
        // Found++;
        CleanConnection(&(*Connection));
    }

    return;
}

void _Cleanup(int none) {
    puts("Ok, cleaning up, please wait...\n");
    for (int x = 0; x < MAX_SOCKS; x++) {
        VerifyConnection(&Connections[x]);
        CleanConnection(&Connections[x]);
    }
    puts("Done.\n");
}

uint16_t sockConnect(char *out, const char *ip_add, int port, const char *cmd, uint16_t length)
{

    // struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buf[2048] = {0};
    //  char buffer[2048] = {0};
    //    char buffer[2048] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return 0;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ip_add, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return 0;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return 0;
    }
    send(sock, cmd, length, 0);

    valread = read(sock, buf, 2048);
    close(sock);

    if (valread == 0)
    {
        printf("\nNo bytes read\n");
    }
    else
    {
        // buf + 4 strips 4 byte header
        // valread - 3 leaves 1 byte for terminating null character
        strncpy(out, buf + 4, valread - 3);
    }

    return valread;
}

void SendTPLinkCommand(const char *Command, char *IP, uint16_t Port, char *Response)
{
    uint16_t cmdLen = strlen(Command);

    // Encrypt outgoing message.. Can be moved outside of this loop later on.
    char encrypted[cmdLen + 4];
    char cmdCpy[cmdLen];
    sprintf(cmdCpy, "%s", Command);
    encryptWithHeader(encrypted, cmdCpy, cmdLen);
    // char response[2048] = {0};
    uint16_t length = sockConnect(Response, IP, Port, encrypted, cmdLen + 4);
    if (length > 0)
        decrypt(Response, length - 4);
    else
        return;
    
    return;
}

void CheckTPLinkDeviceInfo(char *IP, uint16_t Port)
{
    const char *cmd = "{\"system\":{\"get_sysinfo\":{}}}";
    char Result[2048] = {0};
    SendTPLinkCommand(cmd, IP, Port, Result);

    printf("Response from server: %s\n", Result);
}

int main(int argc, char *argv[]) {
    unsigned long n_ip, h_ip;           // TODO(Oskar): Naming
    unsigned long current_ip, end_ip;   // TODO(Oskar): Naming
    int verif_sock_time = 500;          // TODO(Oskar): Naming

    // TODO(Oskar): Fixeable.
    n_ip = inet_addr("192.168.0.0");
    h_ip = ntohl(n_ip);
    end_ip = ntohl(inet_addr("192.168.0.255"));
    current_ip = h_ip;

    // TODO(Oskar): Naming
    // clean struct array
    for (int X = 0; X < MAX_SOCKS; X++) 
    {
        CleanConnection(&Connections[X]);
    }

    // TODO(Oskar): Naming / Formatting.
    while (current_ip <= end_ip) {
        for (int x = 0; x < SocketNumber; x++) {
            // if array index is unused, we'll use it
            if (Connections[x].Status == STATUS_NONE) {
                Connections[x].ConnectionAddress.sin_addr.s_addr = htonl(current_ip);
                Connections[x].ConnectionAddress.sin_port = htons((unsigned short)TP_LINK_PORT);
                Connections[x].ConnectionAddress.sin_family = AF_INET;
                if (Connect(&Connections[x]) == -1) {
                    fprintf(stderr,
                            "Oops, try with `-s < %u'. Sleeping 10secs.\n",
                            SocketNumber);
                    sleep(10);
                    break;
                }

                fflush(stdout);
                
                current_ip++;
            }
            if (current_ip > end_ip) break;
        }

        // prevent 100% cpu usage
        usleep(verif_sock_time * 1000);
        for (int x = 0; x < SocketNumber; x++)
        {
            VerifyConnection(&Connections[x]);
        }
    }

    putchar('\n');

    // TODO(Oskar): Might be required.
    sleep(Timeout);
    for (int x = 0; x < SocketNumber; x++) 
    {
        VerifyConnection(&Connections[x]);
        CleanConnection(&Connections[x]);
    }

    _Cleanup(0);

    printf("Open %lu [Done]\n", Found);

    printf("The found ones are: \n");
    for (int Index = 0; Index < Found; ++Index)
    {
        printf("Open %s:9999    \n", inet_ntoa(FoundAddresses[Index]));
        CheckTPLinkDeviceInfo(inet_ntoa(FoundAddresses[Index]), 9999);
    }

    return 0;
}