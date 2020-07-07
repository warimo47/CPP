// Copyright 2020. Kangsan Bae all rights reserved.

// Default Header
#include "Basic.h"
#include "SocketMgr.h"

int main()
{
	CSocketMgr* mySocketMgr = new CSocketMgr;

	mySocketMgr->initialize();

	mySocketMgr->tick();

	return 0;
}

