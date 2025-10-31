#include "ConnectFour.h"


ConnectFour::ConnectFour()
{
    _grid = new Grid(7, 6);
}

ConnectFour::~ConnectFour()
{
    delete _grid;
}

Bit* ConnectFour::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    //assumuing 2 players, 0 and 1
    bit->LoadTextureFromFile(playerNumber ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber));
    return bit;
}

void ConnectFour::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "square.png");

    p1State = 0;
    p2State = 0;
    
    gameOver = false;

    startGame();
}


bool ConnectFour::actionForEmptyHolder(BitHolder &holder)
{
    if (holder.bit() || gameOver) {
        return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    if (bit) {
        bit->setPosition(holder.getPosition());
        //print out the positions of the bit being placed
        //std::cout << "Placing piece for player " << getCurrentPlayer()->playerNumber() << " at position (" << holder.getPosition().x << ", " << holder.getPosition().y << ")" << std::endl;
        holder.setBit(bit);
        int x = (holder.getPosition().x - 40) / 80;
        int y = (holder.getPosition().y - 40) / 80;
        UpdatePeiceDropXY(x, y);
        _grid->getSquare(x, y)->dropBitAtPoint(bit, holder.getPosition());
        //std::cout << "Piece dropped at (" << x << ", " << y << ")" << std::endl;
        UpdatePlayerStates(x, y);

        //print out the bit states
        //std::cout << x << ", " << y << " placed." << std::endl;
        //std::cout << "Player 1 State: " << std::bitset<48>(p1State) << std::endl;
        //std::cout << "Player 2 State: " << std::bitset<48>(p2State) << std::endl;
        endTurn();
        return true;
    }   
    return false;
}

bool ConnectFour::UpdatePeiceDropXY(int x, int &y)
{

    uint64_t fullBoard = p1State | p2State;
    fullBoard >>= (7 - x);
    for (int col = 5; col >= 0; col--)
    {
        if (!(fullBoard & 1))
        {
            y = col;
            return true;
        }

        fullBoard >>= 8;
    }
    return false;


}

void ConnectFour::UpdatePlayerStates(int x, int y)
{
    //y * 8 not 7 to offset the empty column
    int bitPosition = 47 - (8 * y + x);
    if (getCurrentPlayer()->playerNumber()) p2State |= (1ULL << bitPosition);
    else p1State |= (1ULL << bitPosition);
}

bool ConnectFour::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool ConnectFour::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void ConnectFour::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

//
// helper function for the winner check


Player* ConnectFour::checkForWinner()
{

    //check player 1
    //check player 2

    uint64_t match;
    //player 2
        //shift by a value to see if there are 2 in a rows

        //shift by double that value to see if there are two 2 in a rows, which thus mean 4 in a row

        //horizontal: 1
        //vertical: 8
        //diagonal postive: 7
        //diagnal negative: 9

        gameOver = true;
        match = p2State & (p2State >> 1);
        if (match & (match >> 2)) return getPlayerAt(1);
        match = p2State & (p2State >> 8);
        if (match & (match >> 16)) return getPlayerAt(1);
        match = p2State & (p2State >> 7);
        if (match & (match >> 14)) return getPlayerAt(1);
        match = p2State & (p2State >> 9);
        if (match & (match >> 18)) return getPlayerAt(1);

    //player1
        match = p1State & (p1State >> 1);
        if (match & (match >> 2)) return getPlayerAt(0);
        match = p1State & (p1State >> 8);
        if (match & (match >> 16)) return getPlayerAt(0);
        match = p1State & (p1State >> 7);
        if (match & (match >> 14)) return getPlayerAt(0);
        match = p1State & (p1State >> 9);
        if (match & (match >> 18)) return getPlayerAt(0);

    gameOver = false;
    return nullptr;
}

bool ConnectFour::checkForDraw()
{

    uint64_t FULL = 0x0000FEFEFEFEFEFEULL;
    //std::cout << "State: " << std::bitset<42>(p1State ^ p2State) << std::endl;

    if ((p1State ^ p2State) == FULL)
    {
        std::cout << p1State << " " << p2State <<std::endl;
        gameOver = true;
        return true;
    }
    return false;
}

//
// state strings
//
std::string ConnectFour::initialStateString()
{
    return "000000000000000000000000000000000000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string ConnectFour::stateString()
{
    std::string s = "000000000000000000000000000000000000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 7 + x] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void ConnectFour::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y*7 + x;
        int playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit( PieceForPlayer(playerNumber-1) );
            int bitPosition = 47 - (8 * y + x);
            if (playerNumber - 1) p2State |= (1ULL << bitPosition);
            else p1State |= (1ULL << bitPosition);
        } else {
            square->setBit( nullptr );
        }

    });
}


//
// this is the function that will be called by the AI
//

static int s = 0;
void ConnectFour::updateAI() 
{

    if (gameOver) return;
    // int bestVal = -1000;
    // BitHolder* bestMove = nullptr;
    // std::string state = stateString();

    // // Traverse all cells, evaluate minimax function for all empty cells
    // _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
    //     int index = y * 3 + x;
    //     // Check if cell is empty
    //     if (state[index] == '0') {
    //         // Make the move
    //         state[index] = '2';
    //         int moveVal = -NegaMax(state, 0, HUMAN_PLAYER);
    //         // Undo the move
    //         state[index] = '0';
    //         // If the value of the current move is more than the best value, update best
    //         if (moveVal > bestVal) {
    //             bestMove = square;
    //             bestVal = moveVal;
    //         }
    //     }
    // });


    // // Make the best move
    // if(bestMove) {
    //     if (actionForEmptyHolder(*bestMove)) {
    //     }
    // }

    uint64_t *currentPlayerState;
    int currentPlayerNumber = getCurrentPlayer()->playerNumber();
    if (!currentPlayerNumber) currentPlayerState = &p1State;
    else currentPlayerState = &p2State;

    int bestX = -1;
    int curBest = -2147483647;
    for (int i = 0; i < 7; i++) {
        int y = 0;
        if (UpdatePeiceDropXY(i,y)) {
            int bitPosition = 47 - (8 * y + i);
            *currentPlayerState |= (1ULL << bitPosition);
            int score = -NegaMax((currentPlayerNumber == 1)? 1 : 2, -2147483647, 2147483647, 1);
            if (score > curBest) {
                curBest = score;
                bestX = i;
            }

            //////IM HERE RN, TURNNING THE BYTE BACK TO 0
            *currentPlayerState &= ~(1ULL << bitPosition);
        }
    }
    actionForEmptyHolder(*_grid->getSquare(bestX, 0));
    //std::cout << s << std::endl;
}

int ConnectFour::NegaMaxCheckForWinner()
{

    //check player 1
    //check player 2

    uint64_t match;
    //player 2
        match = p2State & (p2State >> 1);
        if (match & (match >> 2)) return 2;
        match = p2State & (p2State >> 8);
        if (match & (match >> 16)) return 2;
        match = p2State & (p2State >> 7);
        if (match & (match >> 14)) return 2;
        match = p2State & (p2State >> 9);
        if (match & (match >> 18)) return 2;

    //player1
        match = p1State & (p1State >> 1);
        if (match & (match >> 2)) return 1;
        match = p1State & (p1State >> 8);
        if (match & (match >> 16)) return 1;
        match = p1State & (p1State >> 7);
        if (match & (match >> 14)) return 1;
        match = p1State & (p1State >> 9);
        if (match & (match >> 18)) return 1;

        return 0;
}

int ConnectFour::NegaMax(int playerNumber, int alpha, int beta, int depth)
{
    //check draw
    if ((p1State ^ p2State) == 0x0000FEFEFEFEFEFEULL) return 0;
    //check winner
    int winner = NegaMaxCheckForWinner();
    if (winner == (playerNumber)) return 2147483646;
    if (winner == ((playerNumber % 2) + 1)) return -2147483646;
    //check for depth// 6 for now
    if (depth == 10) return EvaluationFunction(playerNumber);

    int curBest = -2147483647;
    uint64_t *currentPlayerState;
    if (playerNumber == 1) currentPlayerState = &p1State;
    else currentPlayerState = &p2State;
    for (int i = 0; i < 7; i++) {
        int y = 0;
        if (UpdatePeiceDropXY(i,y)) {
            int bitPosition = 47 - (8 * y + i);
            *currentPlayerState |= (1ULL << bitPosition);
            int score = -NegaMax((playerNumber == 1)? 2 : 1, -beta, -alpha, depth + 1);
            if (score > curBest) {
                curBest = score;
            }

            //////IM HERE RN, TURNNING THE BYTE BACK TO 0
            *currentPlayerState &= ~(1ULL << bitPosition);
            alpha = std::max(alpha, curBest);
            if (alpha >= beta) break;
        }
    }
    return curBest;
}

int ConnectFour::EvaluationFunction(int currentPlayer)
{
    //s++;

    //evaluate row

    //check every 4 in a row
    //3 empty + 1 of mine = 1
    //2 empty + 2 of mine = 10
    //3 empty + 1 empty = 100

    int scoring[5] = {0,1,10,100, 10000000};

    int p1Score = 0;
    int p2Score = 0;

    uint64_t p1StateCopy = p1State >> 1;
    uint64_t p2StateCopy = p2State >> 1;
    uint64_t rowWin = 15UL;
    for (int i = 0; i < 6; i++ )
    {
        //checking all 4 row combos
        for (int j = 0; j < 4; j++)
        {
            //update p1 score
            if (!(p2StateCopy & rowWin))
            {
                int count = 0;
                for (int k = 0; k < 4; k++)
                {
                    if (p1StateCopy >> k & 1) count++;
                }
               p1Score += scoring[count];
            }

            //update p2 score
            if (!(p1StateCopy & rowWin))
            {
                int count = 0;
                for (int k = 0; k < 4; k++)
                {
                    if (p2StateCopy >> k & 1) count++;
                }
                if (count) p2Score += scoring[count];
            }
            
            //indicate next combo
            p1StateCopy >>= 1;
            p2StateCopy >>= 1;
        }

        //to shift to the next column
        p1StateCopy >>= 4;
        p2StateCopy >>= 4;
    }
    //std:: cout << "p1 state: " << std::bitset<48>(p1State) << " p2 state: " << std::bitset<48>(p2State) << std::endl;
    //std::cout << p1Score << std::endl;


    

    //column evaluation

    //counts the combos hit for each column, -1 to indicate and enemy has been here
    int p1ColumnCombo[7];
    int p2ColumnCombo[7];

    p1StateCopy = p1State;
    p2StateCopy = p2State;
    uint64_t p1Cur;
    uint64_t p2Cur;


    //looping thorugh each combo
    for (int i = 0; i < 3; i++)
    {
        for (int& x : p1ColumnCombo) x = 0;
        for (int& x : p2ColumnCombo) x = 0;

        //comparing with the 3 rows above it
        for (int j = 0; j < 4; j++)
        {
            p1Cur = (p1StateCopy >> (8 * j));
            p2Cur = (p2StateCopy >> (8 * j));
            for (int k = 1; k < 8; k++)
            {
                //update p1 combos

                //if the combo is still active
                if (p1ColumnCombo[k-1] > -1)
                {
                    //if theres a same block
                   if ((p1Cur >> k) & 1) p1ColumnCombo[k-1]++;
                   //if an enemy block blocks you
                   else if ((p2Cur >> k) & 1) p1ColumnCombo[k-1] = -1;
                }

                //update p2 combos
                if (p2ColumnCombo[k-1] > -1)
                {
                   if ((p2Cur >> k) & 1) p2ColumnCombo[k-1]++;
                   else if ((p1Cur >> k) & 1) p2ColumnCombo[k-1] = -1;
                }
            }
        }

        for (int x : p1ColumnCombo) if (x > 0) p1Score += scoring[x];
        for (int x : p2ColumnCombo) if (x > 0) p2Score += scoring[x];

        p1StateCopy >>= 8;
        p2StateCopy >>= 8;

    }



    //diaginals
    //same logic
    //hard coding the rest

    int posDiagnalShifts[] = {4, 5, 6, 7, 12, 13, 14, 15, 20, 21, 22, 23};
    int negDiagnalShifts[] = {1, 2, 3, 4, 9, 10, 11, 12, 17, 18, 19, 20};


    //check the repeating diagnals (ones that are not the line)

    for (int i: posDiagnalShifts)
    {
        p1Cur = p1State >> i;
        p2Cur = p2State >> i;
        if (!((p2Cur & 1)|| ((p2Cur >> 7) & 1) || ((p2Cur >> 14) & 1) || ((p2Cur >> 21) & 1)))
        {
            p1Score += scoring[(p1Cur & 1) + ((p1Cur >> 7) & 1) + ((p1Cur >> 14) & 1) + ((p1Cur >> 21) & 1)];
        }

        if (!((p1Cur & 1)|| ((p1Cur >> 7) & 1) || ((p1Cur >> 14) & 1) || ((p1Cur >> 21) & 1)))
        {
            p2Score += scoring[(p2Cur & 1) + ((p2Cur >> 7) & 1) + ((p2Cur >> 14) & 1) + ((p2Cur >> 21) & 1)];
        }
        
    }

    for (int i: negDiagnalShifts)
    {
        p1Cur = p1State >> i;
        p2Cur = p2State >> i;
        if (!((p2Cur & 1)|| ((p2Cur >> 9) & 1) || ((p2Cur >> 18) & 1) || ((p2Cur >> 27) & 1)))
        {
            p1Score += scoring[(p1Cur & 1) + ((p1Cur >> 9) & 1) + ((p1Cur >> 18) & 1) + ((p1Cur >> 27) & 1)];
        }

        if (!((p1Cur & 1)|| ((p1Cur >> 9) & 1) || ((p1Cur >> 18) & 1) || ((p1Cur >> 27) & 1)))
        {
            p2Score += scoring[(p2Cur & 1) + ((p2Cur >> 9) & 1) + ((p2Cur >> 18) & 1) + ((p2Cur >> 27) & 1)];
        }
        
    }

    if (currentPlayer == 1)  return p1Score - p2Score;
    else   return p2Score - p1Score;
}
