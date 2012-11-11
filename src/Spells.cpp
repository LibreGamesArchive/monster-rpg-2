#include "monster2.hpp"


typedef Spell *(*SpellFactory)(void);

std::map<std::string, SpellFactory> spellFactory;


static Spell *Bolt1Factory(void)
{
	loadPlayDestroy("bolt.ogg");
	return new LightningSpell();
}

static Spell *Bolt2Factory(void)
{
	return new Bolt2Spell();
}

static Spell *Bolt3Factory(void)
{
	return new Bolt3Spell();
}

static Spell *Ice1Factory(void)
{
	loadPlayDestroy("ice1.ogg");
	return new Ice1Spell();
}

static Spell *Ice2Factory(void)
{
	loadPlayDestroy("freeze.ogg");
	return new Ice2Spell();
}

static Spell *Ice3Factory(void)
{
	return new Ice3Spell();
}

static Spell *Fire1Factory(void)
{
	loadPlayDestroy("fire1.ogg");
	return new Fire1Spell();
}

static Spell *Fire2Factory(void)
{
	playPreloadedSample("Fire2.ogg");
	return new Fire2Spell();
}

static Spell *Fire3Factory(void)
{
	return new Fire3Spell();
}

static Spell *Darkness1Factory(void)
{
	return new Darkness1Spell();
}

static Spell *Darkness2Factory(void)
{
	return new Darkness2Spell();
}

static Spell *WeepFactory(void)
{
	return new WeepSpell();
}

static Spell *WaveFactory(void)
{
	return new WaveSpell();
}

static Spell *WhirlpoolFactory(void)
{
	return new WhirlpoolSpell();
}

static Spell *Cure1Factory(void)
{
	return new CureSpell(50, "Cure1");
}

static Spell *Cure2Factory(void)
{
	return new CureSpell(250, "Cure2");
}

static Spell *Cure3Factory(void)
{
	return new CureSpell(2000, "Cure3");
}

static Spell *HealFactory(void)
{
	return new HealSpell();
}

static Spell *StoneFactory(void)
{
	return new StoneSpell();
}

static Spell *SludgeFactory(void)
{
	return new SludgeSpell();
}

static Spell *RendFactory(void)
{
	loadPlayDestroy("Rend.ogg");
	return new RendSpell(170);
}

static Spell *StompFactory(void)
{
	loadPlayDestroy("Stomp.ogg");
	return new StompSpell();
}

static Spell *SprayFactory(void)
{
	return new SpraySpell();
}

static Spell *ReviveFactory(void)
{
	return new ReviveSpell();
}

static Spell *StunFactory(void)
{
	return new StunSpell();
}

static Spell *SlowFactory(void)
{
	return new SlowSpell();
}

static Spell *QuickFactory(void)
{
	return new QuickSpell();
}

static Spell *AcidFactory(void)
{
	return new AcidSpell(165);
}

static Spell *BellyAcidFactory(void)
{
	return new AcidSpell(770);
}

static Spell *PunchFactory(void)
{
	return new PunchSpell(200);
}

static Spell *TorrentFactory(void)
{
	return new TorrentSpell();
}

static Spell *BeamFactory(void)
{
	return new BeamSpell();
}

static Spell *BlazeFactory(void)
{
	return new BlazeSpell();
}

static Spell *CharmFactory(void)
{
	return new CharmSpell();
}

static Spell *TwisterFactory(void)
{
	return new TwisterSpell();
}

static Spell *WhipFactory(void)
{
	return new WhipSpell();
}

static Spell *AcornsFactory(void)
{
	return new AcornsSpell();
}

static Spell *DaisyFactory(void)
{
	return new DaisySpell();
}

static Spell *VampireFactory(void)
{
	return new VampireSpell();
}

static Spell *ArcFactory(void)
{
	return new ArcSpell();
}

static Spell *BananaFactory(void)
{
	return new BananaSpell("Banana", 350);
}

static Spell *FireballFactory(void)
{
	return new FireballSpell();
}

static Spell *KissofDeathFactory(void)
{
	loadPlayDestroy("high_cackle.ogg");
	return new KissOfDeathSpell();
}

static Spell *TouchofDeathFactory(void)
{
	loadPlayDestroy("TouchofDeath.ogg");
	return new KissOfDeathSpell();
}

static Spell *BreathofFireFactory(void)
{
	return new BoFSpell();
}

static Spell *TalonFactory(void)
{
	loadPlayDestroy("Rend.ogg");
	return new RendSpell(1000);
}


static Spell *ClawFactory(void)
{
	loadPlayDestroy("Meow.ogg");
	return new RendSpell(500);
}

static Spell *BoulderFactory(void)
{
	return new BananaSpell("boulder", 400);
}


static Spell *DropFactory(void)
{
	return new DropSpell();
}

static Spell *MachineGunFactory(void)
{
	return new MachineGunSpell();
}


static Spell *LaserFactory(void)
{
	return new LaserSpell();
}


static Spell *UFOFactory(void)
{
	return new UFOSpell();
}


static Spell *OrbitFactory(void)
{
	return new OrbitSpell();
}


static Spell *WebFactory(void)
{
	return new WebSpell();
}


static Spell *Darkness3Factory(void)
{
	return new Darkness3Spell();
}


static Spell *SwallowFactory(void)
{
	return new SwallowSpell();
}

static Spell *PukeFactory(void)
{
	return new PukeSpell();
}


Spell *createSpell(std::string name)
{
	static bool inited = false;

	if (!inited) {
		inited = true;
		spellFactory["Bolt1"] = Bolt1Factory;
		spellFactory["Bolt2"] = Bolt2Factory;
		spellFactory["Bolt3"] = Bolt3Factory;
		spellFactory["Ice1"] = Ice1Factory;
		spellFactory["Ice2"] = Ice2Factory;
		spellFactory["Ice3"] = Ice3Factory;
		spellFactory["Fire1"] = Fire1Factory;
		spellFactory["Fire2"] = Fire2Factory;
		spellFactory["Fire3"] = Fire3Factory;
		spellFactory["Darkness1"] = Darkness1Factory;
		spellFactory["Darkness2"] = Darkness2Factory;
		spellFactory["Darkness3"] = Darkness3Factory;
		spellFactory["Weep"] = WeepFactory;
		spellFactory["Wave"] = WaveFactory;
		spellFactory["Whirlpool"] = WhirlpoolFactory;
		spellFactory["Cure1"] = Cure1Factory;
		spellFactory["Cure2"] = Cure2Factory;
		spellFactory["Cure3"] = Cure3Factory;
		spellFactory["Heal"] = HealFactory;
		spellFactory["Stone"] = StoneFactory;
		spellFactory["Sludge"] = SludgeFactory;
		spellFactory["Rend"] = RendFactory;
		spellFactory["Stomp"] = StompFactory;
		spellFactory["Spray"] = SprayFactory;
		spellFactory["Revive"] = ReviveFactory;
		spellFactory["Stun"] = StunFactory;
		spellFactory["Slow"] = SlowFactory;
		spellFactory["Quick"] = QuickFactory;
		spellFactory["Acid"] = AcidFactory;
		spellFactory["BellyAcid"] = BellyAcidFactory;
		spellFactory["Punch"] = PunchFactory;
		spellFactory["Torrent"] = TorrentFactory;
		spellFactory["Beam"] = BeamFactory;
		spellFactory["Charm"] = CharmFactory;
		spellFactory["Twister"] = TwisterFactory;
		spellFactory["Whip"] = WhipFactory;
		spellFactory["Acorns"] = AcornsFactory;
		spellFactory["Daisy"] = DaisyFactory;
		spellFactory["Vampire"] = VampireFactory;
		spellFactory["Arc"] = ArcFactory;
		spellFactory["Banana"] = BananaFactory;
		spellFactory["Fireball"] = FireballFactory;
		spellFactory["Kiss of Death"] = KissofDeathFactory;
		spellFactory["Touch of Death"] = TouchofDeathFactory;
		spellFactory["Breath of Fire"] = BreathofFireFactory;
		spellFactory["Talon"] = TalonFactory;
		spellFactory["Claw"] = ClawFactory;
		spellFactory["Boulder"] = BoulderFactory;
		spellFactory["Drop"] = DropFactory;
		spellFactory["Blaze"] = BlazeFactory;
		spellFactory["MachineGun"] = MachineGunFactory;
		spellFactory["Laser"] = LaserFactory;
		spellFactory["UFO"] = UFOFactory;
		spellFactory["Orbit"] = OrbitFactory;
		spellFactory["Web"] = WebFactory;
		spellFactory["Swallow"] = SwallowFactory;
		spellFactory["Puke"] = PukeFactory;
	}

	return (*spellFactory[name])();
}



SpellAlignment getSpellAlignment(std::string name)
{
	// FIXME when adding white magic
	if (name == "Cure1" || name == "Heal" || name == "Cure2"
		|| name == "Cure3" || name == "Quick" || name == "Revive")
		return SPELL_WHITE;
	return SPELL_BLACK;
}


int getSpellCost(std::string name)
{
	if (name == "Bolt1")
		return 3;
	else if (name == "Bolt2")
		return 8;
	else if (name == "Bolt3")
		return 20;
	else if (name == "Fire1")
		return 3;
	else if (name == "Fire2")
		return 8;
	else if (name == "Fire3")
		return 20;
	else if (name == "Ice1")
		return 3;
	else if (name == "Ice2")
		return 8;
	else if (name == "Ice3")
		return 20;
	else if (name == "Darkness1") {
		return 4;
	}
	else if (name == "Weep") {
		return 4;
	}
	else if (name == "Cure1") {
		return 3;
	}
	else if (name == "Heal") {
		return 5;
	}
	else if (name == "Cure2") {
		return 6;
	}
	else if (name == "Cure3") {
		return 15;
	}
	else if (name == "Revive") {
		return 16;
	}
	else if (name == "Stun") {
		return 10;
	}
	else if (name == "Slow") {
		return 10;
	}
	else if (name == "Quick") {
		return 10;
	}
	else if (name == "Torrent") {
		return 15;
	}
	else if (name == "Charm") {
		return 10;
	}
	else if (name == "Vampire") {
		return 10;
	}

	return 1;
}


static void applyAttackSpell(int damage, Combatant *caster,
	Combatant **targets, int numTargets, Element element)
{
	if (!(battle->getName() == "first_battle")) {
		if (config.getDifficulty() == CFG_DIFFICULTY_EASY) {
			if (caster->getType() == COMBATENTITY_TYPE_PLAYER) {
				damage *= 1.15;
			}
			else {
				damage -= damage * 0.15;
			}
		}
		else if (config.getDifficulty() == CFG_DIFFICULTY_HARD) {
			if (caster->getType() == COMBATENTITY_TYPE_PLAYER) {
				damage -= damage * 0.15;
			}
			else {
				damage *= 1.15;
			}
		}
	}

	if (superpower) {
		CombatPlayer *p = dynamic_cast<CombatPlayer *>(caster);
		if (p) {
			damage = 9999;
		}
	}

	int work;

	for (int i = 0; i < numTargets; i++) {
		Combatant *c = targets[i];

		if (superpower) {
			CombatPlayer *p = dynamic_cast<CombatPlayer *>(c);
			if (p) {
				damage = 1;
			}
		}

		work = damage;

		CombatantInfo &info = c->getInfo();
		int mdefense = info.abilities.mdefense;
		// Could hit in the head, chest, or legs
		int r = rand() % 10;
		if (r < 5) {
			mdefense += 
				info.equipment.carmor < 0 ? 0 :
				armorMagicDefense(ITEM_TYPE_CHEST_ARMOR, items[info.equipment.carmor].id);
		}
		else if (r < 8) {
			mdefense += 
				info.equipment.harmor < 0 ? 0 :
				armorMagicDefense(ITEM_TYPE_HEAD_ARMOR, items[info.equipment.harmor].id);
		}
		else {
			mdefense += 
				info.equipment.farmor < 0 ? 0 :
				armorMagicDefense(ITEM_TYPE_FEET_ARMOR, items[info.equipment.farmor].id);
		}


		work -= mdefense;

		// If attacking multiple people, damage is less per person
		// but higher overall
		if (numTargets > 1) {
			work += (int)(work * 0.25f);
			work /= numTargets;
		}

		if (work <= 0)
			work = 1;

		if (element != ELEMENT_NONE && c->getType() == COMBATENTITY_TYPE_ENEMY) {
			work = applyStrengthsAndWeaknesses((CombatEnemy *)c,
				work,
				element);
		}

		if (!(battle->getName() == "first_battle")) {
			int sign = work < 0 ? -1 : 1;
			if (c->isDefending()) {
				work /= 2;
				if (work == 0) work = sign;
				c->setDefending(false);
			}
		}
		else {
			work = 50;
		}

		info.abilities.hp -= work;
		if (info.abilities.hp > info.abilities.maxhp)
			info.abilities.hp = info.abilities.maxhp;

		if (info.abilities.hp <= 0 &&
				c->getType() == COMBATENTITY_TYPE_ENEMY) {
			((CombatEnemy *)c)->die(caster->getId());
		}

		int x;
		int y;

		getTextPos(c, &x, &y);

		char s[20];

		sprintf(s, "%d", abs(work));

		MCOLOR col;

		if (work < 0)
			col = m_map_rgb(0, 255, 0);
		else
			col = white;

		TemporaryText *tt = new TemporaryText(x, y, std::string(s),
			col);

		battle->addEntity(tt);
	}
}


void applyWhiteMagicSpell(std::string name, CombatantInfo &info, int numTargets, bool sound, Combatant *target, int text_x, int text_y)
{
	if (name == "Revive" && target && (target->getName() == "Zombie" || target->getName() == "Mummy")) {
		if (sound)
			loadPlayDestroy("Revive.ogg");
		info.abilities.hp -= 1000;
		return;
	}

	if (info.abilities.hp <= 0 && name != "Revive") {
		if (name == "Cure1" || name == "Cure2" || name == "Cure3") {
			if (sound) {
				loadPlayDestroy("Cure.ogg");
			}
		}
		else if (name == "Heal") {
			if (sound)
				loadPlayDestroy("Heal.ogg");
		}
		return;
	}
	
	if (name == "Cure1" || name == "Cure2" || name == "Cure3") {
		int amount = 0;
		if (name == "Cure1") {
			amount = 50;
		}
		else if (name == "Cure2") {
			amount = 250;
		}
		else if (name == "Cure3") {
			amount = 2000;
		}
		if (numTargets > 1) {
			amount = ceil((amount*1.2)/numTargets);
		}
                if (target && (target->getName() == "Zombie" || target->getName() == "Mummy")) {
                	amount = -amount;
                }
		info.abilities.hp += amount;
		if (info.abilities.hp > info.abilities.maxhp) {
			if (text_x != -1 || text_y != -1) {
				amount -= (info.abilities.hp - info.abilities.maxhp);
			}
			info.abilities.hp = info.abilities.maxhp;
		}
		if (sound) {
			loadPlayDestroy("Cure.ogg");
		}
		if (text_x != -1 || text_y != -1) {
			char text[100];
			sprintf(text, "%d", amount);
			addOmnipotentText(std::string(text), text_x, text_y, m_map_rgb(0, 255, 0));
		}
	}
	else if (name == "Heal") {
		if (target && target->getType() == COMBATENTITY_TYPE_PLAYER) {
			if (target->getAnimationSet()->getSubName() != "cast") {
				target->getAnimationSet()->setSubAnimation("stand");
			}
		}
		info.condition = CONDITION_NORMAL;
		if (sound)
			loadPlayDestroy("Heal.ogg");
		if (text_x != -1 || text_y != -1) {
			char text[100];
			strcpy(text, "HEAL!");
			addOmnipotentText(std::string(text), text_x, text_y, m_map_rgb(0, 255, 0));
		}
	}
	else if (name == "Revive") {
		int maxhp = info.abilities.maxhp;
		int hp = info.abilities.hp;
		if (text_x != -1 || text_y != -1) {
			char text[100];
			if (hp <= 0) {
				strcpy(text, "LIFE!");
			}
			else {
				sprintf(text, "%d", maxhp-hp);
			}
			addOmnipotentText(std::string(text), text_x, text_y, m_map_rgb(0, 255, 0));
		}
		if (info.abilities.hp <= 0) {
			info.abilities.hp = info.abilities.maxhp * 0.2f;
		}
		else {
			info.abilities.hp = info.abilities.maxhp;
		}
		if (sound)
			loadPlayDestroy("Revive.ogg");
	}
}



static int targetsContain(Combatant **targets, int numTargets, std::string name)
{
	for (int i = 0; i < numTargets; i++) {
		if (targets[i]->getName() == name) {
			return i;
		}
	}

	return -1;
}


void Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	this->caster = caster;
	this->targets = targets;
	this->numTargets = numTargets;
}


Spell::~Spell(void)
{
	delete[] targets;
}



bool LightningSpell::update(int step)
{
	static bool bounced_back = false;

	count += step;

	if (count >= lifetime) {
		if (!bounced_back && targetsContain(targets, numTargets, "Gator") != -1) {
			loadPlayDestroy("bolt.ogg");
			count = 0;
			bounced_back = true;
			caster->getAnimationSet()->setSubAnimation("hit");
			LightningEffect *le = new LightningEffect(caster);
			battle->addEntity(le);
			return false;
		}
		else {
			bounced_back = false;
			return true;
		}
	}

	return false;
}


void LightningSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	ALLEGRO_DEBUG("in LightningSpell::init");
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		LightningEffect *le = new LightningEffect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
	ALLEGRO_DEBUG("end of LightningSpell::init");
}


void LightningSpell::apply(void)
{
	int i;

	if ((i = targetsContain(targets, numTargets, "Gator")) != -1) {
		Combatant *newTargets[1] = { caster };
		applyAttackSpell(55, targets[i], newTargets, 1, ELEMENT_ELECTRICITY);
	}
	else {
		applyAttackSpell(55, caster, targets, numTargets, ELEMENT_ELECTRICITY);
	}
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


LightningSpell::LightningSpell(void) :
	count(0),
	lifetime(0)
{
}


LightningSpell::~LightningSpell(void)
{
}


bool Bolt2Spell::update(int step)
{
	static bool bounced_back = false;

	count += step;

	if (count >= lifetime) {
		if (!bounced_back && targetsContain(targets, numTargets, "Gator") != -1) {
			loadPlayDestroy("bolt.ogg");
			count = 0;
			bounced_back = true;
			caster->getAnimationSet()->setSubAnimation("hit");
			LightningEffect *le = new LightningEffect(caster);
			battle->addEntity(le);
			return false;
		}
		else {
			bounced_back = false;
			return true;
		}
	}

	return false;
}


void Bolt2Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		Bolt2Effect *le = new Bolt2Effect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
	
	loadPlayDestroy("Bolt2.ogg");
}


void Bolt2Spell::apply(void)
{
	int i;

	if ((i = targetsContain(targets, numTargets, "Gator")) != -1) {
		Combatant *newTargets[1] = { caster };
		applyAttackSpell(250, targets[i], newTargets, 1, ELEMENT_ELECTRICITY);
	}
	else {
		applyAttackSpell(250, caster, targets, numTargets, ELEMENT_ELECTRICITY);
	}
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Bolt2Spell::Bolt2Spell(void) :
	count(0),
	lifetime(0)
{
}


Bolt2Spell::~Bolt2Spell(void)
{
}


bool Ice1Spell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void Ice1Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		Ice1Effect *le = new Ice1Effect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
}


void Ice1Spell::apply(void)
{
	applyAttackSpell(55, caster, targets, numTargets, ELEMENT_ICE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Ice1Spell::Ice1Spell(void) :
	count(0),
	lifetime(0)
{
}


Ice1Spell::~Ice1Spell(void)
{
}


bool Ice2Spell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void Ice2Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		Ice2Effect *le = new Ice2Effect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
}


void Ice2Spell::apply(void)
{
	applyAttackSpell(250, caster, targets, numTargets, ELEMENT_ICE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Ice2Spell::Ice2Spell(void) :
	count(0),
	lifetime(0)
{
}


Ice2Spell::~Ice2Spell(void)
{
}



bool Fire2Spell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void Fire2Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		Fire2Effect *le = new Fire2Effect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
}


void Fire2Spell::apply(void)
{
	applyAttackSpell(250, caster, targets, numTargets, ELEMENT_FIRE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Fire2Spell::Fire2Spell(void) :
	count(0),
	lifetime(0)
{
}


Fire2Spell::~Fire2Spell(void)
{
}


bool Fire1Spell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void Fire1Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		Fire1Effect *le = new Fire1Effect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
}


void Fire1Spell::apply(void)
{
	applyAttackSpell(55, caster, targets, numTargets, ELEMENT_FIRE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Fire1Spell::Fire1Spell(void) :
	count(0),
	lifetime(0)
{
}


Fire1Spell::~Fire1Spell(void)
{
}

bool Darkness1Spell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void Darkness1Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		Darkness1Effect *le = new Darkness1Effect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
}


void Darkness1Spell::apply(void)
{
	applyAttackSpell(40, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Darkness1Spell::Darkness1Spell(void) :
	count(0),
	lifetime(0)
{
}


Darkness1Spell::~Darkness1Spell(void)
{
}


bool WeepSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void WeepSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		WeepEffect *le = new WeepEffect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
}


void WeepSpell::apply(void)
{
	applyAttackSpell(42, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


WeepSpell::WeepSpell(void) :
	count(0),
	lifetime(0)
{
}


WeepSpell::~WeepSpell(void)
{
}




bool WaveSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void WaveSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		WaveEffect *le = new WaveEffect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
	loadPlayDestroy("Wave.ogg");
}


void WaveSpell::apply(void)
{
	applyAttackSpell(50, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


WaveSpell::WaveSpell(void) :
	count(0),
	lifetime(0)
{
}


WaveSpell::~WaveSpell(void)
{
}


bool WhirlpoolSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void WhirlpoolSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		WhirlpoolEffect *le = new WhirlpoolEffect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
	loadPlayDestroy("Whirlpool.ogg");
}


void WhirlpoolSpell::apply(void)
{
	applyAttackSpell(55, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


WhirlpoolSpell::WhirlpoolSpell(void) :
	count(0),
	lifetime(0)
{
}


WhirlpoolSpell::~WhirlpoolSpell(void)
{
}


bool CureSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void CureSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Cure.ogg");

	for (int i = 0; i < numTargets; i++) {
		int a = numTargets > 1 ? (amount*1.2)/numTargets : amount;
		g = new GenericCureEffect(caster, targets[i], a, name);
		lifetime = g->getLifetime();
		battle->addEntity(g);
	}
}


void CureSpell::apply(void)
{
	for (int i = 0; i < numTargets; i++) {
		g->finalize(targets[i]);
		applyWhiteMagicSpell(name, targets[i]->getInfo(), numTargets, false, targets[i]);
	}
}


CureSpell::CureSpell(int amount, std::string name) :
	count(0),
	lifetime(0)
{
	this->amount = amount;
	this->name = name;
}


CureSpell::~CureSpell(void)
{
}


bool HealSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void HealSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Heal.ogg");

	for (int i = 0; i < numTargets; i++) {
		g = new GenericHealEffect(caster, targets[i]);
		lifetime = g->getLifetime();
		battle->addEntity(g);
	}
}


void HealSpell::apply(void)
{
	for (int i = 0; i < numTargets; i++) {
		g->finalize(targets[i]);
		applyWhiteMagicSpell("Heal", targets[i]->getInfo(), numTargets, false, targets[i]);
	}
}


HealSpell::HealSpell(void) :
	count(0),
	lifetime(0)
{
}


HealSpell::~HealSpell(void)
{
}




bool StoneSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void StoneSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Stone.ogg");
}


void StoneSpell::apply(void)
{
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getInfo().condition = CONDITION_STONED;
	}
}


StoneSpell::StoneSpell(void) :
	count(0),
	lifetime(500)
{
}


StoneSpell::~StoneSpell(void)
{
}




bool SludgeSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void SludgeSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("woosh.ogg");

	SludgeFlyEffect *se = NULL;
	
	for (int i = 0; i < numTargets; i++) {
		se = new SludgeFlyEffect(targets[i], caster, "SludgeBall");
		battle->addEntity(se);
	}

	if (se)
		lifetime = se->getLifetime();
	else
		lifetime = 1;
}


void SludgeSpell::apply(void)
{
	applyAttackSpell(100, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		SludgeEffect *se = new SludgeEffect(targets[i], m_map_rgb(0, 0, 255));
		battle->addEntity(se);
	}
}


SludgeSpell::SludgeSpell(void) :
	count(0)
{
}


SludgeSpell::~SludgeSpell(void)
{
}




bool RendSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void RendSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	RendEffect *se = NULL;
	
	for (int i = 0; i < numTargets; i++) {
		se = new RendEffect(targets[i]);
		battle->addEntity(se);
	}

	if (se)
		lifetime = se->getLifetime();
	else
		lifetime = 1;
}


void RendSpell::apply(void)
{
	applyAttackSpell(damage, caster, targets, numTargets, ELEMENT_NONE);
}


RendSpell::RendSpell(int damage) :
	count(0)
{
	this->damage = damage;
}


RendSpell::~RendSpell(void)
{
}



bool StompSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void StompSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	StompEffect *se = new StompEffect(targets[0]->getLocation());
	battle->addEntity(se);
	
	lifetime = se->getLifetime();
}


void StompSpell::apply(void)
{
	applyAttackSpell(250, caster, targets, numTargets, ELEMENT_NONE);
}


StompSpell::StompSpell(void) :
	count(0)
{
}


StompSpell::~StompSpell(void)
{
}




bool SpraySpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void SpraySpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	SprayEffect *se = NULL;
	
	for (int i = 0; i < numTargets; i++) {
		se = new SprayEffect(targets[i]);
		battle->addEntity(se);
	}

	if (se)
		lifetime = se->getLifetime();
	else
		lifetime = 1;
	
	caster->getAnimationSet()->setSubAnimation("spray");
}


void SpraySpell::apply(void)
{
	caster->getAnimationSet()->setSubAnimation("stand");
	applyAttackSpell(175, caster, targets, numTargets, ELEMENT_NONE);
}


SpraySpell::SpraySpell(void) :
	count(0)
{
}


SpraySpell::~SpraySpell(void)
{
}


bool ReviveSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void ReviveSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("HolyWater.ogg");

	for (int i = 0; i < numTargets; i++) {
		g = new GenericHolyWaterEffect(caster, targets[i]);
		lifetime = g->getLifetime();
		battle->addEntity(g);
	}
}


void ReviveSpell::apply(void)
{
	for (int i = 0; i < numTargets; i++) {
		g->finalize(targets[i]);
		applyWhiteMagicSpell("Revive", targets[i]->getInfo(), numTargets, false, targets[i]);
	}
}


ReviveSpell::ReviveSpell(void) :
	count(0),
	lifetime(0)
{
}


ReviveSpell::~ReviveSpell(void)
{
}


bool StunSpell::update(int step)
{
	count += step;
	if (count > 2000)
		return true;
	return false;
}

void StunSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Stun.ogg");
}

void StunSpell::apply(void)
{
	if (targets[0]->getType() == COMBATENTITY_TYPE_ENEMY) {
		CombatEnemy *e = (CombatEnemy *)targets[0];
		if (e->spellHasNoEffect("Stun")) {
			return;
		}
	}
	targets[0]->getInfo().condition = CONDITION_PARALYZED;
	targets[0]->getInfo().paralyzeCount = 3;
}


StunSpell::StunSpell(void) :
	count(0)
{
}


StunSpell::~StunSpell(void)
{
}


bool SlowSpell::update(int step)
{
	count += step;
	if (count > 2000)
		return true;
	return false;
}


void SlowSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Slow.ogg");
}


void SlowSpell::apply(void)
{
	if (targets[0]->getType() == COMBATENTITY_TYPE_ENEMY) {
		CombatEnemy *e = (CombatEnemy *)targets[0];
		if (e->spellHasNoEffect("Slow")) {
			return;
		}
	}
	targets[0]->getInfo().condition = CONDITION_SLOW;
	targets[0]->getInfo().missed_extra = false;
}


SlowSpell::SlowSpell(void) :
	count(0)
{
}


SlowSpell::~SlowSpell(void)
{
}


bool QuickSpell::update(int step)
{
	count += step;
	if (count > 2000)
		return true;
	return false;
}


void QuickSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Quick.ogg");
}


void QuickSpell::apply(void)
{
	targets[0]->getInfo().condition = CONDITION_QUICK;
	targets[0]->getInfo().missed_extra = false;
	if (targets[0]->getType() == COMBATENTITY_TYPE_PLAYER) {
		if (caster != targets[0]) {
			targets[0]->getAnimationSet()->setSubAnimation("stand");
		}
	}
}


QuickSpell::QuickSpell(void) :
	count(0)
{
}


QuickSpell::~QuickSpell(void)
{
}


bool AcidSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void AcidSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("woosh.ogg");

	SludgeFlyEffect *se = NULL;
	
	for (int i = 0; i < numTargets; i++) {
		se = new SludgeFlyEffect(targets[i], caster, "AcidBall");
		battle->addEntity(se);
	}

	if (se)
		lifetime = se->getLifetime();
	else
		lifetime = 1;
}


void AcidSpell::apply(void)
{
	applyAttackSpell(damage, caster, targets, numTargets, ELEMENT_ELECTRICITY);

	for (int i = 0; i < numTargets; i++) {
		SludgeEffect *se = new SludgeEffect(targets[i], m_map_rgb(0, 255, 0));
		battle->addEntity(se);
	}
}


AcidSpell::AcidSpell(int damage) :
	count(0)
{
	this->damage = damage;
}


AcidSpell::~AcidSpell(void)
{
}


bool PunchSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void PunchSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	caster->getAnimationSet()->setSubAnimation("punching");

	loadPlayDestroy("woosh.ogg");

	PunchEffect *se = NULL;
	
	for (int i = 0; i < numTargets; i++) {
		se = new PunchEffect(targets[i], caster);
		battle->addEntity(se);
	}

	if (se)
		lifetime = se->getLifetime();
	else
		lifetime = 1;
}


void PunchSpell::apply(void)
{
	caster->getAnimationSet()->setSubAnimation("normal");
	loadPlayDestroy("Punch.ogg");
	applyAttackSpell(damage, caster, targets, numTargets, ELEMENT_NONE);
}


PunchSpell::PunchSpell(int damage) :
	count(0),
	damage(damage)
{
}


PunchSpell::~PunchSpell(void)
{
}



bool TorrentSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void TorrentSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Weep.ogg");

	TorrentEffect *se = NULL;
	
	se = new TorrentEffect(caster);
	battle->addEntity(se);

	if (se)
		lifetime = se->getLifetime();
	else
		lifetime = 1;
}


void TorrentSpell::apply(void)
{
	applyAttackSpell(300, caster, targets, numTargets, ELEMENT_NONE);
}


TorrentSpell::TorrentSpell(void) :
	count(0)
{
}


TorrentSpell::~TorrentSpell(void)
{
}


bool BeamSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void BeamSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Beam.ogg");

	CombatEntity *se = NULL;
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		se = new BeamP1Effect(targets[i], caster);
		battle->addEntity(se);
		se = new BeamP2Effect(targets[i], caster);
		battle->addEntity(se);
	}

	if (se)
		lifetime = se->getLifetime();
	else
		lifetime = 1;
}


void BeamSpell::apply(void)
{
	applyAttackSpell(9999, caster, targets, numTargets, ELEMENT_NONE);
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


BeamSpell::BeamSpell(void) :
	count(0)
{
}


BeamSpell::~BeamSpell(void)
{
}


bool Fire3Spell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void Fire3Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Fire3.ogg");

	Fire3Effect *se = NULL;
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		se = new Fire3Effect(targets[i]);
		battle->addEntity(se);
	}

	if (se)
		lifetime = se->getLifetime();
	else
		lifetime = 1;
}


void Fire3Spell::apply(void)
{
	applyAttackSpell(1000, caster, targets, numTargets, ELEMENT_FIRE);
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Fire3Spell::Fire3Spell(void) :
	count(0)
{
}


Fire3Spell::~Fire3Spell(void)
{
}


bool Ice3Spell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void Ice3Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Ice3.ogg");

	Ice3Effect *se = NULL;
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		se = new Ice3Effect(targets[i]);
		battle->addEntity(se);
	}

	if (se)
		lifetime = se->getLifetime();
	else
		lifetime = 1;
}


void Ice3Spell::apply(void)
{
	applyAttackSpell(1000, caster, targets, numTargets, ELEMENT_ICE);
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Ice3Spell::Ice3Spell(void) :
	count(0)
{
}


Ice3Spell::~Ice3Spell(void)
{
}


bool Bolt3Spell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void Bolt3Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	playPreloadedSample("Bolt3.ogg");

	Bolt3Effect *se = NULL;
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		se = new Bolt3Effect(targets[i]);
		battle->addEntity(se);
	}

	if (se)
		lifetime = se->getLifetime();
	else
		lifetime = 1;
}


void Bolt3Spell::apply(void)
{
	applyAttackSpell(1000, caster, targets, numTargets, ELEMENT_ELECTRICITY);
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Bolt3Spell::Bolt3Spell(void) :
	count(0)
{
}


Bolt3Spell::~Bolt3Spell(void)
{
}


bool CharmSpell::update(int step)
{
	count += step;
	if (count > 2000)
		return true;
	return false;
}


void CharmSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Charm.ogg");
}


void CharmSpell::apply(void)
{
	if (targets[0]->getType() == COMBATENTITY_TYPE_ENEMY) {
		CombatEnemy *e = (CombatEnemy *)targets[0];
		if (e->spellHasNoEffect("Charm")) {
			return;
		}
	}
	if (targets[0]->getName() == "Minion" || targets[0]->getType() == COMBATENTITY_TYPE_PLAYER) {
		targets[0]->getInfo().condition = CONDITION_CHARMED;
		targets[0]->getInfo().charmedCount = 3;
	}
}


CharmSpell::CharmSpell(void) :
	count(0)
{
}


CharmSpell::~CharmSpell(void)
{
}


bool Darkness2Spell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void Darkness2Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
	}

	CombatLocation loc;
	if (caster->getLocation() == LOCATION_LEFT)
		loc = LOCATION_RIGHT;
	else
		loc = LOCATION_LEFT;

	Darkness2Effect *le = new Darkness2Effect(loc);
	lifetime = le->getLifetime();
	battle->addEntity(le);
}


void Darkness2Spell::apply(void)
{
	applyAttackSpell(600, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Darkness2Spell::Darkness2Spell(void) :
	count(0),
	lifetime(0)
{
}


Darkness2Spell::~Darkness2Spell(void)
{
}


bool TwisterSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void TwisterSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		TwisterEffect *le = new TwisterEffect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}

	loadPlayDestroy("Twister.ogg");
}


void TwisterSpell::apply(void)
{
	applyAttackSpell(275, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


TwisterSpell::TwisterSpell(void) :
	count(0),
	lifetime(0)
{
}


TwisterSpell::~TwisterSpell(void)
{
}


bool WhipSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void WhipSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		WhipEffect *le = new WhipEffect(caster, targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}

	loadPlayDestroy("Whip.ogg");
}


void WhipSpell::apply(void)
{
	applyAttackSpell(400, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


WhipSpell::WhipSpell(void) :
	count(0),
	lifetime(0)
{
}


WhipSpell::~WhipSpell(void)
{
}


bool AcornsSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void AcornsSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		AcornsEffect *le = new AcornsEffect(caster, targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}

	loadPlayDestroy("Acorns.ogg");
}


void AcornsSpell::apply(void)
{
	applyAttackSpell(550, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


AcornsSpell::AcornsSpell(void) :
	count(0),
	lifetime(0)
{
}


AcornsSpell::~AcornsSpell(void)
{
}


bool DaisySpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void DaisySpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		DaisyEffect *le = new DaisyEffect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}

	loadPlayDestroy("Daisy.ogg");
}


void DaisySpell::apply(void)
{
	applyAttackSpell(320, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


DaisySpell::DaisySpell(void) :
	count(0),
	lifetime(0)
{
}


DaisySpell::~DaisySpell(void)
{
}



bool VampireSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void VampireSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		VampireEffect *le = new VampireEffect(targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}

	loadPlayDestroy("Vampire.ogg");
}


void VampireSpell::apply(void)
{
	int total = 0;

	for (int i = 0; i < numTargets; i++) {
		int amount;
		int hp = targets[i]->getInfo().abilities.hp;
		if (hp < 50)
			amount = hp;
		else
			amount = 50;
		total += amount;
		targets[i]->getInfo().abilities.hp -= amount;
		targets[i]->getAnimationSet()->setSubAnimation("stand");
		
		if (targets[i]->getInfo().abilities.hp <= 0 &&
				targets[i]->getType() == COMBATENTITY_TYPE_ENEMY) {
			((CombatEnemy *)targets[i])->die(caster->getId());
		}

		int x;
		int y;

		getTextPos(targets[i], &x, &y);

		char s[20];

		sprintf(s, "%d", MIN(hp, amount));

		TemporaryText *tt = new TemporaryText(x, y, std::string(s),
			white);

		battle->addEntity(tt);
	}
	
	int x;
	int y;
		
	getTextPos(caster, &x, &y);

	char s[20];

	sprintf(s, "%d", total);

	TemporaryText *tt = new TemporaryText(x, y, std::string(s),
		m_map_rgb(0, 255, 0));

	battle->addEntity(tt);

	caster->getInfo().abilities.hp += total;
	if (caster->getType() == COMBATENTITY_TYPE_PLAYER) {
		if (caster->getInfo().abilities.hp > caster->getInfo().abilities.maxhp) {
			caster->getInfo().abilities.hp = caster->getInfo().abilities.maxhp;
		}
	}
}


VampireSpell::VampireSpell(void) :
	count(0),
	lifetime(0)
{
}


VampireSpell::~VampireSpell(void)
{
}


bool ArcSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void ArcSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		ArcEffect *le = new ArcEffect(caster, targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}

	loadPlayDestroy("Arc.ogg");
}


void ArcSpell::apply(void)
{
	applyAttackSpell(400, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


ArcSpell::ArcSpell(void) :
	count(0),
	lifetime(0)
{
}


ArcSpell::~ArcSpell(void)
{
}


bool BananaSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void BananaSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		BananaEffect *le = new BananaEffect(caster, targets[i], imageName);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}

	loadPlayDestroy("Banana.ogg");
}


void BananaSpell::apply(void)
{
	applyAttackSpell(damage, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


BananaSpell::BananaSpell(std::string img, int dmg) :
	count(0),
	lifetime(0),
	imageName(img),
	damage(dmg)
{
}


BananaSpell::~BananaSpell(void)
{
}


bool FireballSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void FireballSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		FireballEffect *le = new FireballEffect(caster, targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}

	loadPlayDestroy("Fireball.ogg");
}


void FireballSpell::apply(void)
{
	applyAttackSpell(450, caster, targets, numTargets, ELEMENT_FIRE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


FireballSpell::FireballSpell(void) :
	count(0),
	lifetime(0)
{
}


FireballSpell::~FireballSpell(void)
{
}


bool KissOfDeathSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void KissOfDeathSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		KissOfDeathEffect *le = new KissOfDeathEffect(caster, targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}
}


void KissOfDeathSpell::apply(void)
{
	applyAttackSpell(1000, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


KissOfDeathSpell::KissOfDeathSpell(void) :
	count(0),
	lifetime(0)
{
}


KissOfDeathSpell::~KissOfDeathSpell(void)
{
}


bool BoFSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void BoFSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		BoFEffect *le = new BoFEffect(caster, targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}

	loadPlayDestroy("BoF.ogg");
}


void BoFSpell::apply(void)
{
	applyAttackSpell(1000, caster, targets, numTargets, ELEMENT_FIRE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


BoFSpell::BoFSpell(void) :
	count(0),
	lifetime(0)
{
}


BoFSpell::~BoFSpell(void)
{
}


bool DropSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void DropSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
		DropEffect *le = new DropEffect(caster, targets[i]);
		lifetime = le->getLifetime();
		battle->addEntity(le);
	}

	loadPlayDestroy("jump.ogg");
}


void DropSpell::apply(void)
{
	applyAttackSpell(432, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


DropSpell::DropSpell(void) :
	count(0),
	lifetime(0)
{
}


DropSpell::~DropSpell(void)
{
}


bool BlazeSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void BlazeSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Beam.ogg");

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");
	}

	BlazeEffect *be = new BlazeEffect(caster);
	battle->addEntity(be);

	lifetime = be->getLifetime();
}


void BlazeSpell::apply(void)
{
	applyAttackSpell(400, caster, targets, numTargets, ELEMENT_FIRE);
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


BlazeSpell::BlazeSpell(void) :
	count(0)
{
}


BlazeSpell::~BlazeSpell(void)
{
}


bool MachineGunSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void MachineGunSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Machine_Gun.ogg");

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");

		MachineGunEffect *be = new MachineGunEffect(caster, targets[i]);
		battle->addEntity(be);

		lifetime = be->getLifetime();
	}
}


void MachineGunSpell::apply(void)
{
	for (int i = 0; i < numTargets; i++) {
		doAttack(caster, targets[i], false);
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
	
	caster->getAnimationSet()->setSubAnimation("stand");
}


MachineGunSpell::MachineGunSpell(void) :
	count(0)
{
}


MachineGunSpell::~MachineGunSpell(void)
{
}


bool LaserSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void LaserSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Laser.ogg");

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");

		LaserEffect *be = new LaserEffect(caster, targets[i]);
		battle->addEntity(be);

		lifetime = be->getLifetime();
	}
}


void LaserSpell::apply(void)
{
	applyAttackSpell(400, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


LaserSpell::LaserSpell(void) :
	count(0)
{
}


LaserSpell::~LaserSpell(void)
{
}


bool UFOSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void UFOSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("UFO.ogg");

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");

		UFOEffect *be = new UFOEffect(caster, targets[i]);
		battle->addEntity(be);

		lifetime = be->getLifetime();
	}
}


void UFOSpell::apply(void)
{
	applyAttackSpell(1000, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


UFOSpell::UFOSpell(void) :
	count(0)
{
}


UFOSpell::~UFOSpell(void)
{
}


bool OrbitSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void OrbitSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Orbit.ogg");

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("hit");

		OrbitEffect *be = new OrbitEffect(caster, targets[i]);
		battle->addEntity(be);

		lifetime = be->getLifetime();
	}
}


void OrbitSpell::apply(void)
{
	applyAttackSpell(600, caster, targets, numTargets, ELEMENT_NONE);

	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


OrbitSpell::OrbitSpell(void) :
	count(0)
{
}


OrbitSpell::~OrbitSpell(void)
{
}


bool WebSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void WebSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Web.ogg");

	for (int i = 0; i < numTargets; i++) {
		WebEffect *be = new WebEffect(caster, targets[i]);
		battle->addEntity(be);

		lifetime = be->getLifetime();
	}
}


void WebSpell::apply(void)
{
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getInfo().condition = CONDITION_WEBBED;
		targets[i]->getInfo().paralyzeCount = 3;
	}
}


WebSpell::WebSpell(void) :
	count(0)
{
}


WebSpell::~WebSpell(void)
{
}


bool Darkness3Spell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void Darkness3Spell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Darkness3.ogg");
	
	Darkness3Effect *de = new Darkness3Effect();
	battle->addEntity(de);
	lifetime = de->getLifetime();
}


void Darkness3Spell::apply(void)
{
	applyAttackSpell(1000, caster, targets, numTargets, ELEMENT_NONE);
	
	for (int i = 0; i < numTargets; i++) {
		targets[i]->getAnimationSet()->setSubAnimation("stand");
	}
}


Darkness3Spell::Darkness3Spell(void) :
	count(0)
{
}


Darkness3Spell::~Darkness3Spell(void)
{
}


bool SwallowSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void SwallowSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);

	loadPlayDestroy("Mmm.ogg");

	for (int i = 0; i < numTargets; i++) {
		SwallowEffect *de = new SwallowEffect(caster, targets[i]);
		battle->addEntity(de);
		lifetime = de->getLifetime();

		targets[i]->getInfo().condition = CONDITION_SWALLOWED;
	}
}


void SwallowSpell::apply(void)
{
	loadPlayDestroy("Swallow.ogg");

	CombatEnemyTode *t = (CombatEnemyTode *)caster;
	
	AnimationSet *a = targets[0]->getAnimationSet();
	a->setSubAnimation("hit");
	int w = a->getWidth();
	int h = a->getHeight();
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	MBITMAP *bitmap = m_create_bitmap(w, h); // check
	al_set_new_bitmap_flags(flags);
	ALLEGRO_BITMAP *oldTarget = al_get_target_bitmap();
	m_set_target_bitmap(bitmap);
	my_clear_bitmap(bitmap);
	a->draw(0, 0, 0);

	m_draw_bitmap(stomach_circle, 0, 0, 0);

	al_set_target_bitmap(oldTarget);

	al_convert_mask_to_alpha(bitmap->bitmap, al_map_rgb(255, 0, 255));

	float angle = ((float)(rand() % RAND_MAX)/RAND_MAX)*M_PI*2;

	t->swallow(bitmap, angle);

	caster->getAnimationSet()->setSubAnimation("stand");
}


SwallowSpell::SwallowSpell(void) :
	count(0)
{
}


SwallowSpell::~SwallowSpell(void)
{
}


bool PukeSpell::update(int step)
{
	count += step;

	if (count >= lifetime) {
		return true;
	}

	return false;
}


void PukeSpell::init(Combatant *caster, Combatant **targets, int numTargets)
{
	Spell::init(caster, targets, numTargets);
	
	PukeEffect *p = new PukeEffect(caster);
	battle->addEntity(p);
	lifetime = p->getLifetime();
}


void PukeSpell::apply(void)
{
	caster->getAnimationSet()->setSubAnimation("stand");
	
	std::list<CombatEntity *> &entities = battle->getEntities();
	std::list<CombatEntity *>::iterator it;

	int i;
	for (i = 0, it = entities.begin(); it != entities.end(); i++, it++) {
		CombatEntity *e = *it;
		if (e->getType() == COMBATENTITY_TYPE_PLAYER) {
			CombatPlayer *p = (CombatPlayer *)e;
			if (p->getInfo().condition == CONDITION_SWALLOWED) {
				p->getInfo().condition = CONDITION_NORMAL;
				p->getAnimationSet()->setSubAnimation("stand");
				SludgeEffect *se = new SludgeEffect(p, m_map_rgb(0, 255, 0));
				battle->addEntity(se);
			}
		}
	}

	CombatEnemyTode *t = (CombatEnemyTode *)caster;
	t->puke();
}


PukeSpell::PukeSpell(void) :
	count(0)
{
}


PukeSpell::~PukeSpell(void)
{
}

