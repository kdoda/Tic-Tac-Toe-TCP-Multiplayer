/***********************************************************************
 * Program:
 *    Lab RPSPclient, RPSP Rock/Paper/Scissors Protocol - Client Code
 *    Brother Jones, CS 460
 * Author:
 *    Klevin Doda
 * Summary:
 *    The client that interacts to play the Tic Tac Toe game.
 ************************************************************************/

/*
 ** client.c -- a stream socket client demo
 **
 ** This is code from http://beej.us/guide/bgnet/
 ** It was modified to partly conform to BYU-Idaho style.
 */

#include <iostream>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <sstream>

using namespace std;

bool quitGame = false;
const int SIZE = 3;
char board[SIZE][SIZE];
char letter;
const std::string FIRST    = "f";
const std::string SECOND   = "s";
const std::string QUIT     = "q";
const std::string WON      = "w";
const std::string LOST     = "l";
const std::string TIE      = "t";
const std::string ERROR    = "e";
const std::string CONTINUE = "c";
const std::string TAKEN    = "a";

#define MAXDATASIZE 100 // max number of bytes we can get at once

int wins = 0;
int losts = 0;
int ties = 0;

// references: http://beej.us/guide/bgnet/

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
    return string(buf);
}


void initialize(char board[][SIZE], int SIZE)
{
    cout << "Pick a position from the board: " << endl;

    for (int i = 0;i < SIZE;++i)
    {
        for (int j = 0;j < SIZE;++j)
        {
            board[i][j] = ' ';
        }
    }

    cout << endl;       
}


void print(char board[][SIZE], int SIZE)
{
    char pos = '0';
    for(int i = 0;i < SIZE; ++i)
    {
        for(int j = 0;j < SIZE; ++j)
        {
            cout << " " << (board[i][j] == ' ' ? pos : board[i][j])  << " " << (j == SIZE - 1 ? "" : "|");
            pos++;
        }

        if (i != SIZE - 1)
        {
            cout << endl << "---+---+---" << endl;
        }
    }

    cout << endl << endl ;
}

bool playAgain() 
{
    char input;
    while(true)
    {
        cout << "Play again y or n: ";
        cin >> input;
        if (input == 'y')
            return true;
        else if(input == 'n')
            return false;
    }
}

/*********************************************************************
 * Validate the number of arguments given from CMD
 *********************************************************************/
void validateArguments(int argc)
{
    if (argc == 1)
    {
        printf("Please enter the service and the port number.\n");
        exit(EXIT_FAILURE);
    }
    else if (argc == 2)
    {
        printf("Please enter the port number.\n");
        exit(EXIT_FAILURE);
    }
    else if (argc > 3)
    {
        printf("Please provide just service and the port number.\n");
        printf("Other arugments are not useful.\n");
        exit(EXIT_FAILURE);
    }
}

bool isValid(int input)
{
    return 0 <= input && input <= 8;
}

bool player2Input(int sockfd) 
{
    bool shouldContinue = false;
    string respond = receive(sockfd);

    std::stringstream ss(respond);
    string player2Choice;
    string gameResult;
    ss >> player2Choice >> gameResult;
    
    if (player2Choice == QUIT)
    {
        cout << "Player 2 quit the game!" << endl << "Game over!" << endl;
        quitGame = true;
    } 
    else 
    {
        int player2ChoiceInt = atoi(player2Choice.c_str());
        int i = player2ChoiceInt / 3;
        int j = player2ChoiceInt % 3;
        board[i][j] = (letter == 'x') ? 'o' : 'x';
            
        if (gameResult == TIE)
        {
            cout << "It's a TIE!" << endl;
            ties++;
        }
        else if (gameResult == WON)
        {
            cout << "You Won!" << endl << endl;
            wins++;
        }
        else if (gameResult == LOST)
        {
            cout << "You Lost! Let's play another one" << endl <<  endl;
            losts++;
        }
        else if (gameResult == CONTINUE)
        {
            shouldContinue = true;
        }
    }
    
    return shouldContinue;
}

void getLetter()
{
    while(true)
    {
        cout << "Pick x or o: ";
        cin >> letter;
        if (letter == 'x' || letter == 'o')
            return;
    }
}


int main(int argc, char *argv[])
{
    validateArguments(argc);

    // playAgain forever
    playAgain:

    initialize(board, SIZE);

    int sockfd;
    int numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }
        
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("client: connect");
            close(sockfd);
            continue;
        }
        
        break;
    }
    
    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
    
    freeaddrinfo(servinfo); // all done with this structure
    
    cout << "#############################" << endl 
         << "You are now playing Tic Tac Toe!" << endl
         << "#############################" << endl;
    cout << "Waiting for player 2." << endl;


    string respond = receive(sockfd);

    cout << "Player 2 connected!" << endl;
    getLetter();

    if (respond == FIRST)
    {
        cout << "You are going first!" << endl;
        cout << "Pick a position!" << endl;
    } 
    else
    {
        cout << "You are going second!" << endl;
        cout << "Waiting for player 2 choice!" << endl;
        if (!player2Input(sockfd))
        {
            close(sockfd);
            return 0;
        }
    }

    // PLAY GAME
    string userInput;
    bool shouldContinue = false;
    do 
    {
        shouldContinue = false;
        cout << "Game State! You are: " << letter << endl << endl; 
        print(board, SIZE);

        while(true)
        {
            cout << ">> ";
            cin >> userInput;
            if ((userInput[0] >= '0' && userInput[0] <= '8') || userInput[0] == 'q')
                break;
            else 
                cout << "Please enter a valid input." << endl;
        }   

        int userInputInt = atoi(userInput.c_str());
        int i = userInputInt / 3;
        int j = userInputInt % 3;

        if (userInput == QUIT)
        {
            send(sockfd, QUIT);
            cout << "Game over!" << endl;
            quitGame = true;
        }
        else if (!isValid(userInputInt))
        {
            cout << "Please enter a valid position" << endl;
            shouldContinue = true;
        }
        else if (board[i][j] != ' ')
        {
            cout << "Position is taken! Please choose another one." << endl;
            shouldContinue = true;
        }
        else
        {
            cout << "You picked " << userInput << endl;
            
            board[i][j] = letter;
            cout << "Game State! You are: " << letter << endl;
            print(board, SIZE);

            cout << "Waiting for player 2." << endl << endl;

            send(sockfd, userInput);
            shouldContinue = player2Input(sockfd);
        }

    } while(shouldContinue);

    close(sockfd);
    
    cout << "Game Statistics: " << ties << " ties, " << wins << " wins, " << losts << " losts." << endl << endl; 

    if (quitGame)
        return 0;
    if (!playAgain())
        return 0;

    goto playAgain;

    return 0;
}

