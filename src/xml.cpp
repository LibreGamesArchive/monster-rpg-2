#include "monster2.hpp"

bool XMLData::getFailed(void)
{
	return failed;
}

std::string XMLData::getName(void)
{
	return name;
}


std::list<XMLData*> &XMLData::getNodes(void)
{
	return nodes;
}


XMLData::XMLData(std::string name, ALLEGRO_FILE* f) :
	file(f),
	debug(false),
	ungot(-1),
	ungotReady(false),
	failed(false)
{
	this->name = name;
}


XMLData::XMLData(std::string filename, int must_exist) :
	debug(false),
	ungot(-1),
	ungotReady(false),
	failed(false)
{
	name = std::string("main");

	file = al_fopen(filename.c_str(), "r");
	if (!file) {
		if (must_exist == 1) {
			native_error("Load error.", filename.c_str());
		}
		al_fclose(file);
		failed = true;
		return;
	}

	read();

	al_fclose(file);
}


XMLData::XMLData(std::string name, std::string value) :
	debug(false),
	failed(false)
{
	this->name = name;
	this->value = value;
}


void XMLData::add(XMLData* node)
{
	nodes.push_back(node);
}


XMLData* XMLData::find(std::string name)
{
	std::list<XMLData*>::iterator it = nodes.begin();

	while (it != nodes.end()) {
		if ((*it)->name == name) {
			return *it;
		}
		it++;
	}

	return 0;
}


std::string XMLData::getValue()
{
	return value;
}


XMLData::~XMLData()
{
	std::list<XMLData*>::iterator it = nodes.begin();

	while (it != nodes.end()) {
		XMLData* node = *it;
		delete node;
		it++;
	}

	nodes.clear();
}


std::string XMLData::readToken()
{
	if (al_feof(file)) {
		return "(null)";
	}

	int c;

	/* Skip whitespace */

	for (;;) {
		c = get();
		if (c == EOF) {
			return "(null)";
		}
		if (!isspace(c)) {
			break;
		}
	}

	/* Found tag */

	if (c == '<') {
		if (debug)
			std::cout << "Found tag start/end\n";
		std::string token;
		token += c;
		for (;;) {
			c = get();
			if (c == EOF) {
				break;
			}
			token += c;
			if (c == '>')
				break;
		}
		if (debug)
			std::cout << "Read <token>: " << token << "\n";
		return token;
	}
	/* Found data */
	else {
		std::string data;
		data += c;
		for (;;) {
			c = get();
			if (c == EOF) {
				break;
			}
			if (c == '<') {
				unget(c);
				break;
			}
			data += c;
		}
		if (debug)
			std::cout << "Read data: " << data << "\n";
		return data;
	}

	return "(null)";
}


int XMLData::get()
{
	int c;

	if (ungotReady) {
		c = ungot;
		ungotReady = false;
	}
	else {
		c = al_fgetc(file);
	}

	return c;
}


void XMLData::unget(int c)
{
	ungot = c;
	ungotReady = true;
}


void XMLData::read()
{
	// read until EOF or end token
	
	for (;;) {
		std::string token;
		token = readToken();
		if (token == "(null)" || (!strncmp(token.c_str(), "</", 2))) {
			return;
		}
		if (token.c_str()[0] == '<') {
			if (debug) {
				std::cout << "Reading sub tag\n";
			}
			std::string name = getName(token);
			if (debug)
				std::cout << "Token is " << name.c_str() << "\n";
			XMLData* newdata = new XMLData(name, file);
			newdata->read();
			nodes.push_back(newdata);
		}
		else {
			value += token.c_str();
		}
	}
}


std::string XMLData::getName(std::string token)
{
	std::string s;
	
	for (int i = 1; token.c_str()[i] != '>' && token.c_str()[i]; i++) {
		if (debug)
			std::cout << "Read character " << i << "\n";
		s += token.c_str()[i];
	}

	return s;
}


void XMLData::write(std::ofstream& out, int tabs = 0)
{
	if (value == "") {
		for (int i = 0; i < tabs; i++) {
			out << "\t";
		}

		out << "<" << name.c_str() << ">" << std::endl;

		std::list<XMLData*>::iterator it = nodes.begin();

		while (it != nodes.end()) {
			XMLData* node = dynamic_cast<XMLData*>(*it);
			node->write(out, tabs+1);
			it++;
		}

		for (int i = 0; i < tabs; i++) {
			out << "\t";
		}

		out << "</" << name.c_str() << ">" << std::endl;
	}
	else {
		for (int i = 0; i < tabs; i++) {
			out << "\t";
		}

		out << "<" << name.c_str() << ">";

		out << value;
		
		std::list<XMLData*>::iterator it = nodes.begin();

		while (it != nodes.end()) {
			XMLData* node = dynamic_cast<XMLData*>(*it);
			node->write(out, tabs+1);
			it++;
		}

		out << "</" << name.c_str() << ">" << std::endl;
	}
}


