#include "knight.h"
#include "elf.h"
#include "dragon.h"

Knight::Knight(int x, int y, const std::string& name) : 
    NPC(KnightType, x, y, name) {
    move_distance = 30;
    kill_distance = 10;
}

Knight::Knight(std::istream &is) : NPC(KnightType, is) {}

void Knight::print()
{
    std::cout << *this;
}

void Knight::save(std::ostream &os)
{
    os << std::endl << KnightType << std::endl;
    NPC::save(os);
}

bool Knight::accept(std::shared_ptr<NPC> visitor)
{
    return visitor->fight(std::dynamic_pointer_cast<Knight>(shared_from_this()));
}

bool Knight::fight(std::shared_ptr<Knight> other)
{
    int attack_power = roll_dice();
    int defense_power = other->roll_dice();
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

bool Knight::fight(std::shared_ptr<Elf> other)
{
    int attack_power = roll_dice();
    int defense_power = other->roll_dice();
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

bool Knight::fight(std::shared_ptr<Dragon> other)
{
    int attack_power = roll_dice();
    int defense_power = other->roll_dice();
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

std::ostream &operator<<(std::ostream &os, Knight &knight)
{
    os << "Странствующий рыцарь '" << knight.name << "': " 
       << *static_cast<NPC *>(&knight);
    return os;
}