#include "monster2.hpp"


ItemEffect *createItemEffect(int itemIndex)
{
	std::string name = items[inventory[itemIndex].index].name;

	if (name == "Cure") {
		return new CureEffect(itemIndex, 50, "Cure1");
	}
	if (name == "Cure2") {
		return new CureEffect(itemIndex, 250, "Cure2");
	}
	if (name == "Cure3") {
		return new CureEffect(itemIndex, 2000, "Cure3");
	}
	else if (name == "Holy Water") {
		return new HolyWaterEffect(itemIndex);
	}
	else if (name == "Heal") {
		return new HealEffect(itemIndex);
	}
	else if (name == "Elixir") {
		return new ElixirEffect(itemIndex);
	}
	else if (name == "Juice") {
		return new JuiceEffect(itemIndex);
	}
	else if (name == "Meatballs") {
		return new MeatballsEffect(itemIndex);
	}

	return NULL;
}


void ItemEffect::apply(void)
{
	for (int i = 0; i < numTargets; i++) {
		use(targets[i], itemIndex, false);
	}
}



bool CureEffect::update(int step)
{
	count += step;
	if (count >= lifetime) {
		apply();
		return true;
	}

	return false;
}


void CureEffect::init(Combatant *user, Combatant **targets, int numTargets)
{
	ItemEffect::init(user, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		int amount = numTargets > 1 ? (this->amount*1.2)/numTargets : this->amount;
		g = new GenericCureEffect(user, targets[i], amount, name, true);
		lifetime = g->getLifetime();
		battle->addEntity(g);
	}
}


CureEffect::CureEffect(int itemIndex, int amount, std::string name) :
	ItemEffect(itemIndex)
{
	count = 0;
	this->amount = amount;
	this->name = name;

	type = COMBATENTITY_TYPE_FRILL;
	
	loadPlayDestroy("Cure.ogg");
}


CureEffect::~CureEffect(void)
{
}


bool HolyWaterEffect::update(int step)
{
	count += step;
	if (count >= lifetime) {
		apply();
		return true;
	}

	return false;
}


void HolyWaterEffect::init(Combatant *user, Combatant **targets, int numTargets)
{
	ItemEffect::init(user, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		g = new GenericHolyWaterEffect(user, targets[i], true);
		lifetime = g->getLifetime();
		battle->addEntity(g);
	}
}


HolyWaterEffect::HolyWaterEffect(int itemIndex) :
	ItemEffect(itemIndex)
{
	count = 0;

	type = COMBATENTITY_TYPE_FRILL;
}


HolyWaterEffect::~HolyWaterEffect(void)
{
}


bool HealEffect::update(int step)
{
	count += step;
	if (count >= getLifetime()) {
		apply();
		return true;
	}

	return false;
}


void HealEffect::init(Combatant *user, Combatant **targets, int numTargets)
{
	ItemEffect::init(user, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		g = new GenericHealEffect(user, targets[i], true);
		lifetime = g->getLifetime();
		battle->addEntity(g);
	}
}

HealEffect::HealEffect(int itemIndex) :
	ItemEffect(itemIndex)
{
	count = 0;

	type = COMBATENTITY_TYPE_FRILL;
		
	loadPlayDestroy("Heal.ogg");
}


HealEffect::~HealEffect(void)
{
}


bool ElixirEffect::update(int step)
{
	count += step;
	if (count >= getLifetime()) {
		apply();
		return true;
	}

	return false;
}


void ElixirEffect::init(Combatant *user, Combatant **targets, int numTargets)
{
	ItemEffect::init(user, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		g = new GenericElixirEffect(user, targets[i]);
		lifetime = g->getLifetime();
		battle->addEntity(g);
	}
}

ElixirEffect::ElixirEffect(int itemIndex) :
	ItemEffect(itemIndex)
{
	count = 0;

	type = COMBATENTITY_TYPE_FRILL;
}


ElixirEffect::~ElixirEffect(void)
{
}


bool JuiceEffect::update(int step)
{
	count += step;
	if (count >= lifetime) {
		apply();
		return true;
	}

	return false;
}


void JuiceEffect::init(Combatant *user, Combatant **targets, int numTargets)
{
	ItemEffect::init(user, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		int amount = 9999;
		g = new GenericCureEffect(user, targets[i], amount, "Cure1");
		lifetime = g->getLifetime();
		battle->addEntity(g);
	}
}


JuiceEffect::JuiceEffect(int itemIndex) :
	ItemEffect(itemIndex)
{
	count = 0;

	type = COMBATENTITY_TYPE_FRILL;
}


JuiceEffect::~JuiceEffect(void)
{
}



bool MeatballsEffect::update(int step)
{
	count += step;
	if (count >= lifetime) {
		apply();
		return true;
	}

	return false;
}


void MeatballsEffect::init(Combatant *user, Combatant **targets, int numTargets)
{
	ItemEffect::init(user, targets, numTargets);

	lifetime = 200;
}


MeatballsEffect::MeatballsEffect(int itemIndex) :
	ItemEffect(itemIndex)
{
	count = 0;

	type = COMBATENTITY_TYPE_FRILL;
}


MeatballsEffect::~MeatballsEffect(void)
{
	inventory[itemIndex].quantity--;
	if (inventory[itemIndex].quantity <= 0) {
		inventory[itemIndex].index = -1;
	}
}

