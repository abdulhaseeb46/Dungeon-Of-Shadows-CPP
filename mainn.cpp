/* ============================================================
   DUNGEON OF SHADOWS —  Visual Edition
      C++ Console RPG with Colors, ASCII Art & Health Bars
	    Built using OOP: Encapsulation, Inheritance,
		                Abstraction, Polymorphism
   ============================================================ */

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <windows.h>   // For console colors (Windows only)
#include <conio.h>     // For _getch() — press any key to continue
using namespace std;

// ============================================================
//  CONSOLE VISUAL HELPERS
//  These functions control colors and visual effects
// ============================================================

// Color codes for Windows console
// Each number = a different color
#define BLACK        0
#define DARK_BLUE    1
#define DARK_GREEN   2
#define DARK_CYAN    3
#define DARK_RED     4
#define DARK_MAGENTA 5
#define DARK_YELLOW  6
#define GRAY         7
#define DARK_GRAY    8
#define BLUE         9
#define GREEN        10
#define CYAN         11
#define RED          12
#define MAGENTA      13
#define YELLOW       14
#define WHITE        15

// Get the handle to the console window
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

// setColor(text, background) — changes the console text color
// Example: setColor(YELLOW, BLACK) makes yellow text on black
void setColor(int text, int bg = BLACK) {
    SetConsoleTextAttribute(hConsole, (bg << 4) | text);
}

// Reset to normal white text
void resetColor() { setColor(WHITE, BLACK); }

// clearScreen() — clears the console (like cls in cmd)
void clearScreen() { system("cls"); }

// pause() — waits for any key press
void pause() {
    setColor(DARK_GRAY);
    cout << "\n  Press any key to continue...";
    resetColor();
    _getch();  // waits for a key without showing it
}

// slowPrint() — prints text letter by letter for dramatic effect
void slowPrint(const string& text, int delayMs = 30) {
    for (char c : text) {
        cout << c;
        cout.flush();          // force the character to show immediately
        Sleep(delayMs);        // wait delayMs milliseconds
    }
    cout << "\n";
}

// drawHealthBar() — draws a visual HP bar like: [########  ] 80/100
void drawHealthBar(const string& label, int current, int max, int color) {
    int barWidth  = 20;
    // filled = how many # blocks to show
    int filled    = (int)((float)current / max * barWidth);
    if (filled < 0) filled = 0;
    int empty     = barWidth - filled;

    setColor(WHITE);
    cout << "  " << label << " ";

    // Draw the filled part in the given color
    setColor(color, BLACK);
    cout << "[";
    for (int i = 0; i < filled; i++) cout << "#";

    // Draw the empty part in dark gray
    setColor(DARK_GRAY, BLACK);
    for (int i = 0; i < empty;  i++) cout << "-";

    setColor(color, BLACK);
    cout << "] ";

    setColor(WHITE);
    cout << current << "/" << max << "\n";
    resetColor();
}

// drawBox() — draws a box border with a title
void drawBox(const string& title, int width = 50) {
    setColor(CYAN);
    cout << "\n  +";
    for (int i = 0; i < width - 4; i++) cout << "-";
    cout << "+\n";

    // Center the title inside the box
    int padding = (width - 4 - (int)title.size()) / 2;
    cout << "  |";
    for (int i = 0; i < padding; i++) cout << " ";
    setColor(YELLOW);
    cout << title;
    setColor(CYAN);
    int rightPad = width - 4 - padding - (int)title.size();
    for (int i = 0; i < rightPad; i++) cout << " ";
    cout << "|\n";

    cout << "  +";
    for (int i = 0; i < width - 4; i++) cout << "-";
    cout << "+\n";
    resetColor();
}

// ============================================================
//  ASCII ART — Characters & Enemies
// ============================================================

void printWarriorArt() {
    setColor(DARK_MAGENTA);
    cout << "        O\n";
    cout << "       /|\\\n";
    cout << "       / \\\n";
    cout << "      [===]\n";
    resetColor();
}

void printMageArt() {
    setColor(MAGENTA);
    cout << "        *\n";
    cout << "       /O\\\n";
    cout << "        |\n";
    cout << "       /|\\\n";
    resetColor();
}

void printArcherArt() {
    setColor(GREEN);
    cout << "        O\n";
    cout << "       \\|/-->\n";
    cout << "        |\n";
    cout << "       / \\\n";
    resetColor();
}

void printRogueArt() {
    setColor(DARK_CYAN);
    cout << "       (O)\n";
    cout << "       /|\\\n";
    cout << "      / | \\\n";
    cout << "        |\n";
    resetColor();
}

void printGoblinArt() {
    setColor(GREEN);
    cout << "       ,_,\n";
    cout << "      (o o)\n";
    cout << "     __|_|__\n";
    cout << "      |   |\n";
    resetColor();
}

void printSkeletonArt() {
    setColor(GRAY);
    cout << "       ___\n";
    cout << "      (x x)\n";
    cout << "      --|--\n";
    cout << "      /   \\\n";
    resetColor();
}

void printOrcArt() {
    setColor(DARK_GREEN);
    cout << "      _____\n";
    cout << "     (> O <)\n";
    cout << "     /|___|  \\\n";
    cout << "    /        \\\n";
    resetColor();
}

void printBossArt() {
    setColor(RED);
    cout << "    /\\_____/\\\n";
    cout << "   ( >X   X< )\n";
    cout << "    \\  ___  /\n";
    cout << "   __|_| |_|__\n";
    cout << "  /  MALACHAR  \\\n";
    resetColor();
}

// ============================================================
//  WEAPON CLASS
// ============================================================
class Weapon {
private:
    string name;
    int    damage;
    string type;
public:
    Weapon() : name("Fists"), damage(0), type("Melee") {}
    Weapon(string n, int d, string t) : name(n), damage(d), type(t) {}

    string getName()   const { return name; }
    int    getDamage() const { return damage; }
    string getType()   const { return type; }

    void displayInfo() const {
        setColor(YELLOW); cout << "    [" << type << "] ";
        setColor(WHITE);  cout << name;
        setColor(GREEN);  cout << " => +" << damage << " dmg\n";
        resetColor();
    }
};

// ============================================================
//  INVENTORY CLASS
// ============================================================
class Inventory {
private:
    vector<Weapon> weapons;
    int maxSlots;
public:
    Inventory(int slots = 5) : maxSlots(slots) {}

    bool addWeapon(const Weapon& w) {
        if ((int)weapons.size() >= maxSlots) return false;
        weapons.push_back(w);
        return true;
    }

    void display() const {
        if (weapons.empty()) {
            setColor(DARK_GRAY); cout << "    (empty)\n"; resetColor();
            return;
        }
        for (int i = 0; i < (int)weapons.size(); i++) {
            setColor(CYAN); cout << "    " << i+1 << ". ";
            resetColor();
            weapons[i].displayInfo();
        }
    }

    Weapon getWeapon(int i) const { return weapons[i]; }
    int    getCount()       const { return (int)weapons.size(); }
};

// ============================================================
//  CHARACTER BASE CLASS  (Abstract)
// ============================================================
class Character {
protected:
    string    name;
    int       health, maxHealth, baseAttack, defense, level, experience;
    Weapon    equippedWeapon;
    Inventory inventory;

public:
    Character(string n, int hp, int atk, int def)
        : name(n), health(hp), maxHealth(hp),
          baseAttack(atk), defense(def), level(1), experience(0) {}

     virtual ~Character() {}

    // Pure virtual — EVERY subclass must define its own attack
    // This is ABSTRACTION + POLYMORPHISM
    virtual int    attack()          = 0;
    virtual void   displayStats()    const = 0;
    virtual void   printArt()        const = 0;
    virtual string getClassName()    const = 0;
    virtual int    getColorCode()    const = 0;

    void takeDamage(int dmg) {
        int actual = dmg - defense;
        if (actual < 1) actual = 1;
        health -= actual;
        if (health < 0) health = 0;
    }

    void heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
    }

    bool isAlive()    const { return health > 0; }
    string getName()  const { return name; }
    int getHealth()   const { return health; }
    int getMaxHealth()const { return maxHealth; }
    int getDefense()  const { return defense; }
    int getLevel()    const { return level; }
    int getExperience()const{ return experience; }
    Inventory& getInventory(){ return inventory; }

    void equipWeapon(const Weapon& w) {
        equippedWeapon = w;
        setColor(GREEN);
        cout << "  >> " << name << " equipped " << w.getName() << "!\n";
        resetColor();
    }

    void addToInventory(const Weapon& w) { inventory.addWeapon(w); }

    void showInventory() const {
        drawBox(name + "'s Inventory");
        inventory.display();
    }

    void gainExperience(int xp) {
        experience += xp;
        setColor(YELLOW);
        cout << "  >> " << name << " gained " << xp << " XP!\n";
        resetColor();
        if (experience >= level * 100) levelUp();
    }

    void levelUp() {
        level++;
        maxHealth += 20; health = maxHealth;
        baseAttack += 5; defense += 2;
        setColor(YELLOW);
        cout << "\n  *** LEVEL UP! " << name
             << " is now Level " << level << "! ***\n";
        resetColor();
    }

    string serialize() const {
        ostringstream o;
        o << name <<" "<< health <<" "<< maxHealth <<" "
          << baseAttack <<" "<< defense <<" "<< level <<" "<< experience;
        return o.str();
    }

    void deserialize(int hp,int mhp,int atk,int def,int lvl,int xp){
        health=hp; maxHealth=mhp; baseAttack=atk;
        defense=def; level=lvl; experience=xp;
    }
};

// ============================================================
//  WARRIOR
// ============================================================
class Warrior : public Character {
private:
    int rage;
public:
    Warrior(string name) : Character(name, 130, 18, 10), rage(0) {
        addToInventory(Weapon("Iron Sword",  8,  "Melee"));
        addToInventory(Weapon("Battle Axe",  14, "Melee"));
        addToInventory(Weapon("War Hammer",  18, "Melee"));
        equipWeapon(Weapon("Iron Sword", 8, "Melee"));
    }

    int attack() override {
        rage++;
        int dmg = baseAttack + equippedWeapon.getDamage();
        if (rage % 3 == 0) {
            dmg *= 2;
            setColor(YELLOW);
            cout << "\n  *** POWER STRIKE! *** ";
            setColor(RED);
            cout << name << " deals " << dmg << " damage!\n";
        } else {
            setColor(WHITE);
            cout << "  Warrior " << name << " slashes for ";
            setColor(RED); cout << dmg; setColor(WHITE); cout << " damage.\n";
        }
        resetColor();
        return dmg;
    }

    void displayStats() const override {
        drawBox("WARRIOR — " + name);
        printArt();
        setColor(WHITE);
        cout << "  Level   : "; setColor(YELLOW); cout << level << "\n";
        drawHealthBar("HP     ", health, maxHealth, GREEN);
        setColor(WHITE);
        cout << "  Attack  : "; setColor(RED);    cout << baseAttack << "\n";
        cout << "  Defense : "; setColor(CYAN);   cout << defense    << "\n";
        cout << "  Weapon  : "; setColor(YELLOW); cout << equippedWeapon.getName() << "\n";
        cout << "  Rage    : "; setColor(RED);    cout << rage << " (Power Strike every 3)\n";
        resetColor();
    }

    void   printArt()     const override { printWarriorArt(); }
    string getClassName() const override { return "Warrior"; }
    int    getColorCode() const override { return YELLOW; }
};

// ============================================================
//  MAGE
// ============================================================
class Mage : public Character {
private:
    int mana, maxMana;
public:
    Mage(string name) : Character(name, 80, 25, 3), mana(100), maxMana(100) {
        addToInventory(Weapon("Wooden Staff",  6,  "Magic"));
        addToInventory(Weapon("Crystal Wand",  12, "Magic"));
        addToInventory(Weapon("Arcane Tome",   18, "Magic"));
        equipWeapon(Weapon("Wooden Staff", 6, "Magic"));
    }

    int attack() override {
        if (mana >= 20) {
            mana -= 20;
            int dmg = baseAttack + equippedWeapon.getDamage() + 10;
            setColor(MAGENTA);
            cout << "\n  *** FIREBALL! *** ";
            setColor(RED);
            cout << name << " deals " << dmg << " damage!";
            setColor(BLUE);
            cout << " (Mana: " << mana << "/" << maxMana << ")\n";
            resetColor();
            return dmg;
        }
        int dmg = max(5, baseAttack / 2);
        setColor(DARK_GRAY);
        cout << "  Out of mana! Weak hit for " << dmg << " damage.\n";
        resetColor();
        return dmg;
    }

    void displayStats() const override {
        drawBox("MAGE — " + name);
        printArt();
        setColor(WHITE);
        cout << "  Level   : "; setColor(YELLOW); cout << level << "\n";
        drawHealthBar("HP     ", health, maxHealth, GREEN);
        drawHealthBar("Mana   ", mana,   maxMana,   BLUE);
        setColor(WHITE);
        cout << "  Attack  : "; setColor(RED);     cout << baseAttack << "\n";
        cout << "  Defense : "; setColor(CYAN);    cout << defense    << "\n";
        cout << "  Weapon  : "; setColor(YELLOW);  cout << equippedWeapon.getName() << "\n";
        resetColor();
    }

    void   printArt()     const override { printMageArt(); }
    string getClassName() const override { return "Mage"; }
    int    getColorCode() const override { return MAGENTA; }
};

// ============================================================
//  ARCHER
// ============================================================
class Archer : public Character {
private:
    int critChance;
public:
    Archer(string name) : Character(name, 100, 20, 6), critChance(30) {
        addToInventory(Weapon("Short Bow",   7,  "Ranged"));
        addToInventory(Weapon("Long Bow",    13, "Ranged"));
        addToInventory(Weapon("Hunter Bow",  18, "Ranged"));
        equipWeapon(Weapon("Short Bow", 7, "Ranged"));
    }

    int attack() override {
        int dmg = baseAttack + equippedWeapon.getDamage();
        if ((rand() % 100) < critChance) {
            dmg *= 2;
            setColor(GREEN);
            cout << "\n  *** CRITICAL HIT! *** ";
            setColor(RED);
            cout << name << " fires for " << dmg << " damage!\n";
        } else {
            setColor(GREEN);
            cout << "  " << name << " shoots for ";
            setColor(RED); cout << dmg; setColor(GREEN); cout << " damage.\n";
        }
        resetColor();
        return dmg;
    }

    void displayStats() const override {
        drawBox("ARCHER — " + name);
        printArt();
        setColor(WHITE);
        cout << "  Level   : "; setColor(YELLOW); cout << level << "\n";
        drawHealthBar("HP     ", health, maxHealth, GREEN);
        setColor(WHITE);
        cout << "  Attack  : "; setColor(RED);   cout << baseAttack << "\n";
        cout << "  Defense : "; setColor(CYAN);  cout << defense    << "\n";
        cout << "  Weapon  : "; setColor(YELLOW);cout << equippedWeapon.getName() << "\n";
        cout << "  Crit%   : "; setColor(GREEN); cout << critChance << "%\n";
        resetColor();
    }

    void   printArt()     const override { printArcherArt(); }
    string getClassName() const override { return "Archer"; }
    int    getColorCode() const override { return GREEN; }
};

// ============================================================
//  ROGUE
// ============================================================
class Rogue : public Character {
private:
    int stealthBonus;
public:
    Rogue(string name) : Character(name, 90, 15, 4), stealthBonus(10) {
        addToInventory(Weapon("Rusty Dagger",  5,  "Melee"));
        addToInventory(Weapon("Shadow Blade",  11, "Melee"));
        addToInventory(Weapon("Poison Knife",  15, "Melee"));
        equipWeapon(Weapon("Rusty Dagger", 5, "Melee"));
    }

    int attack() override {
        int hit1  = baseAttack + equippedWeapon.getDamage();
        int hit2  = (baseAttack / 2) + stealthBonus;
        int total = hit1 + hit2;
        setColor(CYAN);
        cout << "  " << name << " DUAL STRIKES! (";
        setColor(RED); cout << hit1;
        setColor(CYAN); cout << " + ";
        setColor(RED); cout << hit2;
        setColor(CYAN); cout << ") = ";
        setColor(RED); cout << total;
        setColor(CYAN); cout << " damage!\n";
        resetColor();
        return total;
    }

    void displayStats() const override {
        drawBox("ROGUE — " + name);
        printArt();
        setColor(WHITE);
        cout << "  Level   : "; setColor(YELLOW); cout << level << "\n";
        drawHealthBar("HP     ", health, maxHealth, GREEN);
        setColor(WHITE);
        cout << "  Attack  : "; setColor(RED);    cout << baseAttack << "\n";
        cout << "  Defense : "; setColor(CYAN);   cout << defense    << "\n";
        cout << "  Weapon  : "; setColor(YELLOW); cout << equippedWeapon.getName() << "\n";
        cout << "  Stealth : "; setColor(CYAN);   cout << "+" << stealthBonus << " on 2nd hit\n";
        resetColor();
    }

    void   printArt()     const override { printRogueArt(); }
    string getClassName() const override { return "Rogue"; }
    int    getColorCode() const override { return CYAN; }
};

// ============================================================
//  ENEMY BASE CLASS
// ============================================================
class Enemy {
protected:
    string name;
    int health, maxHealth, attackPower, defense, xpReward;
    string type;
public:
    Enemy(string n,int hp,int atk,int def,int xp,string t="Normal")
        : name(n),health(hp),maxHealth(hp),
          attackPower(atk),defense(def),xpReward(xp),type(t) {}

    virtual ~Enemy() {}
    virtual int  attack()            = 0;
    virtual void printArt()          const = 0;
    virtual int  getColorCode()      const = 0;

    virtual void displayStats() const {
        setColor(getColorCode());
        cout << "  [" << type << "] " << name;
        resetColor();
        setColor(WHITE); cout << " | ";
        drawHealthBar("HP", health, maxHealth, RED);
        setColor(WHITE);
        cout << "  ATK: "; setColor(RED); cout << attackPower;
        setColor(WHITE); cout << " | DEF: "; setColor(CYAN); cout << defense << "\n";
        resetColor();
    }

    virtual void takeDamage(int dmg) {
        int actual = dmg - defense;
        if (actual < 1) actual = 1;
        health -= actual;
        if (health < 0) health = 0;
    }

    bool   isAlive()     const { return health > 0; }
    string getName()     const { return name; }
    int    getHealth()   const { return health; }
    int    getMaxHealth()const { return maxHealth; }
    int    getXPReward() const { return xpReward; }
    string getType()     const { return type; }
};

// ---- Goblin ------------------------------------------------
class Goblin : public Enemy {
public:
    Goblin() : Enemy("Goblin", 40, 10, 1, 30) {}
    int attack() override {
        int dmg = attackPower + (rand() % 6);
        setColor(GREEN);
        cout << "  Goblin scratches for ";
        setColor(RED); cout << dmg; setColor(GREEN); cout << " damage!\n";
        resetColor();
        return dmg;
    }
    void printArt()     const override { printGoblinArt(); }
    int  getColorCode() const override { return GREEN; }
};

// ---- Skeleton ----------------------------------------------
class Skeleton : public Enemy {
private:
    bool shield; int turn;
public:
    Skeleton() : Enemy("Skeleton", 60, 14, 3, 50), shield(true), turn(0) {}

    int attack() override {
        turn++;
        int dmg = attackPower + (turn % 2 == 0 ? 8 : 0);
        setColor(GRAY);
        if (turn % 2 == 0) {
            cout << "  Skeleton RATTLES bones and hits for ";
        } else {
            cout << "  Skeleton attacks for ";
        }
        setColor(RED); cout << dmg; setColor(GRAY); cout << " damage!\n";
        resetColor();
        return dmg;
    }

    void takeDamage(int dmg) override {
        if (shield) {
            dmg /= 2;
            setColor(GRAY);
            cout << "  Bone Shield blocked half the damage!\n";
            resetColor();
        }
        shield = !shield;
        Enemy::takeDamage(dmg);
    }

    void printArt()     const override { printSkeletonArt(); }
    int  getColorCode() const override { return GRAY; }
};

// ---- Orc ---------------------------------------------------
class Orc : public Enemy {
public:
    Orc() : Enemy("Orc", 80, 20, 5, 70) {}
    int attack() override {
        int dmg = attackPower + (rand() % 8);
        setColor(DARK_GREEN);
        cout << "  Orc SLAMS for ";
        setColor(RED); cout << dmg; setColor(DARK_GREEN); cout << " damage!\n";
        resetColor();
        return dmg;
    }
    void printArt()     const override { printOrcArt(); }
    int  getColorCode() const override { return DARK_GREEN; }
};

// ---- Dark Lord Final BOSS ----------------------------------------
class DarkLord : public Enemy {
private:
    int phase, turn;
public:
    DarkLord() : Enemy("Dark Lord Malachar",200,28,8,300,"BOSS"),
                 phase(1), turn(0) {}

    int attack() override {
        turn++;
        int dmg = attackPower + (phase==2 ? (turn%3==0 ? 20 : 10) : 0);
        setColor(RED);
        if (phase==2 && turn%3==0)
            cout << "  DARK LORD casts DEATH CURSE for ";
        else if (phase==2)
            cout << "  DARK LORD unleashes DARK ENERGY for ";
        else
            cout << "  DARK LORD strikes for ";
        setColor(YELLOW); cout << dmg;
        setColor(RED); cout << " damage!!\n";
        resetColor();
        return dmg;
    }

    void takeDamage(int dmg) override {
        Enemy::takeDamage(dmg);
        if (phase==1 && health <= maxHealth/2) {
            phase=2; attackPower+=10;
            setColor(RED);
            slowPrint("\n  !!! DARK LORD ENTERS PHASE 2 !!!", 40);
            slowPrint("  He absorbs dark energy — ATTACK INCREASED!!\n", 40);
            resetColor();
        }
    }

    void displayStats() const override {
        setColor(RED);
        cout << "  [BOSS] " << name;
        resetColor();
        cout << "\n";
        drawHealthBar("HP    ", health, maxHealth, RED);
        setColor(WHITE); cout << "  Phase: ";
        setColor(RED);   cout << phase << "\n";
        resetColor();
    }

    void printArt()     const override { printBossArt(); }
    int  getColorCode() const override { return RED; }
};

// ============================================================
//  GAME CLASS
// ============================================================
class Game {
private:
    Character* player;
    int  currentLevel;
    bool gameOver;

    // ---- Title Screen ----
    void showTitle() {
        clearScreen();
        setColor(GREEN);
        cout << "\n";
        cout << "  ██████╗ ██╗   ██╗███╗   ██╗ ██████╗ ███████╗ ██████╗ ███╗   ██╗\n";
        cout << "  ██╔══██╗██║   ██║████╗  ██║██╔════╝ ██╔════╝██╔═══██╗████╗  ██║\n";
        cout << "  ██║  ██║██║   ██║██╔██╗ ██║██║  ███╗█████╗  ██║   ██║██╔██╗ ██║\n";
        cout << "  ██║  ██║██║   ██║██║╚██╗██║██║   ██║██╔══╝  ██║   ██║██║╚██╗██║\n";
        cout << "  ██████╔╝╚██████╔╝██║ ╚████║╚██████╔╝███████╗╚██████╔╝██║ ╚████║\n";
        cout << "  ╚═════╝  ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚══════╝ ╚═════╝ ╚═╝  ╚═══╝\n";
        setColor(DARK_YELLOW);
        cout << "\n              O F   S H A D O W S\n";
        setColor(DARK_GRAY);
        cout << "\n         A C++ RPG — OOP Semester Project\n";
        setColor(CYAN);
        cout << "    Encapsulation | Inheritance | Abstraction | Polymorphism\n";
        resetColor();
        cout << "\n";
    }

    // ---- Character Select ----
    Character* selectCharacter() {
        clearScreen();
        drawBox("CHOOSE YOUR CLASS", 54);

        // Warrior
        setColor(YELLOW); cout << "\n  [1] WARRIOR\n";
        printWarriorArt();
        setColor(WHITE);
        cout << "      HP: 130 | ATK: 18 | DEF: 10\n";
        cout << "      Power Strike every 3rd attack\n";

        // Mage
        setColor(MAGENTA); cout << "\n  [2] MAGE\n";
        printMageArt();
        setColor(WHITE);
        cout << "      HP:  80 | ATK: 25 | DEF: 3\n";
        cout << "      Fireball spell using Mana\n";

        // Archer
        setColor(GREEN); cout << "\n  [3] ARCHER\n";
        printArcherArt();
        setColor(WHITE);
        cout << "      HP: 100 | ATK: 20 | DEF: 6\n";
        cout << "      30% Critical Hit chance\n";

        // Rogue
        setColor(CYAN); cout << "\n  [4] ROGUE\n";
        printRogueArt();
        setColor(WHITE);
        cout << "      HP:  90 | ATK: 15 | DEF: 4\n";
        cout << "      Dual Strike — attacks twice\n";

        cout << "\n  Enter choice (1-4): ";
        int c; cin >> c;

        cout << "  Enter your hero's name: ";
        string n; cin >> n;

        // POLYMORPHISM — player pointer holds any derived class
        switch(c) {
            case 1: return new Warrior(n);
            case 2: return new Mage(n);
            case 3: return new Archer(n);
            case 4: return new Rogue(n);
            default:
                setColor(DARK_GRAY);
                cout << "  Invalid — defaulting to Warrior.\n";
                resetColor();
                return new Warrior(n);
        }
    }

    // ---- Combat Status Bar ----
    void showCombatStatus(Enemy* e) const {
        cout << "\n";
        setColor(player->getColorCode());
        cout << "  " << player->getName();
        resetColor();
        drawHealthBar(" ", player->getHealth(), player->getMaxHealth(), GREEN);

        setColor(e->getColorCode());
        cout << "  " << e->getName();
        resetColor();
        drawHealthBar(" ", e->getHealth(), e->getMaxHealth(), RED);
        cout << "\n";
    }

    // ---- Weapon Swap ----
    void weaponMenu() {
        player->showInventory();
        cout << "\n  Enter weapon number (0 = cancel): ";
        int idx; cin >> idx;
        if (idx > 0 && idx <= player->getInventory().getCount())
            player->equipWeapon(player->getInventory().getWeapon(idx-1));
    }

    // ---- Combat ----
    bool runCombat(Enemy* enemy) {
        clearScreen();

        // Show both fighters side by side
        setColor(player->getColorCode());
        cout << "\n      HERO                    ENEMY\n";
        resetColor();
        player->printArt();
        enemy->printArt();

        setColor(YELLOW);
        slowPrint("\n  A wild " + enemy->getName() + " appears!", 35);
        resetColor();
        enemy->displayStats();
        pause();

        while (player->isAlive() && enemy->isAlive()) {
            clearScreen();
            showCombatStatus(enemy);

            setColor(CYAN);
            cout << "  What will " << player->getName() << " do?\n\n";
            setColor(WHITE);
            cout << "  [1] Attack\n";
            cout << "  [2] Swap Weapon\n";
            cout << "  [3] View Stats\n\n";
            cout << "  Choice: ";
            resetColor();

            int a; cin >> a;
            cout << "\n";

            if (a == 1) {
                // POLYMORPHISM — calls the right attack() automatically
                int dmg = player->attack();
                enemy->takeDamage(dmg);
                Sleep(500);
                if (!enemy->isAlive()) break;
                int edmg = enemy->attack();
                player->takeDamage(edmg);
                Sleep(500);
            } else if (a == 2) {
                weaponMenu();
            } else if (a == 3) {
                player->displayStats();
                pause();
            }
        }

        if (player->isAlive()) {
            setColor(GREEN);
            slowPrint("\n  >> VICTORY! " + player->getName() +
                      " defeated " + enemy->getName() + "!", 30);
            resetColor();
            player->gainExperience(enemy->getXPReward());
            pause();
            return true;
        }
        return false;
    }

    // ---- Save & Load ----
    void saveGame() const {
        ofstream f("savegame.txt");
        if (!f) { setColor(RED); cout << "  ERROR: Could not save.\n"; resetColor(); return; }
        f << player->getClassName() << "\n"
          << player->serialize()    << "\n"
          << currentLevel           << "\n";
        f.close();
        setColor(GREEN); cout << "  >> Game saved successfully!\n"; resetColor();
    }

    bool loadGame() {
        ifstream f("savegame.txt");
        if (!f) { setColor(RED); cout << "  No save file found.\n"; resetColor(); return false; }
        string cls, name;
        int hp,mhp,atk,def,lvl,xp;
        f >> cls >> name >> hp >> mhp >> atk >> def >> lvl >> xp >> currentLevel;
        f.close();
        delete player;
        if      (cls=="Warrior") player = new Warrior(name);
        else if (cls=="Mage")    player = new Mage(name);
        else if (cls=="Archer")  player = new Archer(name);
        else                     player = new Rogue(name);
        player->deserialize(hp,mhp,atk,def,lvl,xp);
        setColor(GREEN);
        cout << "  >> Loaded! Welcome back, " << name << "!\n";
        resetColor();
        return true;
    }

    // ---- Level ----
    void runLevel(int lvl) {
        clearScreen();
        setColor(CYAN);
        slowPrint("\n  === DUNGEON LEVEL " + to_string(lvl) + " ===", 40);
        resetColor();
        Sleep(500);

        for (int i = 0; i < 2; i++) {
            Enemy* e;
            int r = rand() % 3;
            if      (r==0) e = new Goblin();
            else if (r==1) e = new Skeleton();
            else           e = new Orc();

            bool won = runCombat(e);
            delete e;
            if (!won) { gameOver = true; return; }

            setColor(YELLOW);
            cout << "\n  Save game? (1=Yes / 2=No): ";
            resetColor();
            int s; cin >> s;
            if (s==1) saveGame();
        }
        currentLevel++;
    }

    void runBoss() {
        clearScreen();
        setColor(RED);
        slowPrint("\n  ================================================", 10);
        slowPrint("           FINAL BOSS — DARK LORD MALACHAR", 40);
        slowPrint("  ================================================", 10);
        resetColor();
        Sleep(600);
        printBossArt();
        setColor(RED);
        slowPrint("\n  The dungeon trembles...", 40);
        slowPrint("  A dark presence rises from the shadows...", 40);
        slowPrint("  DARK LORD MALACHAR awakens!!", 50);
        resetColor();
        pause();

        DarkLord* boss = new DarkLord();
        bool won = runCombat(boss);
        delete boss;
        if (!won) gameOver = true;
    }

public:
    Game() : player(nullptr), currentLevel(1), gameOver(false) {
        srand((unsigned int)time(0));
    }
    ~Game() { delete player; }

    void run() {
        showTitle();

        setColor(WHITE);
        cout << "  [1] New Game\n";
        cout << "  [2] Load Game\n\n";
        cout << "  Choice: ";
        int c; cin >> c;

        if (c==2 && loadGame()) {
            setColor(CYAN);
            cout << "  Resuming from Level " << currentLevel << "\n";
            resetColor();
            pause();
        } else {
            player = selectCharacter();
            clearScreen();
            setColor(GREEN);
            slowPrint("\n  Welcome to the dungeon, " + player->getName() + "!", 35);
            resetColor();
            player->displayStats();
            pause();
        }

        while (!gameOver && currentLevel <= 3)
            runLevel(currentLevel);

        if (!gameOver) runBoss();

        clearScreen();
        if (gameOver) {
            setColor(RED);
            cout << "\n\n";
            slowPrint("  ================================================", 10);
            slowPrint("                   GAME OVER", 60);
            slowPrint("  " + player->getName() + " has fallen in the dungeon.", 40);
            slowPrint("  The darkness claims another soul...", 40);
            slowPrint("  ================================================", 10);
        } else {
            setColor(YELLOW);
            cout << "\n\n";
            slowPrint("  ================================================", 10);
            slowPrint("                   VICTORY!", 60);
            slowPrint("  " + player->getName() + " defeated the Dark Lord!", 40);
            slowPrint("  The dungeon is saved. You are a legend.", 40);
            slowPrint("  ================================================", 10);
            resetColor();
            player->displayStats();
        }
        resetColor();
        pause();
    }
};

// ============================================================
//  MAIN — 3 lines. Everything is inside Game class.
// ============================================================
int main() {
    // Set console to support special characters
    SetConsoleOutputCP(CP_UTF8);
    system("color 11");   // default white text black background

    Game game;
    game.run();
    return 0;
}
