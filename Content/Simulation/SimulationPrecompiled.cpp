#include "SimulationPrecompiled.hpp"

//***************************************************************************
ZilchDefineStaticLibraryAndPlugin(SimulationLibrary, SimulationPlugin, ZilchDependencyStub(Core) ZilchDependencyStub(ZeroEngine))
{
  ZilchInitializeType(Simulation);
  ZilchInitializeType(SimulationEvent);
  ZilchInitializeType(BruteForce);
  ZilchInitializeType(NBodyEvent);
  ZilchInitializeType(BarnesHutQuadTree);
  // Auto Initialize (used by Visual Studio plugins, do not remove this line)
}

//***************************************************************************
void SimulationPlugin::Initialize()
{
  // One time startup logic goes here
  // This runs after our plugin library/reflection is built
  Zilch::Console::WriteLine("SimulationPlugin::Initialize");
}

//***************************************************************************
void SimulationPlugin::Uninitialize()
{
  // One time shutdown logic goes here
  Zilch::Console::WriteLine("SimulationPlugin::Uninitialize");
}
