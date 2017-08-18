#include "SimulationPrecompiled.hpp"


void BodyInfo::Load(Cog* cog)
{
  mCog = cog;
  mPosition = cog->has(Transform)->GetWorldTranslation();
  mMass = cog->has(RigidBody)->GetMass();
}
