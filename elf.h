#pragma once
#include "npc.h"

struct Elf : public NPC
{
    Elf(int x, int y, const std::string& name);
    Elf(std::istream &is);

    void print() override;
    void save(std::ostream &os) override;

    bool accept(std::shared_ptr<NPC> visitor) override;

    bool fight(std::shared_ptr<Knight> other) override;
    bool fight(std::shared_ptr<Elf> other) override;
    bool fight(std::shared_ptr<Dragon> other) override;

    friend std::ostream &operator<<(std::ostream &os, Elf &elf);
};