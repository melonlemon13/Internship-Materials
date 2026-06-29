#pragma once

enum class MatchOutcome
{
    Clear = 0,
    GameOver
};

struct PlayResultData
{
    MatchOutcome outcome = MatchOutcome::Clear;

    // 生存時間（フレーム）
    int survivalFrames = 0;

    // Kill内訳
    int totalKills = 0;
    int pistolKills = 0;
    int arKills = 0;
    int knifeKills = 0;
    int grenadeKills = 0;
    int ultKills = 0;
};
