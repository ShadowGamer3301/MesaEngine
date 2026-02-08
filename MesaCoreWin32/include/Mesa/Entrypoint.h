#pragma once
#include "Application.h"
#include "Exception.h"

#ifdef _DEBUG
	
int main() try
{
	return 0;
}
catch (Mesa::Exception& e)
{
	LOG_F(ERROR, "%s", e.what());
	return 1;
}

#else

#endif