/***********************************************************************
 * Program:
 *    Lab RPSPserver, RPSP Rock/Paper/Scissors Protocol - Server Code
 *    Brother Jones, CS 460
 * Author:
 *    Klevin Doda
 * Summary:
 *    The server that interacts to play the Tic Tac Toe game.
 ************************************************************************/


/*
 ** server.c -- a stream socket server demo
 **
 ** This is a modified of code from http://beej.us/guide/bgnet/
 ** The code was modified to take out the "fork" to start a new process and
 ** the signal handler.
 **
 ** It was modified to partly conform to BYU-Idaho style.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string>
#include <cassert>
#include <iostream>

using namespace std;

const int SIZE = 3;
const std::string FIRST    = "f";
const std::string SECOND   = "s";
const std::string QUIT     = "q";
const std::string WON      = "w";
const std::string LOST     = "l";
const std::string TIE      = "t";
const std::string ERROR    = "e";
const std::string CONTINUE = "c";
const std::string TAKEN    = "a";

#define BACKLOG 10   // how many pending connections queue will hold

#define MAXDATASIZE 100 // max number of bytes we can get at once

//references http://beej.us/guide/bgnet/

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/*********************************************************************
 * Send the data through socket id
 *********************************************************************/
bool send(int id, std::string data)
{
    bool succeeded = (send(id, data.c_str() , data.size(), 0) != -1);
    
    if (!succeeded)
        perror("send");
    
    return succeeded;
}


/*********************************************************************
 * Wait for data on the socket id and return it as a string
 *********************************************************************/
std::string receive(int id)
{
    int numbytes;
    char buf[MAXDATASIZE];
    if ((numbytes = recv(id, buf, MAXDATASIZE-1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }
    
    buf[numbytes] = '\0';
    return std::string(buf);
}

/*********************************************************************
 * Is the client choice valid 
 *********************************************************************/
bool isValid(std::string client1Choice) 
{
    return true;
}

/*********************************************************************
 * Validate the number of arguments given from CMD
 *********************************************************************/
void validateArguments(int argc)
{
    if (argc == 1)
    {
        printf("Please provide a port number.\n");
        exit(EXIT_FAILURE);
    }
    else if (argc > 2)
    {
        printf("Please provide just the port number.\n");
        printf("Other arugments are not useful.\n");
        exit(EXIT_FAILURE);
    }
}

void print(int board[][SIZE], int SIZE)
{
    for(int i = 0;i < SIZE; ++i)
    {
        for(int j = 0;j < SIZE; ++j)
        {
            cout << " " << board[i][j] << " " << (j == SIZE - 1 ? "" : "|");
        }

        if (i != SIZE - 1)
        {
            cout << endl << "---+---+---" << endl;
        }
    }

    cout << endl << endl ;
}


std::string gameState(int board[][SIZE], int SIZE, int clientId, int clientChoice)
{
    //for debugging board state in abkcend
    // print(board, SIZE);

    int col = clientChoice % 3;
    int row = clientChoice / 3;
    assert(row < SIZE && col < SIZE);

    if (board[row][col] != 0)
        return TAKEN;
    
    board[row][col] = clientId;
    // check row
    for (int i = 0; i < SIZE; ++i)
    {
        if (board[i][0] == board[i][1] 
         && board[i][0] == board[i][2]
         && board[i][0] == clientId)
            return WON;
    }

    // check col
    for (int i = 0; i < SIZE; ++i)
    {
        if (board[0][i] == board[1][i] 
         && board[0][i] == board[2][i]
         && board[0][i] == clientId)
            return WON;
    }

    // check diags
    if ((board[0][0] == board[1][1] 
     && board[0][0] == board[2][2]
     && board[0][0] == clientId)
     ||(board[0][2] == board[1][1] 
     && board[0][2] == board[2][0]
     && board[0][2] == clientId))
        return WON;

    return CONTINUE;
}

void initialize(int board[][SIZE], int SIZE)
{
    for (int i = 0;i < SIZE;++i)
        for (int j = 0;j < SIZE;++j)
            board[i][j] = 0;
}
    
int main(int argc , char *argv[])
{
    validateArguments(argc);
    
    int board[SIZE][SIZE];
    int sockfd;      // listen on sock_fd
    int new_fd;      // new connection on new_fd
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    //struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        
        break;
    }
    
    freeaddrinfo(servinfo); // all done with this structure
    
    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    
    printf("server: waiting for connections...\n");
    
    
    int client1;
    int client2;
    
    // listen forever
    listen:
    int connections = 0;

    do 
    {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }
        
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);
        
        connections++;
            
        if (connections == 1)
            client1 = new_fd;
        else
            client2 = new_fd;
    
    }while (connections < 2);
    
     
    send(client1, FIRST);
    send(client2, SECOND);

    std::string client1Choice;
    std::string client2Choice;
    
    
    initialize(board, SIZE);
    int count = 1;

    // PLAY GAME
    while(1)
    {
        client1Choice = receive(client1);
        //if it is a q send it to the other client don't wait for his input
        if (client1Choice == QUIT)
        {
            send(client2, QUIT);
            break;
        }
        
        if (!isValid(client1Choice))
        {
            send(client1, ERROR);
            send(client2, ERROR);
            continue;
        }
         
        std::string client1Result = gameState(board, SIZE, client1, atoi (client1Choice.c_str()));

        if (client1Result == WON)
        {
            send(client1, client1Choice + " " + WON);
            send(client2, client1Choice + " " + LOST);
            break;
        } 
        else if (client1Result == CONTINUE && count == SIZE * SIZE)
        {
            send(client1, client1Choice + " " + TIE);
            send(client2, client1Choice + " " + TIE);
            break;
        }
        else 
        {
            send(client2, client1Choice + " " + CONTINUE);
        }
        count++;    

        client2Choice = receive(client2);
        //if it is a q send it to the other client don't wait for his input
        if (client2Choice == QUIT)
        {
            send(client1, QUIT);
            break;
        }
        
        if (!isValid(client2Choice))
        {
            send(client1, ERROR);
            send(client2, ERROR);
            continue;
        }

        std::string client2Result = gameState(board, SIZE, client2, atoi (client2Choice.c_str()));

        if (client2Result == WON)
        {
            send(client1, client2Choice + " " + LOST);
            send(client2, client2Choice + " " + WON);
            break;
        }
        else 
        {
            send(client1, client2Choice + " " + CONTINUE);
        }
        count++;
    }
    
    printf("server: closed connection\n");
    close(client1);
    close(client2);

    goto listen;

    return 0;
}

