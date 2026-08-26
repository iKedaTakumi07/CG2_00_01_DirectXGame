#pragma once
#include "baseEnemy.h"

class baseBossEnemy : public baseEnemy {
public:
    virtual float GetHpRate() const = 0;
    virtual int GetCurrentPhase() const = 0;

protected:
    int maxHp_ = 1000;
    int currentHp_ = 1000;
    int currentPhase_ = 1;
};
