Code Implementation:


- the board is build the same way as tictactoe, all the code is referenced from the TicTacToe.cpp file in terms of initalizing the board.
- Changed up actionforplayer function where now after placing down the block, the game checks if there are any empty space below the block, and if there is, do the falling animation into the block. 
- all player states is tracked thourgh a bitmap now, where I have two u64int representing the position for each player
- Added a gameover bool to the ConnectFour class, to prevent actions after the game is over
- all checking functions will be using these two player state ints, for potential faster performance (especially the checkforwin/checkforwinner functions)
- CheckForWinner function checks the winner thorugh bit-shifts and bit-and operations, by checking in the horizontal, vertical, positive diagnal, and negative diagnal direction to see if there is a pair, and then checking if two pairs are next to each other.
- CheckDraw checks the board state by taking the bit-xor of player states, and if the number is equal to the theoretical full board, return true
- Adjust the space string functions to the 6 * 7 boards
- In the set state string function, also update the player state ints
- Keep the same NegaMax structure as TicTacToe, added a depth of 4 initially and after more testing decided on 10, implemented alpha beta pruning the same as TicTacToe
- NegaMax updates the player state ints directly through bit-shifts, and return them to its original state after function call
- uses a separate check winner and check draw function that does not affect gameOver bool
- Once the Code reaches the depth indicated, it will then call the evaluation function
- evaluation function follows the logic of:    

    check every 4 in a row
    3 empty + 1 of mine = 1
    2 empty + 2 of mine = 10
    3 empty + 1 empty = 100

    caculate for both p1 and p2, and then take the differnce between them (current player - opponent) and return
