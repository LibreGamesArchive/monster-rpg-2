#ifndef COMBAT_ACTIONS

void getTextPos(Combatant *attacked, int *x, int *y);
void getCenter(Combatant *attacked, int *x, int *y);
void doAttack(Combatant *attacker, Combatant *attacked, bool swoosh = true);

#endif

