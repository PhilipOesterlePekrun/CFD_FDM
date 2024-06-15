#pragma once

#include <string>
using namespace std;

class ISimulation // base class for all simulations
{
public:
	// simulation information
	static string simulationMethodName;
	int simulationID;

	// every simulation must have run(), and possibly more which I will add in time
	virtual bool run();
	virtual void printInformation();
};