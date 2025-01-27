#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H
#include "ArrayList.h"
#include "chessboard.h"
#include "search_engine.h"
#include <climits>
#include <cfloat> 
#include <cmath>

#define tiny 1e-6

class MCTSNode{
public:
    double value;
    double travelNum;
    int subNum;
    MCTSNode *subMCTS;
    MCTSNode *parent;
    ArrayList moveList;
    MCTSNode();
};

class MCTS{
public:
    MCTS(Chessboard gameBoard);
    int search(int currentPlayer);
private:
    Chessboard gameBoard;
    Chessboard oriBoard;
    void expand(MCTSNode *currentNode,int currentPlayer);
    int rollout(MCTSNode *currentNode,int currentPlayer,int depth);
    void Backpropagation(MCTSNode *currentNode,int rolloutValue);
};

#endif