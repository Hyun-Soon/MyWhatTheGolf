#pragma once

#include "WindowInitializer.h"
#include "DirectXInitializer.h"

class Initializer
{
public:
	bool initialize();

private:
	WindowInitializer  windowInit;
	DirectXInitializer dxInit;
};