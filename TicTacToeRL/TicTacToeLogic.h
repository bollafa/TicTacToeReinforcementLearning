#pragma once
#include <utility>
#include <unordered_map>
#include <string>
enum PossibleMoves : unsigned char
{
    TOPLEFT,
    MIDTOP,
    TOPRIGHT,
    MIDLEFT,
    MID,
    MIDRIGHT,
    BOTLEFT,
    BOTMID,
    BOTRIGHT
};
enum CellType : unsigned char
{
    Blank,
    Cross,
    Circle
};
enum GameState : unsigned char
{
    Playing,
    Draw,
    Loss,
    Win,
};
std::unordered_map<CellType, std::string> Graphics = { {Blank,"."},{Cross,"X"},{Circle,"O"} };
std::unordered_map<GameState, std::string> GameStateStr = { { Playing,"Playing" },{ Draw,"Draw" },{ Loss,"Loss" },{Win,"Win"} };

/*
   @Map : 9 - Sized Array 
*/
std::pair<CellType,GameState> CheckGameState(const void* Map)
{
    const auto&& _Map = (static_cast<const CellType*>(Map));
    // Suppose: map is a 9 sized array row first
    //  0 1 2
    //  3 4 5
    //  6 7 8
    /* Check Rows */
    // Not extendible, but who really cares
    for (int i = 0; i < 3; i++) 
    {
       bool bFull = _Map[i * 3 + 0] == _Map[i * 3 + 1] && _Map[i*3 + 1] == _Map[i*3 + 2] 
           && _Map[i * 3 + 0] != Blank;
       if (bFull)
           return { _Map[i * 3 + 0], Win };
    }
    /* Check Cols */
    for (int i = 0; i < 3; i++)
    {
        bool bFull = _Map[i + 0 * 3] == _Map[i + 1 * 3]  && _Map[i+ 1 * 3] == _Map[i + 2 * 3]
            && _Map[i + 0 * 3] != Blank;
        if (bFull)
            return { _Map[i + 0 * 3], Win };
    }
    /* Check Diagonals */
    /*for (int w = 0; w < 3; w++)
    {
        for (int i, j = 0; i < 3; i++, j++)
        {
            bool bFull = _Map[i * 3 + 0] == _Map[i * 3 + 1] == _Map[i * 3 + 2];
            if (bFull)
                return { _Map[i * 3 + 0], Win };
        }
    }*/
    bool bFull = _Map[0] == _Map[4]  && _Map[4] == _Map[8] && _Map[0] != Blank;
    if (bFull)
        return { _Map[0], Win };
    bFull = _Map[2] == _Map[4] && _Map[4] == _Map[6] && _Map[2] != Blank; 
    if (bFull)
        return { _Map[2], Win };
    //
    GameState FinalGameState = Draw;
    /* Check there are no blanks, WHY we got here so we can only be drawing or playing */
    for (int i = 0; i < 9; i++)
    {
        if (_Map[i] == Blank)
        {
            FinalGameState =  Playing;
            break;
        }
        
    }
    /* Default: Playing */
    return { Circle,FinalGameState };
}

void PrintMap(const void* Map)
{
    const auto&& _Map = (static_cast<const CellType*>(Map));
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            
            std::cout << Graphics[_Map[i * 3 + j]];
            
            std::cout << " ";
        }
        std::cout << '\n';
    }
}