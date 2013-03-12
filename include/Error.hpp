#ifndef ERROR_H
#define ERROR_H

class Error {};
class ReadError : public Error {};	// error reading file
class WriteError : public Error {};	// error writing file
class NoMode : public Error {};		// error setting graphics mode
class ScriptError : public Error {};
class InitError : public Error {};

#endif

