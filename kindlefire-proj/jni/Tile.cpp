#include "monster2.hpp"


void Tile::setAnimationNum(unsigned int layer, int animationNum)
{
	animationNums[layer] = animationNum;
}


bool Tile::isSolid(void)
{
	return solid;
}


void Tile::setSolid(bool solid)
{
	this->solid = solid;
}


Tile *Tile::clone(void)
{
	return new Tile(animationNums, solid, tu, tv);
}


Tile::Tile(int animationNums[TILE_LAYERS], bool solid, short tu[TILE_LAYERS], short tv[TILE_LAYERS]) :
	solid(solid)
{
	int i;

	for (i = 0; i < TILE_LAYERS; i++) {
		this->animationNums[i] = animationNums[i];
		alpha[i] = 255;

		this->tu[i] = tu[i];
		this->tv[i] = tv[i];
	}
}


int Tile::getAlpha(int layer)
{
	return alpha[layer];
}


void Tile::setAlpha(int l, int a)
{
	alpha[l] = a;
}


short Tile::getTU(int layer)
{
	return tu[layer];
}

short Tile::getTV(int layer)
{
	return tv[layer];
}

void Tile::setTU(int layer, short tu)
{
	this->tu[layer] = tu;
}

void Tile::setTV(int layer, short tv)
{
	this->tv[layer] = tv;
}

Tile::~Tile(void)
{
}
