#pragma once

#include "../hyperstd.h"
#include "../hyperlib.h"
#include <string>

class hyper_shell : public program {
public:

    hyper_shell();
	virtual ~hyper_shell();
	virtual void main();
	virtual void static_init();

private:

	handle<int> argc;
	handle<char> argv[20];

    handle<int> servant(handle<char> cmd);
	void exec();
	handle<int> parse();
	handle<int> get();
	void error();
	std::string to_string(handle<char> str);
	
};

void register_shell();
