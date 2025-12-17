#include "dragon.h"
#include "knight.h"
#include "elf.h"

Dragon::Dragon(int x, int y, const std::string& name) : 
    NPC(DragonType, x, y, name) {
    move_distance = 50;
    kill_distance = 30;
}

Dragon::Dragon(std::istream &is) : NPC(DragonType, is) {}

void Dragon::print()
{
    std::cout << *this;
}

void Dragon::save(std::ostream &os)
{
    os << std::endl << DragonType << std::endl;
    NPC::save(os);
}

bool Dragon::accept(std::shared_ptr<NPC> visitor)
{
    return visitor->fight(std::dynamic_pointer_cast<Dragon>(shared_from_this()));
}

bool Dragon::fight(std::shared_ptr<Knight> other)
{
    int attack_power = roll_dice();
    int defense_power = other->roll_dice();
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

bool Dragon::fight(std::shared_ptr<Elf> other)
{
    int attack_power = roll_dice();
    int defense_power = other->roll_dice();
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

bool Dragon::fight(std::shared_ptr<Dragon> other)
{
    int attack_power = roll_dice();
    int defense_power = other->roll_dice();
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

std::ostream &operator<<(std::ostream &os, Dragon &dragon)
{
    os << "Дракон '" << dragon.name << "': " 
       << *static_cast<NPC *>(&dragon);
    return os;
}