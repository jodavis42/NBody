#pragma once

using ZeroEngine::Cog;

class NBodyEvent : public ZeroEngine::ZilchEvent
{
public:
  ZilchDeclareType(Zilch::TypeCopyMode::ReferenceType);
  
  NBodyEvent();
  ~NBodyEvent();

  Zilch::HandleOf<Cog> mBody;
};
