#include "SimulationPrecompiled.hpp"

//***************************************************************************
ZilchDefineType(BruteForce, builder, type)
{
  // This is required for component binding
  ZilchBindDestructor();
  ZilchBindConstructor();
  ZilchBindMethod(Initialize);
  
  // Note: All event connection methods must be bound
  ZilchBindMethod(OnLogicUpdate);
  ZilchBindMethod(OnAddBody);
  ZilchBindMethod(OnRemoveBody);

  //ZilchBindField(G)->AddAttributeChainable("Serialized")->Add(new ZeroEngine::MetaSerializedProperty(10));
  ZilchBindFieldProperty(mActive);
  ZilchBindFieldProperty(G);
}

//***************************************************************************
BruteForce::BruteForce()
{
  this->mActive = true;
  this->G = 10;
}

//***************************************************************************
BruteForce::~BruteForce()
{
}

//***************************************************************************
void BruteForce::Initialize(ZeroEngine::CogInitializer* initializer)
{
  ZeroConnectThisTo(this->GetSpace(), "LogicUpdate", "OnLogicUpdate");
  ZeroConnectThisTo(this->GetSpace(), "AddBody", "OnAddBody");
  ZeroConnectThisTo(this->GetSpace(), "RemoveBody", "OnRemoveBody");
}

//***************************************************************************
void BruteForce::OnLogicUpdate(ZeroEngine::UpdateEvent* event)
{
  if(!this->mActive)
    return;

  size_t size = mBodies.Size();
  for(size_t i = 0; i < size; ++i)
  {
    for(size_t j = i + 1; j < size; ++j)
    {
      ComputeForce(i, j);
    }
  }
}

void BruteForce::OnAddBody(NBodyEvent* e)
{
  Cog* body = e->mBody;
  mBodies.PushBack(body);
}

void BruteForce::OnRemoveBody(NBodyEvent* e)
{

}

Real3 BruteForce::ComputeForce(Real3Param posA, Real3Param posB, Real massA, Real massB)
{
  Real3 diff = posB - posA;
  
  Real numer = this->G * massA * massB;
  Real denom = Math::Length(diff);
  denom = denom * denom * denom;
  Real3 force = (numer / denom) * diff;
  return force;
}

void BruteForce::ComputeForce(size_t i, size_t j)
{
  Cog* cogA = mBodies[i];
  RigidBody* bodyA = cogA->has(RigidBody);
  Real massA = cogA->has(MassOverride)->GetMass();
  Real3 posA = cogA->has(Transform)->GetWorldTranslation();

  Cog* cogB = mBodies[j];
  RigidBody* bodyB = cogB->has(RigidBody);
  Real massB = cogB->has(MassOverride)->GetMass();
  Real3 posB = cogB->has(Transform)->GetWorldTranslation();

  Real3 force = ComputeForce(posA, posB, massA, massB);
  bodyA->ApplyForce(force);
  bodyB->ApplyForce(-force);
}

