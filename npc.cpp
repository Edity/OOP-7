#include "npc.h"
#include "knight.h"
#include "elf.h"
#include "dragon.h"
#include <algorithm>
#include <cmath>

NPC::NPC(NpcType t, int _x, int _y, const std::string& _name) : 
    type(t), x(_x), y(_y), name(_name) {}

NPC::NPC(NpcType t, std::istream &is) : type(t)
{
    is >> x;
    is >> y;
    is >> name;
    is >> move_distance;
    is >> kill_distance;
}

void NPC::save(std::ostream &os)
{
    os << x << " ";
    os << y << " ";
    os << name << " ";
    os << move_distance << " ";
    os << kill_distance << " ";
}

int NPC::roll_dice()
{
    return std::rand() % 6 + 1;
}

void NPC::move()
{
    if (!alive) return;
    
    int dx = (std::rand() % (2 * move_distance + 1)) - move_distance;
    int dy = (std::rand() % (2 * move_distance + 1)) - move_distance;
    
    x += dx;
    y += dy;
    
    x = std::clamp(x, 0, (int)MAP_SIZE);
    y = std::clamp(y, 0, (int)MAP_SIZE);
}

double NPC::distance(std::shared_ptr<NPC> other) const
{
    double dx = x - other->x;
    double dy = y - other->y;
    return std::sqrt(dx * dx + dy * dy);
}

bool NPC::is_close(std::shared_ptr<NPC> other) const
{
    return distance(other) <= kill_distance;
}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer)
{
    observers.push_back(observer);
}

void NPC::fight_notify(const std::shared_ptr<NPC> defender, bool win)
{
    std::lock_guard<std::mutex> lock(coutMutex);
    for (auto &o : observers)
        o->on_fight(shared_from_this(), defender, win);
}

std::ostream &operator<<(std::ostream &os, NPC &npc)
{
    os << "{ x:" << npc.x << ", y:" << npc.y << ", имя:'" << npc.name << "'} ";
    return os;
}