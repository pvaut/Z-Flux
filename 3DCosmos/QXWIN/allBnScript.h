#pragma once

namespace BNInterface{
	class TBNInterface;
}

void    RegisterScriptsToBn(BNInterface::TBNInterface* itf);

struct RegisterBnScript{
	RegisterBnScript(const char*, const char*);
};
