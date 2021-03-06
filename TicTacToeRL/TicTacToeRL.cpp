// TicTacToeRL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
// Now on C/C++ later on LISP.
#include "TicTacToeLogic.h"
#include "Agent.h"



// Possible states, array of arrays. [ [state1] , [state2] ] state1 := [ st1,st2,st3...] and so on.

template<typename T>
struct LinkedStateList
{
    std::array<T,9> Map;
    std::vector<LinkedStateList<T>> Next;
};
template<typename T>
using LList = LinkedStateList<T>;
/*
    Returns an LinkedList? array Of all posible states ** there are a lot **
*/
// Recursive????? NoTE: i hate them because they are hard as f to understand/read
std::vector<LList<CellType>> AllPossibleStates(int depth = 1, std::array<CellType, 9> InitialMap =  { Blank,Blank,Blank , Blank,Blank,Blank , Blank,Blank,Blank }, bool bCTurn = true )
{
    static int times = 0;
    /*
        For now just depth 1 . Meaning only the first slice of possibilities ( i dont even know what that means)
    */
    std::vector<LList<CellType>> States;
    // WHY? Crosses always start, ? PENDING DELETE OBV OFC
    
    while(times < depth)
        for (int i = 0; i < 9; i++)
        {
            bool bCrossTurn = bCTurn;
            std::array<CellType, 9> Map = InitialMap;
            LList<CellType> TempState;
            if(Map[i] == Blank)
                Map[i] = bCrossTurn ? Cross : Circle;
            if (CheckGameState(&Map).second == GameState::Win || CheckGameState(&Map).second == GameState::Draw)
                break;
            //times++;
            bCrossTurn = !bCrossTurn;
            States.push_back(LList<CellType>{ Map, AllPossibleStates(9,Map,bCrossTurn) }); // Empty depth 2 because math
            
        }
    /*int numberofmoves = 0;
    
    for (int j = 0; j < 9; j++)
    {
        std::array<CellType, 9> Map = InitialMap;
        Map[j] = Cross;
        LList<CellType> CurrentState;
        CurrentState.Map = Map;
        for (int i = j; i < 9-j; i++)
        {
            std::array<CellType, 9> NewMap = Map;
            NewMap[i] = Circle;

        }
        
    }*/
    
        
    return States;
}

bool CompareStateValuePair(const StateValuePair& lhs, const StateValuePair& rhs)
{
    return lhs.second < rhs.second;
}
std::array<CellType, 9> BlankMap = { Blank,Blank,Blank ,Blank,Blank,Blank ,Blank,Blank,Blank };
// Agent is Circle, crosses start.
void inline Practice(Agent& CrossAgent,  Agent& CircleAgent,const unsigned int& NumberOfRuns,Agent::AttackMode&& CrossType)
{
    for (int i = 0; i <NumberOfRuns; i++)
    {
        std::array<CellType, 9> IMap = BlankMap; // initial state
        bool bGameEnded = false;
        while (!bGameEnded)
        {
            /*IMap[CrossAgent.AgentPlay(IMap)] = Cross;
            if (CheckGameState(&IMap).second != Playing)
            {
            bGameEnded = true;
            CrossAgent.Learn(IMap);
            CircleAgent.Learn(IMap);
            }*/

            CircleAgent.Move(IMap);
            CircleAgent.StateHistory.push_back(IMap);
            /*std::cout << "-X-\n";
            PrintMap(&IMap);
            std::cout << '\n';*/
            CrossAgent.Move(IMap, CrossType);
            /*std::cout << "-O-\n";
            PrintMap(&IMap);
            std::cout << '\n';*/
            CircleAgent.StateHistory.push_back(IMap);
            CrossAgent.StateHistory.push_back(IMap);
            if (CheckGameState(&IMap).second != Playing)
            {
                /*std::cout << "CellType: " << Graphics[CheckGameState(&IMap).first] << '\n';
                std::cout << "State: " << GameStateStr[CheckGameState(&IMap).second] << '\n';*/
                switch (CheckGameState(&IMap).second)
                {
                case Win:
                    if (CheckGameState(&IMap).first == Cross)
                    {
                        CrossAgent.MatchHistory[Agent::Won]++;
                        CircleAgent.MatchHistory[Agent::Lost]++;
                    }
                    else
                    {
                        CircleAgent.MatchHistory[Agent::Won]++;
                        CrossAgent.MatchHistory[Agent::Lost]++;
                    }

                    break;
                case Draw:
                    CircleAgent.MatchHistory[Agent::Drew]++;
                    CrossAgent.MatchHistory[Agent::Drew]++;
                    break;
                default:
                    break;
                }
                /*std::cout << "CellType: " << Graphics[CheckGameState(&IMap).first] << '\n';
                std::cout << "State: " << GameStateStr[CheckGameState(&IMap).second] << '\n';
                PrintMap(&IMap);
                std::cout << '\n';*/

                CircleAgent.Finish();
                CrossAgent.Finish();
                bGameEnded = true;
            }

        }
    }
}
int main()
{
    
    /*std::cout << "CellType: " << Graphics[CheckGameState(Map).first] << std::endl;
    std::cout << "State: " <<  GameStateStr[CheckGameState(Map).second] << std::endl;*/
    //std::vector<LList<CellType>> AllStates = AllPossibleStates();
    /*for (auto& map : AllStates)
    {
        PrintMap(&map.Map);
        for (auto& a : map.Next)
        {
            PrintMap(&a.Map);
            std::cout << '\n';
        }
        std::cout << '\n';
    }*/
    auto Maps = NextPossibleStates();
    /*for (const auto &a : Maps)
    {        
        PrintMap(&a.first);
        std::cout << "Value: " << StateValue[a.first] << '\n';
        std::cout << "AI Move: " << AgentPlay(a.first) << '\n';
    }*/
    std::cout << "0 1 2\n3 4 5\n6 7 8\n";
    Agent CrossAgent(Cross);
    Agent CircleAgent(Circle);
    const unsigned int NofTests = 450000;
    
    Practice(CrossAgent, CircleAgent, NofTests, Agent::Greedy);
    Practice(CrossAgent, CircleAgent, NofTests, Agent::Random);
    Practice(CrossAgent, CircleAgent, NofTests, Agent::Learning);
    Practice(CrossAgent, CircleAgent, NofTests, Agent::Greedy);
    std::cout << "CircleAgent: WON: " << ((float)CircleAgent.MatchHistory[Agent::Won]/ ((float)NofTests * 4))*100 << "% drew: "<< ((float)CircleAgent.MatchHistory[Agent::Drew] / ((float)NofTests * 4)) * 100 <<'\n';
    std::cout << "CrossAgent: WON: " << ((float)CrossAgent.MatchHistory[Agent::Won] / ((float)NofTests*4))*100 << "% drew: " << CrossAgent.MatchHistory[Agent::Drew]<< '\n';
    // Now player time!
    
    
    for (;;)
    {
        std::array<CellType, 9> TreeMap = { Blank,Blank,Blank ,Blank,Blank,Blank , Blank,Blank,Blank }; // initial state
        while (true)
        {

            CircleAgent.Move(TreeMap,Agent::Greedy);
            std::cout << "------\tO\n";
            PrintMap(&TreeMap);
            std::cout << '\n';
            if (CheckGameState(&TreeMap).second != Playing)
            {
                std::cout << "CellType: " << Graphics[CheckGameState(&TreeMap).first] << '\n';
                std::cout << "State: " << GameStateStr[CheckGameState(&TreeMap).second] << '\n';
                //PrintMap(&TreeMap);
                std::cout << '\n';

                break;
            }
            // Player Turn:
            int Place = 0;
            std::cin >> Place;
            TreeMap[Place] = Cross;
            std::cout << "------\tX\n";
            PrintMap(&TreeMap);
            std::cout << '\n';
            if (CheckGameState(&TreeMap).second != Playing)
            {
                std::cout << "CellType: " << Graphics[CheckGameState(&TreeMap).first] << '\n';
                std::cout << "State: " << GameStateStr[CheckGameState(&TreeMap).second] << '\n';
                //PrintMap(&TreeMap);
                std::cout << '\n';

                break;
            }
        }
    }
    
        

    
    //PrintMap(Map);

}

