#include <iostream>
#include <utility>
#include <vector>
#include <list>

using namespace std;

class State
{
public:
    State(int b, int s);
    // Constructor that creates a state with b blocs and s stacks

    State(const State &s);
    // Constructor that creates a copy of s

    void setInitial();
    // Initialise this with the initial state (blocs uniformly distributed)

    bool isFinal();
    // Return true if this is a final state (all blocs on the last stack, from
    // largest to smallest)

    typedef pair<int, int> Move;
    // A move is a pair of stacks: the bloc on top of the first stack is moved to
    // the top of the second stack

    void doMove(Move m);
    // Precondition: 0 <= m.first < getNbStacks(), 0 <= m.second < getNbStacks(),
    //               and !emptyStack(m.first)
    // Apply the doMove (m.first->m.second) to this

    void getNeighbours(vector<State> path, vector<State> &neighbors);

    void display() const;
    // Display this

    bool operator==(const State &) const;

    int getNbStacks() const;
    // Return the number of stacks

    bool emptyStack(int s);
    // Precondition: 0 <= s < getNbStacks()
    // Return true if stack s has no block

    ~State();
    // Destructor

    int NbBlocsNotOnStack(int s) const;

    int getNbBlocs() const;

    int heuristicProf() const;

private:
    int nbBlocs, nbStacks;
    int *stack; // For each bloc b: stack[b] = stack that contains b
    int *next;  // For each bloc b: if b is the lowest block of its stack,
    // then next[b]=-1, else next[b]=bloc immediately below b
    int *top; // For each stack s: if s is empty, then top[s]=-1
    // else top[s]=bloc on top of s
    bool equal(const int *, const int *, const int *) const; // compare states
};

int State::heuristicProf() const
{
    int sum = 0;
    //  * If block is not on the last stack then +1
    //  * If one of the blocks below the current block is bigger than the current block => +1
    //  * If block is not well placed on the last stack +2

    for (int i = 0; i < nbBlocs; i++)
    {
        if (stack[i] != nbStacks - 1)
        {
            sum++;
            int j = i;
            while (next[j] != -1)
            {
                if (next[j] > i)
                {
                    sum++;
                    break;
                }
                j = next[j];
            }
        }
        else if (stack[i] == nbStacks - 1)
        {
            int j = i;
            while (next[j] != -1)
            {
                if (next[j] != j + 1)
                {
                    sum += 2;
                    break;
                }
                j = next[j];
            }
            if (next[j] == -1 && j != nbBlocs - 1)
            {
                sum += 2;
            }
        }
    }
    return sum;
}

int State::getNbBlocs() const
{
    return nbBlocs;
}

State::State(int b, int s)
{
    // Constructor that creates a state with b blocs and s stacks
    nbBlocs = b;
    nbStacks = s;
    stack = new int[nbBlocs];
    next = new int[nbBlocs];
    top = new int[nbStacks];
}

State::State(const State &state)
{
    // Constructor that creates a copy of state
    nbBlocs = state.nbBlocs;
    nbStacks = state.nbStacks;
    stack = new int[nbBlocs];
    next = new int[nbBlocs];
    top = new int[nbStacks];
    for (int s = 0; s < nbStacks; s++)
    {
        top[s] = state.top[s];
    }
    for (int b = 0; b < nbBlocs; b++)
    {
        next[b] = state.next[b];
        stack[b] = state.stack[b];
    }
}

State::~State()
{
    delete[] next;
    delete[] stack;
    delete[] top;
}

void State::setInitial()
{
    // Initialise this with the initial state (blocs uniformly distributed)
    for (int s = 0; s < nbStacks; s++)
        top[s] = -1;
    for (int b = 0; b < nbBlocs; b++)
    {
        int s = b % nbStacks; // Put bloc b on top of stack s
        stack[b] = s;
        next[b] = top[s];
        top[s] = b;
    }
}

bool State::isFinal()
{
    // Return true if this is a final state (all blocs on the last stack, from
    // largest to smallest)
    for (int s = 0; s < nbStacks - 1; s++)
    {
        if (top[s] != -1)
            return false;
    }
    if (top[nbStacks - 1] != 0)
        return false;
    for (int b = 0; b < nbBlocs - 1; b++)
    {
        if (stack[b] != nbStacks - 1)
            return false;
        if (next[b] != b + 1)
            return false;
    }
    if (next[nbBlocs - 1] != -1)
        return false;
    return true;
}

void State::doMove(Move m)
{
    // Precondition: 0 <= m.first < getNbStacks(), 0 <= m.second < getNbStacks(),
    //               and !emptyStack(m.first)
    // Apply the doMove (s1->s2) to this
    int b1 = top[m.first];
    int bb1 = next[b1];
    int b2 = top[m.second];
    next[b1] = b2;
    top[m.second] = b1;
    top[m.first] = bb1;
    stack[b1] = m.second;
}

void displayStack(int s, int *next)
{
    // Display all blocs below s (including s), from bottom to top
    if (s >= 0)
    {
        displayStack(next[s], next);
        cout << s << " ";
    }
}

void State::display() const
{
    // Display this
    for (int s = 0; s < nbStacks; s++)
    {
        cout << "stack " << s << ": ";
        displayStack(top[s], next);
        cout << endl;
    }
}

int State::getNbStacks() const
{
    return nbStacks;
}

bool State::emptyStack(int s)
{
    // Precondition: 0 <= s < getNbStacks()
    // Return true if stack s has no block
    return top[s] == -1;
}

bool State::operator==(const State &state) const
{
    // Return true if this == state
    return state.equal(stack, top, next);
}

int State::NbBlocsNotOnStack(int s) const
{
    // Return the number of blocs not on stack s
    int nb = 0;
    for (int b = 0; b < nbBlocs; b++)
    {
        if (stack[b] != s)
            nb++;
    }
    return nb;
}

bool State::equal(const int *stack2, const int *top2, const int *next2) const
{
    for (int b = 0; b < nbBlocs; b++)
    {
        if (stack[b] != stack2[b])
            return false;
        if (next[b] != next2[b])
            return false;
    }
    for (int s = 0; s < nbStacks; s++)
    {
        if (top[s] != top2[s])
            return false;
    }
    return true;
}

void State::getNeighbours(vector<State> path, vector<State> &neighbors)
{
    // Return the neighbours of this
    for (int s1 = 0; s1 < nbStacks; s1++)
    {

        for (int s2 = 0; s2 < nbStacks; s2++)
        {
            if (s1 != s2 && !emptyStack(s1))
            {
                State state(*this);
                state.doMove(make_pair(s1, s2));
                bool found = false;
                for (int i = 0; i < path.size(); i++)
                {
                    if (state == path[i])
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    neighbors.push_back(state);
                }
            }
        }
    }
}
