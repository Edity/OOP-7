#include "npc.h"
#include "knight.h"
#include "elf.h"
#include "dragon.h"
#include "battleManager.h"

#include <atomic>
#include <ctime>
#include <thread>
#include <chrono>
#include <array>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std::chrono_literals;

std::atomic<bool> stopFlag = false;
std::atomic<int> gameSeconds{0};
std::mutex coutMutex;
std::shared_mutex npcMutex;

class TextObserver : public IFightObserver
{
public:
    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win)
    {
        if (win) {
            std::cout << std::endl;
            attacker->print();
            std::cout << " убивает ";
            defender->print();
        } else {
            std::cout << std::endl;
            attacker->print();
            std::cout << " убивает ";
            defender->print();
        }
    }
};

class FileObserver : public IFightObserver
{
private:
    std::ofstream logFile;

public:
    FileObserver(const std::string& fileName) {
        logFile.open(fileName, std::ios_base::app);
    }

    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win)
    {
        if (win && logFile.is_open()) {
            logFile << *attacker << " убивает " << *defender << std::endl;
        }
    }
};

std::shared_ptr<TextObserver> textObs = std::make_shared<TextObserver>();
std::shared_ptr<FileObserver> fileObs = std::make_shared<FileObserver>("log.txt");

std::shared_ptr<NPC> factory(std::istream &is)
{
    std::shared_ptr<NPC> result;
    int type{0};
    if (is >> type)
    {
        switch (type)
        {
            case KnightType:
                result = std::make_shared<Knight>(is);
                break;
            case ElfType:
                result = std::make_shared<Elf>(is);
                break;
            case DragonType:
                result = std::make_shared<Dragon>(is);
                break;
            default:
                break;
        }
    }

    if (result) {
        result->subscribe(textObs);
        result->subscribe(fileObs);
    }

    return result;
}

std::string generate_name(NpcType type, int id) {
    static std::vector<std::string> knightNames = {"Артемиус", "Гланцелот", "Рыдцарек"};
    static std::vector<std::string> elfNames = {"Леголас", "Эльронд", "Галадриэль"};
    static std::vector<std::string> dragonNames = {"Смауг", "Дрого", "Визерион"};
    
    std::string name;
    if (type == KnightType) 
        name = knightNames[std::rand() % knightNames.size()] + "_" + std::to_string(id);
    else if (type == ElfType)
        name = elfNames[std::rand() % elfNames.size()] + "_" + std::to_string(id);
    else
        name = dragonNames[std::rand() % dragonNames.size()] + "_" + std::to_string(id);
    
    return name;
}

std::shared_ptr<NPC> factory(NpcType type, int x, int y, int id)
{
    std::shared_ptr<NPC> result;
    std::string name = generate_name(type, id);

    switch (type) {
        case KnightType:
            result = std::make_shared<Knight>(x, y, name);
            break;
        case ElfType:
            result = std::make_shared<Elf>(x, y, name);
            break;
        case DragonType:
            result = std::make_shared<Dragon>(x, y, name);
            break;
        default:
            break;
    }

    if (result) {
        result->subscribe(textObs);
        result->subscribe(fileObs);
    }

    return result;
}

void save(const set_t &array, const std::string &fileName)
{
    std::ofstream fs(fileName);
    fs << array.size() << std::endl;
    for (auto &n : array) {
        n->save(fs);
    }
    fs.flush();
    fs.close();
}

set_t load(const std::string &fileName)
{
    set_t result;
    std::ifstream is(fileName);
    if (is.good() && is.is_open())
    {
        int count;
        is >> count;

        for (int i = 0; i < count; ++i) {
            result.insert(factory(is));
        }

        is.close();
    }

    return result;
}

void moveThread(set_t &npcs)
{
    while (!stopFlag) {
        {
            std::shared_lock<std::shared_mutex> lock(npcMutex);

            for (const auto &attacker : npcs) {
                if (attacker->is_alive()) {

                    attacker->move();

                    for (const auto &defender : npcs) {
                        if (attacker.get() != defender.get() && defender->is_alive() && attacker->is_close(defender)) {
                            std::lock_guard<std::mutex> tasks_lock(battleTasksMutex);
                            battleTasks.push({attacker, defender});
                        }
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(100ms);
    }
}

void battleThread()
{
    while (!stopFlag) {
        BattleTask currentTask;
        bool taskFound = false;

        {
            std::lock_guard<std::mutex> tasks_lock(battleTasksMutex);

            if (!battleTasks.empty()) {
                currentTask = battleTasks.front();
                battleTasks.pop();
                taskFound = true;
            }
        }

        if (taskFound) {
            completeBattle(currentTask);
        } else {
            std::this_thread::sleep_for(10ms);
        }
    }
}

void printThread(set_t &npcs)
{
    const int grid = 20;
    const int stepX = (int)MAP_SIZE / grid;
    const int stepY = (int)MAP_SIZE / grid;
    std::array<char, grid * grid> fields;

    while (!stopFlag && gameSeconds < GAME_LENGTH) {
        {
            std::shared_lock<std::shared_mutex> lock_npc(npcMutex);

            fields.fill(' ');

            for (const std::shared_ptr<NPC> &npc : npcs)
            {
                if (npc->is_alive())
                {
                    const auto [x, y] = npc->position();
                    int i = x / stepX;
                    int j = y / stepY;

                    if (i >= 0 && i < grid && j >= 0 && j < grid) {
                        char c = '_';
                        switch (npc->get_type())
                        {
                            case KnightType:
                                c = 'K';
                                break;
                            case ElfType:
                                c = 'E';
                                break;
                            case DragonType:
                                c = 'D';
                                break;
                            default:
                                break;
                        }
                        fields[i + grid * j] = c;
                    }
                }
            }

            lock_npc.unlock();

            {
                std::lock_guard<std::mutex> lock_cout(coutMutex);

                #ifdef _WIN32
                    system("cls");
                #else
                    std::cout << "\033[2J\033[1;1H";
                #endif

                std::cout << "         Игровое поле 100x100       \n";
                std::cout << "Время: " << gameSeconds.load() << "/" << GAME_LENGTH << " сек\n";
                std::cout << "Легенда: K-Рыцарь, E-Эльф, D-Дракон\n\n";
                
                for (int j = 0; j < grid; ++j) {
                    for (int i = 0; i < grid; ++i) {
                        char c = fields[i + j * grid];
                        std::cout << "[" << c << "]";
                    }
                    std::cout << std::endl;
                }
                
                int alive_count = 0;
                int knights = 0, elves = 0, dragons = 0;
                {
                    std::shared_lock<std::shared_mutex> lock_stat(npcMutex);
                    for (const auto &npc : npcs) {
                        if (npc->is_alive()) {
                            alive_count++;
                            switch (npc->get_type()) {
                                case KnightType: knights++; break;
                                case ElfType: elves++; break;
                                case DragonType: dragons++; break;
                            }
                        }
                    }
                }
                
                std::cout << "\nЖивых: " << alive_count << " (K:" << knights << " E:" << elves << " D:" << dragons << ")\n";
                std::cout << "=====================================\n";
            }
        }
        
        std::this_thread::sleep_for(1000ms);
        gameSeconds++;
    }
}

std::ostream &operator<<(std::ostream &os, const set_t &array)
{
    for (auto &n : array) {
        n->print();
    }
    return os;
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    set_t npcs;

    {
        std::lock_guard<std::shared_mutex> lock(npcMutex);
        std::cout << "Генерация NPC..." << std::endl;
        for (size_t i = 0; i < NPC_COUNT; ++i) {
            int type = std::rand() % 3 + 1;
            int x = std::rand() % MAP_SIZE;
            int y = std::rand() % MAP_SIZE;

            auto npc = factory(NpcType(type), x, y, i);
            if (npc) {
                npcs.insert(npc);
            }
        }
        std::cout << "Создано " << npcs.size() << " NPC." << std::endl;
    }

    std::cout << "Начало симуляции..." << std::endl;
    std::thread moveThr(moveThread, std::ref(npcs));
    std::thread battleThr(battleThread);
    std::thread printThr(printThread, std::ref(npcs));

    std::this_thread::sleep_for(std::chrono::seconds(GAME_LENGTH));

    stopFlag = true;
    gameSeconds = GAME_LENGTH;

    if (moveThr.joinable()) {
        moveThr.join();
    }
    if (battleThr.joinable()) {
        battleThr.join();
    }
    if (printThr.joinable()) {
        printThr.join();
    }

    std::cout << "\n\nСимуляция завершена. Список выживших:\n" << std::endl;

    std::shared_lock<std::shared_mutex> lock(npcMutex);
    for (const auto &npc : npcs) {
        if (npc->is_alive()) {
            std::cout << std::endl;
            npc->print();
        }
    }

    std::cout << std::endl;

    return 0;
}