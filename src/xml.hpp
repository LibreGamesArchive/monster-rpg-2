
#ifndef XML_HPP
#define XML_HPP


#include "monster2.hpp"


class XMLData {
public:
	std::list<XMLData*> &getNodes(void);
	XMLData* find(std::string name);
	std::string getValue(void);
	void write(std::ofstream& out, int tab);
	void add(XMLData* node);
	std::string getName(void);
	XMLData(std::string name, std::string value);
	XMLData(std::string filename);
	~XMLData(void);
private:
	XMLData(std::string name, FILE* f);
	std::string readToken(void);
	int get(void);
	void unget(int c);
	void seek(long pos);
	void read();
	std::string getName(std::string token);
	FILE* file;
	std::string name;
	std::string value;
	std::list<XMLData*> nodes;
	bool debug;
	int ungot;
	bool ungotReady;
};


#endif // XML_HPP
