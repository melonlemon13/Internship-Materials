#pragma once

// 「どの攻撃でキルしたか」を識別するタイプ
enum class KillType
{
    Unknown = 0,
    Pistol,
    AR,
    Knife,
    Grenade,
    ULT,
};