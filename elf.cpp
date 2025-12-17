#include "elf.h"
#include "knight.h"
#include "dragon.h"

Elf::Elf(int x, int y, const std::string& name) : 
    NPC(ElfType, x, y, name) {
    move_distance = 10;
    kill_distance = 50;
}

Elf::Elf(std::istream &is) : NPC(ElfType, is) {}

void Elf::print()
{
    std::cout << *this;
}

void Elf::save(std::ostream &os)
{
    os << std::endl << ElfType << std::endl;
    NPC::save(os);
}

bool Elf::accept(std::shared_ptr<NPC> visitor)
{
    return visitor->fight(std::dynamic_pointer_cast<Elf>(shared_from_this()));
}

bool Elf::fight(std::shared_ptr<Knight> other)
{
    int attack_power = roll_dice();
    int defense_power = other->roll_dice();
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

bool Elf::fight(std::shared_ptr<Elf> other)
{
    int attack_power = roll_dice();
    int defense_power = other->roll_dice();
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

bool Elf::fight(std::shared_ptr<Dragon> other)
{
    int attack_power = roll_dice();
    int defense_power = other->roll_dice();
    bool win = attack_power > defense_power;
    fight_notify(other, win);
    return win;
}

std::ostream &operator<<(std::ostream &os, Elf &elf)
{
    os << "Эльф '" << elf.name << "': " 
       << *static_cast<NPC *>(&elf);
    return os;
}