//
// Created by Blue Sky on 4/5/20.
//

#include "search_engine.h"
extern FILE *xp;


int get_index(){
    static int index;
    return index++;
}

int Chessboard::alphaBeta(int depth, int alpha, int beta, int minimaxPlayer)
{
    if (depth <= 0 || judge())
    {
        int value = Evaluate_test(minimaxPlayer);
        return value;
    }
    int val,origin;
    ArrayList move_array;
    Move_Generate(move_array,minimaxPlayer);
    singleMove x;
    ArrayList store;
    while(move_array.size())
    {
        move_array.pop(x);
        origin = makeMove(x,minimaxPlayer);
        val = -alphaBeta(depth - 1, -beta, -alpha, -minimaxPlayer);
        unMakeMove(x,minimaxPlayer,origin);
        if(val>=beta){
            return beta;
        }
        if(val>alpha){
            alpha=val;
        }
    }
    return alpha;
}

singleMove Chessboard::alphaBetaMove(int depth, int alpha, int beta, int minimaxPlayer)
{
    singleMove best_move;
    int flag, val,origin;
    ArrayList move_array ;
    Move_Generate(move_array,minimaxPlayer);
    singleMove x;
    move_array.pop(x);
    best_move = x;
    while(move_array.size())
    {
        origin = makeMove(x,minimaxPlayer);
        val = -alphaBeta(depth - 1, -beta, -alpha, -minimaxPlayer);
        unMakeMove(x,minimaxPlayer,origin);
        move_array.pop(x);
        cout<<val<<endl;
        if(val>alpha){
            alpha=val;
            best_move=x;
        }
    }
    cout<<-alpha<<endl;
    return best_move;
}
int  Chessboard::alphaBetaNullMove(int depth, int alpha, int beta, int minimaxPlayer) {
    if (depth <= 0 || judge())
    {
        int value = Evaluate_test(minimaxPlayer);
        return value;
    }
    int val, origin;
    ArrayList move_array;
    Move_Generate(move_array, -minimaxPlayer);
    singleMove x;
    ArrayList store;
    if (move_array.size())
    {
        move_array.pop(x);
        origin = makeMove(x, -minimaxPlayer);
        val = -alphaBeta(depth - 1 - 2, -beta, -beta + 1, minimaxPlayer);
        unMakeMove(x, -minimaxPlayer, origin);
         if (val >= beta)
            return beta;
    }
    Move_Generate(move_array, minimaxPlayer);
    while (move_array.size())
    {
        move_array.pop(x);
        origin = makeMove(x, minimaxPlayer);
        val = -alphaBeta(depth - 1, -beta, -alpha, -minimaxPlayer);
        unMakeMove(x, minimaxPlayer, origin);
        if (val >= beta)
            return beta;
        if (val > alpha)
            alpha = val;
    }
    return alpha;
}
void Chessboard::AI(stack <eachRound>&round,int depth){
    singleMove bestMove=alphaBetaMove(depth,-INT_MAX,INT_MAX,side);
    eachRound r{bestMove.from.x,bestMove.from.y,board[bestMove.from.x][bestMove.from.y],bestMove.to.x,bestMove.to.y,board[bestMove.to.x][bestMove.to.y]};
    round.push(r);
    makeMove(bestMove,side);
}



MCTSNode::MCTSNode(){
    value = 0;
    travelNum = 0;
    subNum = 0;
    ArrayList moveList;
    subMCTS = NULL;
    parent = NULL;
}

MCTS::MCTS(Chessboard gameBoard){
    this->oriBoard = gameBoard;
    this->oriBoard.print();
}

int MCTS::search(int currentPlayer){
    int depth,maxIndex;
    double currentUCT,maxUCT;
    MCTSNode root;
    MCTSNode *currentNode;
    singleMove move;
    gameBoard = oriBoard;
    expand(&root,currentPlayer);
    for(int i = 0; i<1000; i++){
        depth = 0;
        currentNode = &root;
        gameBoard = oriBoard;
        while(currentNode->subMCTS != NULL){
            maxUCT = DBL_MIN;
            maxIndex = 0;
            for(int j = 0; j < currentNode->subNum; j++){
                if(currentNode->subMCTS[j].travelNum == 0){
                    maxIndex = j;
                    break;
                }
                currentUCT = currentNode->subMCTS[j].value/currentNode->subMCTS[j].travelNum + sqrt(log(currentNode->travelNum)/currentNode->subMCTS[j].travelNum);
                if(currentUCT > maxUCT){
                    maxUCT = currentUCT;
                    maxIndex = j;
                }
            }   
            currentNode->moveList.pull(move,maxIndex);
            gameBoard.makeMove(move,currentPlayer);
            currentPlayer = -currentPlayer;
            currentNode = &currentNode->subMCTS[maxIndex];
            depth++;
        }

        if(currentNode ->travelNum != 0){
            expand(currentNode,currentPlayer);
            currentNode->moveList.pull(move,0);
            gameBoard.makeMove(move,currentPlayer);
            currentPlayer = -currentPlayer;
            currentNode = &currentNode->subMCTS[0];
        }
        
        Backpropagation(currentNode,rollout(currentNode,currentPlayer,depth));
    }
    maxUCT = DBL_MIN;
    for(int i = 0; i < root.subNum; i++)
    {
        if(root.subMCTS[i].value/root.subMCTS[i].travelNum > maxUCT){
            maxUCT = root.subMCTS[i].value/root.subMCTS[i].travelNum;
            maxIndex = i;
        }    
    }

    return maxIndex;
}

void MCTS::expand(MCTSNode *currentNode,int currentPlayer){
    gameBoard.Move_Generate(currentNode->moveList,currentPlayer);
    currentNode->subNum = currentNode->moveList.size();
    currentNode->subMCTS = new MCTSNode[currentNode->subNum];
    for(int i=0;i<currentNode->subNum;i++){
        currentNode->subMCTS[i].parent = currentNode;
    }
}

int MCTS::rollout(MCTSNode *currentNode,int currentPlayer,int depth){
    while(depth<=7){
        ArrayList temp;
        gameBoard.Move_Generate(temp,currentPlayer);
        singleMove move;
        temp.pull(move,rand()%temp.size());
        gameBoard.makeMove(move,currentPlayer);
        currentPlayer = -currentPlayer;
        depth++;
    }
    return gameBoard.Evaluate(currentPlayer);
}

void MCTS::Backpropagation(MCTSNode *currentNode,int rolloutValue){
    while(currentNode != NULL){
    currentNode->value += rolloutValue;
    currentNode->travelNum++;
    currentNode = currentNode->parent;
    }
}