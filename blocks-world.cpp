#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <utility>
#include <list>
#include <functional>
#include <cmath>
#include <algorithm>
#include <utility>
#include <iomanip>
#include <limits>
#include <chrono>

using namespace std;

//typedef vector<int> State;
#include "blocks-world-state.cpp"

// Heuristic 1 : number of blocs not on the last stack
// Heuristic 2 :
//      * Si un bloc n'est pas sur la dernière pile +1
//      * Si tous les blocs en dessous sont pas bien rangés => +1
//      * Si un bloc n'est pas bien placé bloc n'est pas bien placé sur la dernière pile +2

typedef pair<int, int> Move;

typedef function<int(const State &pos)> Heuristic;

//int
//side( const State& b )
//{
//    double y = sqrt( b.size() );
//    int x = y;
//    return x;
//}


int
nullHeuristic(const State &b)
{
    return 0;
}

// Number of blocs not on the last stack
int heuristic1(const State &b) {
    return b.NbBlocsNotOnStack(b.getNbStacks() - 1);
}

int heuristicProf(const State &b) {
    return b.heuristicProf();
}
//int
//nbmis( const State& b )
//{
//    int d = 0;
//    for( int i = 0 ; i < b.size() ; i++ )
//    {
//        if( b[i] != 0 ) // not a tile, '0' doesn't count
//        {
//            if( b[i] != i ) d++;
//        }
//    }
//    return d;
//}

//bool
//finalState( const State& b )
//{
//    return (nbmis(b) == 0); // we use nbmis for it is quick to compute
//}

//void
//print( const State& state )
//{
//    int s = side(state);
//    for( int i = 0 ; i < state.size() ; i++ )
//    {
//        if( i % s == 0 ) cout << endl;
//        cout << setw(2) << setfill('0') << state[i] << " , ";
//    }
//    cout << endl;
//}

//void
//doMove( State &state, const Move &move )
//{
//    swap( state[move.first] , state[move.second] );
//}

//void
//addNeighbor(State &currentState, Move &move,
//            vector<pair<Move, int> > &neighbors,
//            list<State> &path, Heuristic h)
//{
//    currentState.doMove(move);
//    if (find(path.begin(), path.end(), currentState) == path.end())
//    {
//        neighbors.push_back(make_pair(move, h(currentState)));
//    }
//    currentState.doMove(move); // undo move
//}

void
search(State &currentState,
       int ub, // upper bound over which exploration must stop
       int &nub,
       vector<State> &path,
       vector<State> &bestPath,
       Heuristic h,
       int &nbVisitedState)
{
    nbVisitedState++;

    int f; // under-estimation of optimal length
    int g = path.size() - 1; // size of the current path to currentState

    if (currentState.isFinal())
    {
        bestPath = path;
        return;
    }

    // generate the neighbors
    vector<State> neighbors;
    currentState.getNeighbours(path, neighbors);

    for (State state : neighbors) {
        int heuristic = h( state );
        f = g + 1 + heuristic;
        if (f > ub) {
            if (f < nub) {
                nub = f;
            }
        }else {
            path.push_back(state);
            search (state, ub, nub, path, bestPath, h, nbVisitedState);
            path.pop_back();
            if ( !bestPath.empty() ) return;
        }
    }
}

void
blocksWorld(State &initialState,
            Heuristic h,
            vector<State> &bestPath, // path from source to destination
            int &nbVisitedState)
{
    int ub;                      // current upper bound
    int nub = h(initialState); // next upper bound
    vector<State> path;

    path.push_back(initialState); // the path to the target starts with the source

    while (bestPath.empty() && nub != numeric_limits<int>::max())
    {
        ub = nub;
        nub = numeric_limits<int>::max();

        cout << "upper bound: " << ub;
        search(initialState, ub, nub, path, bestPath, h, nbVisitedState);
        cout << " ; nbVisitedState: " << nbVisitedState << endl;
    }
}

int
main()
{
    //State b = {11,5,12,14,15,2,0,9,13,7,6,1,3,10,4,8};
//    State b = {7,11,8,3,14,0,6,15,1,4,13,9,5,12,2,10};
    //State b = {4,8,3,2,0,7,6,5,1};

    int nbStacks = 4;
    int nbBlocs = 20;
    State b = State(nbBlocs, nbStacks);
    b.setInitial();

    vector<State> bestPath;
    int nbVisitedState = 0;

    auto start = std::chrono::high_resolution_clock::now();
    blocksWorld(b, heuristicProf, bestPath, nbVisitedState);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    cout << "Elapsed time: " << elapsed.count() << " s\n";
    cout << "nb moves: " << bestPath.size() - 1 << endl;
    cout << "nb visited states: " << nbVisitedState << endl;

    return 0;
}

