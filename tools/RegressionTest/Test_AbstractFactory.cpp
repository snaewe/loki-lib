#include <iostream>
#include <memory>
#include "AbstractFactory.h"

using namespace ::Loki;


struct Enemy
{
    virtual Enemy* Clone() = 0;
    virtual ~Enemy() {};
};

struct Soldier : Enemy
{
    virtual Soldier* Clone() = 0;
    virtual const char *SoldierName() const = 0;
};

struct Monster : Enemy
{
    virtual Monster* Clone() = 0;
    virtual const char *MonsterName() const = 0;
};

struct SuperMonster : Enemy
{
    virtual SuperMonster* Clone() = 0;
    virtual const char *SuperMonsterName() const = 0;
};

typedef AbstractFactory
<
    TYPELIST_3(Soldier, Monster, SuperMonster)
>
AbstractEnemyFactory;

class SillySoldier 
    : public Soldier
{
public:
    explicit SillySoldier(const char *name = "Anonymous Silly Soldier")
        : name_(name)
    {}

    virtual SillySoldier* Clone()
    {
        return new SillySoldier("Cloned Silly Soldier");
    }

    virtual const char *SoldierName() const { return name_; }

private:
    const char *name_;
};

class SillyMonster 
    : public Monster
{
public:
    explicit SillyMonster(const char *name = "Anonymous Silly Monster")
        : name_(name)
    {}

    virtual SillyMonster* Clone()
    {
        return new SillyMonster("Cloned Silly Moster");
    }

    virtual const char *MonsterName() const { return name_; }

private:
    const char *name_;
};

class SillySuperMonster 
    : public SuperMonster
{
public:
    explicit SillySuperMonster(const char *name = "Anonymous Silly SuperMonster")
        : name_(name)
    {}

    virtual SillySuperMonster* Clone()
    {
        return new SillySuperMonster("Cloned Silly SuperMoster");
    }

    virtual const char *SuperMonsterName() const { return name_; }

private:
    const char *name_;
};

typedef ConcreteFactory
<
    AbstractEnemyFactory,
    OpNewFactoryUnit,
    TYPELIST_3(SillySoldier, SillyMonster, SillySuperMonster)
>
EasyLevelEnemyFactory;

typedef ConcreteFactory
<
    AbstractEnemyFactory,
    PrototypeFactoryUnit
>
EnemyFactory;

void print_names(AbstractEnemyFactory &abstEnemyFactory)
{
    using std::auto_ptr;

    auto_ptr<Soldier>       apSoldier(abstEnemyFactory.Create<Soldier>());
    
    std::cout << apSoldier->SoldierName() << std::endl;

    
    auto_ptr<Monster>       apMonster(abstEnemyFactory.Create<Monster>());

    std::cout << apMonster->MonsterName() << std::endl;


    auto_ptr<SuperMonster>  apSuperMonster(abstEnemyFactory.Create<SuperMonster>());

    std::cout << apSuperMonster->SuperMonsterName() << std::endl;
}

int main()
{
    EasyLevelEnemyFactory   testEasyLevelEnemyFactory;

    print_names(testEasyLevelEnemyFactory);

    EnemyFactory testEnemyFactory;

    SillySoldier        testSillySoldier;
    testEnemyFactory.SetPrototype(&testSillySoldier);

    SillyMonster        testSillyMonster;
    testEnemyFactory.SetPrototype(&testSillyMonster);

    SillySuperMonster   testSillySuperMonster;
    testEnemyFactory.SetPrototype(&testSillySuperMonster);

    print_names(testEnemyFactory);
}

