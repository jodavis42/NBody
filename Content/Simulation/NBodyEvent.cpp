#include "SimulationPrecompiled.hpp"

//***************************************************************************
ZilchDefineType(NBodyEvent, builder, type)
{
  // This is required for event binding
  ZilchBindDestructor();
  ZilchBindConstructor();

  ZilchBindMember(mBody);
}

//***************************************************************************
NBodyEvent::NBodyEvent()
{
}

//***************************************************************************
NBodyEvent::~NBodyEvent()
{
}
