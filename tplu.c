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
        Found++;
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
    return 0;
}