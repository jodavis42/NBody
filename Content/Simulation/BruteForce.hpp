#pragma once

#include "NBodyEvent.hpp"

class BruteForce : public ZeroEngine::ZilchComponent
{
public:
  ZilchDeclareType(Zilch::TypeCopyMode::ReferenceType);
  
  BruteForce();
  ~BruteForce();
  
  void Initialize(ZeroEngine::CogInitializer* initializer);
  
  void OnLogicUpdate(ZeroEngine::UpdateEvent* event);

  void OnAddBody(NBodyEvent* e);
  void OnRemoveBody(NBodyEvent* e);

  Real3 ComputeForce(Real3Param posA, Real3Param posB, Real massA, Real massB);
  void ComputeForce(size_t i, size_t j);

  typedef Zilch::HandleOf<Cog> CogHandle;
  Zilch::Array<Cog*> mBodies;
  Real G;
  bool mActive;
};
