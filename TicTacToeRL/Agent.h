#pragma once
#include "stdafx.h"
#include "TicTacToeLogic.h"
#include <random>
std::random_device rd;     // only used once to initialise (seed) engine
std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
/*
Returns all possible next states from current one
*/
std::vector<std::pair<std::array<CellType, 9>, PossibleMoves>> NextPossibleStates(CellType Turn = Cross, const std::array<CellType, 9>& InitialMap = { Blank,Blank,Blank , Blank,Blank,Blank , Blank,Blank,Blank })
{
    std::vector<std::pair<std::array<CellType, 9>, PossibleMoves>> States;
    if (CheckGameState(&InitialMap).second & (Win | Draw))
        return {};
    for (int i = 0; i < 9; i++)
    {
        std::array<CellType, 9> Map = InitialMap;
        if (Map[i] == Blank)
        {
            Map[i] = Turn == Cross ? Cross : Circle;
            States.push_back({ Map,PossibleMoves(i) });
        }
            

        
    }
    return States;
}
// Copied from https://codereview.stackexchange.com/questions/171999/specializing-stdhash-for-stdarray
template<class T, size_t N>
struct std::hash<std::array<T, N>> {
    std::hash<T> hasher;
    auto operator() (const std::array<T, N>& key) const {

        size_t result = 0;
        for (size_t i = 0; i < N; ++i) {
            result = result * 31 + hasher(key[i]); // ??
        }
        return result;
    }
};
// -1 Crosses win, 0 draw, +1 Circle wins

using StateValuePair = std::pair<std::pair<std::array<CellType, 9>, PossibleMoves>, double>;

class Agent
{
private:
    double dLearningFactor = 0.8;
    const int epsilon = 40;
    CellType Type;
    struct StateWrapper
    {
    private:
        std::unordered_map<std::array<CellType, 9>, double> mStateValue;
        CellType mType;
    public:
        StateWrapper(CellType _type) : mType(_type) {}
        double& operator[](const std::array<CellType, 9>& lhs)
        {
            if (mStateValue.count(lhs) == 0)
            {
                mStateValue[lhs] = Learn(lhs,mType);
            }
            return mStateValue[lhs];
        }
    } StateValue;
    //std::unordered_map<std::array<CellType, 9>, double> StateValue;
public:
    enum AttackMode
    {
        Learning,
        Greedy,
        Random,
    };
    enum MatchType
    {
        Won,
        Drew,
        Lost
    };
    unsigned int MatchHistory[3] = { 0,0,0 };
public:
    Agent(const CellType&& Type) : Type(Type) , StateValue(Type){}
    Agent(const CellType&& Type, const double&& dLF) : dLearningFactor(dLF), StateValue(Type) {}
    std::vector < std::pair<std::pair<std::array<CellType, 9>, PossibleMoves>, double> > GetUtilityValues(const std::vector<std::pair<std::array<CellType, 9>, PossibleMoves>>& States)
    {
        std::vector < std::pair<std::pair<std::array<CellType, 9>, PossibleMoves>, double> > vFinal;
        for (const auto& a : States)
        {
            vFinal.push_back({ a,StateValue[a.first] });
        }
        return vFinal;
    }
    static bool CompareStateValuePair(const StateValuePair& lhs, const StateValuePair& rhs)
    {
        return lhs.second < rhs.second;
    }
    PossibleMoves AgentPlay(const std::array<CellType, 9> & State, AttackMode AttackType = Learning)
    {
        
        auto NextStates = NextPossibleStates(Type, State);
        auto UValues = GetUtilityValues(NextStates);
        std::sort(UValues.begin(), UValues.end(),&Agent::CompareStateValuePair);
        // ASSUMPSION : All values are the same, thus we shuffle 'em and return the last one. ( we could very well return the first one / or any one for that matters.)
        if (UValues.front().second == UValues.back().second || AttackType == Random)
        {
            //std::uniform_int_distribution<int> uni(0, UValues.size()); // guaranteed unbiased

            
            std::shuffle(UValues.begin(), UValues.end(), rng);
            return UValues.back().first.second;
        }
        std::uniform_int_distribution<int> uni(0, 100);
        if ( uni(rng) <= epsilon && AttackType == Learning)
        {
            // Randomize action without choosing the best path.
            UValues.pop_back();
            std::shuffle(UValues.begin(), UValues.end(), rng);
        }
        
        return UValues.back().first.second;
    }
    static double Learn(const std::array<CellType, 9> & State,CellType _type)
    {
        auto CGameState = CheckGameState(&State);
        /*if (CGameState.first == _type && CGameState.second == Win)
        {
            return 1.0;
            //MatchHistory[Won]++;
        }
        else
        {
            if (CGameState.second == Draw && CGameState.first == _type)
            {
                //MatchHistory[Drew]++;
                return 0;
            }
            if(CGameState.second == Loss && CGameState.first == _type)
            {

               // MatchHistory[Lost]++;
                return -1.0;
            }
            
        }*/
        switch (CGameState.second)
        {
        case Win:
            return CGameState.first == _type ? 1.0 : -1.0;
            break;
        case Draw:
            return 0;
            break;
        case Loss:
            return CGameState.first == _type ? -1.0 : 1.0;
        default:
            return 0;
            break;
        }
        
        
    }
    std::vector<std::array<CellType, 9>> StateHistory;
    void Finish()
    {
        if(!StateHistory.empty())
        for (auto it = StateHistory.end()-1; it != StateHistory.begin() ; --it)
        {
            StateValue[*(it-1)] += dLearningFactor * (StateValue[*(it)] - StateValue[*(it-1)]);
        }
        StateHistory.clear();
    }
    void Move(std::array<CellType, 9>& State, AttackMode AttackType = Learning)
    {
        
        //if (CheckGameState(&State).second != Playing)
        //{
        //    
        //    Learn(State);
        //    StateValue[StateHistory.back()] = StateValue[StateHistory.back()] + dLearningFactor * (StateValue[State] - StateValue[StateHistory.back()]);
        //    StateHistory.clear();
        //    return;
        //}
        ///*else if(StateValue[State] == 0)
        //    StateValue[State] = 0.5;*/
        //const auto& OldState = State;
        //
        //const auto& Destination = AgentPlay(State, AttackType);

        //
        //    
        //
        //State[Destination] = Type;
        //StateHistory.push_back(OldState);
        //// Adjust utility's ( u ) value A.K.A LEARNING
        //auto Weird = StateValue[OldState];
        ////Learn(State);
        //StateValue[OldState] = StateValue[OldState] + dLearningFactor * (StateValue[State] - StateValue[OldState]);
        //if (CheckGameState(&State).second != Playing)
        //{
        //    //MoveHistory.clear();
        //    Learn(State);
        //    auto tt = StateValue[State];
        //    auto pp = StateValue[State];
        //    auto gp = StateValue[StateHistory.back()];
        //    double value =  (StateValue[State] - StateValue[StateHistory.back()]);
        //    StateValue[StateHistory.back()] = StateValue[StateHistory.back()] + dLearningFactor * (StateValue[State] - StateValue[StateHistory.back()]);
        //    StateHistory.clear();
        //    return;
        //}
        ///*else if (StateValue[State] == 0)
        //    StateValue[State] = 0.5;*/
        const auto& GameST = CheckGameState(&State);
        
        if (GameST.second == Playing)
        {
            auto OldState = State;
            //StateHistory.push_back(State);
            const auto& Destination = AgentPlay(State, AttackType);
            auto Point = &StateValue[OldState];
            State[Destination] = Type;
            //StateHistory.push_back(State);
            /*for (auto it = StateHistory.begin() + 1; it != StateHistory.end(); ++it)
            {
                StateValue[*(it-1)] = StateValue[*(it - 1)] + dLearningFactor * (StateValue[*it] - StateValue[*(it - 1)]) ;
            }*/
            //StateValue[OldState] = StateValue[OldState] + dLearningFactor * (StateValue[State] - StateValue[OldState]) /*33*/;
        }
    }
    
};