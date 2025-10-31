#pragma once
#include "Game.h"

class ConnectFour : public Game
{
    public:
        ConnectFour();
        ~ConnectFour();

        void setUpBoard() override;
        Player* checkForWinner() override;
        bool checkForDraw() override;
        std::string initialStateString() override;
        std::string stateString() override;
        void setStateString(const std::string &s) override;
        bool actionForEmptyHolder(BitHolder &holder) override;
        bool canBitMoveFrom(Bit &bit, BitHolder &src) override;
        bool canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
        void stopGame() override;

        void updateAI() override;
        bool gameHasAI() override { return true; }
        Grid* getGrid() override { return _grid; }
        bool UpdatePeiceDropXY( int x, int &y);
        void UpdatePlayerStates(int x, int y);

    private:
        Bit * PieceForPlayer(const int playerNumber);
        int NegaMax(int playerNumber, int alpha, int beta, int depth);
        int NegaMaxCheckForWinner();
        int EvaluationFunction(int currentPlayer);

        Grid* _grid;

        //represented as 48 bits
        u_int64_t p1State;
        u_int64_t p2State;

        bool gameOver;
};