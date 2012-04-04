#include "monster2.hpp"


int Combatant::getDirection(void)
{
	return direction;
}


AnimationSet *Combatant::getAnimationSet(void)
{
	return animSet;
}


AnimationSet *Combatant::getWhiteAnimationSet(void)
{
	return whiteAnimSet;
}


CombatLocation Combatant::getLocation(void)
{
	return location;
}


CombatLoyalty Combatant::getLoyalty(void)
{
	return loyalty;
}


std::string Combatant::getName(void)
{
	return name;
}


std::string Combatant::getPrintableName(void)
{
	return printableName;
}


void Combatant::setDirection(Direction dir)
{
	direction = dir;
}


void Combatant::setInfo(CombatantInfo &newinfo)
{
	copyInfo(info, newinfo);
}


void Combatant::setLocation(CombatLocation l)
{
	location = l;
}


void Combatant::setLoyalty(CombatLoyalty l)
{
	loyalty = l;
}


bool Combatant::isDefending(void)
{
	return defending;
}


void Combatant::setDefending(bool def)
{
	defending = def;
}


void Combatant::construct(std::string name, bool alpha)
{
	this->name = name;

	/* Chop of everything from '_' to the end of the string
	 * so we can share animations between "different"
	 * enemies. Keep the original in tact.
	 */
	std::string s = std::string(_t(name.c_str()));
	std::string::size_type loc = s.find("_", 0);
	s = s.substr(0, loc);
	printableName = s;

	s = name;
	loc = s.find("_", 0);
	s = s.substr(0, loc);

	s = std::string(getResource("combat_media/")) + s + ".png";

	referenceBattleAnim(name);
	animSet = findBattleAnim(name);

	animSet->setSubAnimation("stand");

	defending = false;
}

Combatant::Combatant(std::string name, bool alpha) :
	CombatEntity()
{
	construct(name, alpha);
}


Combatant::Combatant(std::string name) :
	CombatEntity()
{
	construct(name, false);
}


Combatant::~Combatant(void)
{
	unreferenceBattleAnim(name);
}

