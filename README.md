I did two projects 

The first Creating a TCP client and server connection in C#. I felt that was not very big so I did also a
Tic-Tac-Toe game in C++.

Tic-Tac Toe game

How to start the game?

1. Start the server
2. Start the two clients

Which clients starts the connection first, he is the one to play the game first.
After both the clients are connected Server sends f for FIRST and s for SECOND.

SERVER SENDS

const std::string FIRST = "f"; 
const std::string SECOND = "s"; 
const std::string QUIT = "q"; 
const std::string WON = "w"; 
const std::string LOST = "l"; 
const std::string TIE = "t"; 
const std::string ERROR = "e"; 
const std::string CONTINUE = "c"; 
const std::string TAKEN = "a";

CLIENTS SENDS

const std::string QUIT = "q";
Position 0-8 AS A STRING.

Every time the client sends a position, the position is send to the other client to update it’s board.
The server sends also the state with the position, for example “1 c”. C for CONTINUE.

Every time the server receives a position it validates the board. If the board state is a 
TIE, WON, LOST, it sends the state and the position to both the clients. 

The client checks if the state send by the server is any of the above and terminates the game round if 
Yes, and displays statistics, then prompts the user to play again, if state is CONTINUE, the game goes on.

The client can QUIT the game, by entering q. 

Server will listen forever even if clients disconnect.

If an error occurs the server sends the ERROR code to both clients.
