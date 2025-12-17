#pragma once

#include <iostream>
#include <memory>
#include <cstring>
#include <string>
#include <random>
#include <fstream>
#include <set>
#include <math.h>
#include <mutex>
#include <shared_mutex>

constexpr size_t MAP_SIZE = 100;
constexpr size_t NPC_COUNT = 50;
constexpr size_t GAME_LENGTH = 30;

struct NPC;
struct Knight;
struct Elf;
struct Dragon;

using set_t = std::set<std::shared_ptr<NPC>>;

enum NpcType
{
    Unknown = 0,
    KnightType = 1,
    ElfType = 2,
    DragonType = 3
};

extern std::mutex coutMutex;
extern std::shared_mutex npcMutex;

struct IFightObserver {
    virtual void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win) = 0;
};

struct NPC : public std::enable_shared_from_this<NPC>
{
protected:
    NpcType type;
    int x;
    int y;
    std::string name;
    int move_distance{0};
    int kill_distance{0};
    bool alive{true};
    std::vector<std::shared_ptr<IFightObserver>> observers;

public:
    NPC(NpcType t, int _x, int _y, const std::string& _name);
    NPC(NpcType t, std::istream &is);

    NpcType get_type() const { return type; }
    std::pair<int, int> position() const { return {x, y}; }
    const std::string& get_name() const { return name; }
    int get_move_distance() const { return move_distance; }
    int get_kill_distance() const { return kill_distance; }
    bool is_alive() const { return alive; }

    void die() { alive = false; }

    void subscribe(std::shared_ptr<IFightObserver> observer);
    void fight_notify(const std::shared_ptr<NPC> defender, bool win);
    virtual bool is_close(std::shared_ptr<NPC> other) const;

    virtual bool accept(std::shared_ptr<NPC> visitor) = 0;
    
    virtual bool fight(std::shared_ptr<Knight> other) = 0;
    virtual bool fight(std::shared_ptr<Elf> other) = 0;
    virtual bool fight(std::shared_ptr<Dragon> other) = 0;
    
    virtual void print() = 0;
    virtual void save(std::ostream &os);

    friend std::ostream &operator<<(std::ostream &os, NPC &npc);

    void move();
    int roll_dice();
    double distance(std::shared_ptr<NPC> other) const;
};