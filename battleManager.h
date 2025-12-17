#ifndef BATTLEMANAGER_H
#define BATTLEMANAGER_H

#include "npc.h"
#include <queue>
#include <mutex>

struct BattleTask {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

extern std::queue<BattleTask> battleTasks;
extern std::mutex battleTasksMutex;

void completeBattle(const BattleTask& task);

#endif