#include "monster2.hpp"


int applyStrengthsAndWeaknesses(CombatEnemy *enemy, int damage, Element element)
{
	if (enemy->getWeakness() == ELEMENT_ALL || enemy->getWeakness() == element) {
		damage *= 2;
	}
	else if (enemy->getStrength() == ELEMENT_ALL || enemy->getStrength() == element) {
		damage = -damage;
	}
	
	return damage;
}


