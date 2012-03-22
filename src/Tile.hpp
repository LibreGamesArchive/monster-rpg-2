#ifndef TILE_HPP
#define TILE_HPP


class Tile {
public:
	int getAnimationNum(unsigned int layer)
	{
		return animationNums[layer];
	}
	void setAnimationNum(unsigned int layer, int animationNum);
	bool isSolid(void);
	void setSolid(bool solid);
	int getAlpha(int layer);
	void setAlpha(int layer, int alpha);
	Tile *clone(void);

	short getTU(int layer);
	short getTV(int layer);
	void setTU(int layer, short tu);
	void setTV(int layer, short tv);

	Tile(int animationsNums[TILE_LAYERS], bool solid, short tu[TILE_LAYERS], short tv[TILE_LAYERS]);
	~Tile(void);

protected:
	int animationNums[TILE_LAYERS];
	bool solid;
	int alpha[TILE_LAYERS];
	short tu[TILE_LAYERS], tv[TILE_LAYERS];
};


#endif

