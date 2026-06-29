#pragma once
#include"ObjectBase.h"

class ItemBase : public ObjectBase
{
private:

protected:
	//Žg—p‰Â”\‚©‚Ç‚¤‚©
	int Usable_Flg = 0;

public:
	virtual void ResetForDrop() = 0;
};

