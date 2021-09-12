***Tic-Tac Toe Client/Server TCP Game***

How to start the game?

1. Start the server
2. Start the two clients

Which clients starts the connection first, he is the one to play the game first.
After both the clients are connected Server sends f for FIRST and s for SECOND.

SERVER SENDS

const std::string FIRST = "f";  <br />
const std::string SECOND = "s"; <br />
const std::string QUIT = "q";   <br />
const std::string WON = "w";    <br />
const std::string LOST = "l";   <br />
const std::string TIE = "t";   <br />
const std::string ERROR = "e";  <br />
const std::string CONTINUE = "c";  <br />
const std::string TAKEN = "a";  <br />

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
