#include "battleManager.h"

std::queue<BattleTask> battleTasks;
std::mutex battleTasksMutex;

void completeBattle(const BattleTask& task)
{
    auto attacker = task.attacker;
    auto defender = task.defender;

    if (!attacker->is_alive() || !defender->is_alive()) {
        return;
    }

    if (defender->accept(attacker)) {
        int attack = attacker->roll_dice();
        int defense = defender->roll_dice();

        bool success = (attack > defense);

        if (success) {
            defender->die();
            attacker->fight_notify(defender, true);
        } else {
            attacker->fight_notify(defender, false);
        }
    }
}