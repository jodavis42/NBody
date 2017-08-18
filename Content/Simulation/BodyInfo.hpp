#pragma once

class BodyInfo
{
public:
  void Load(Cog* cog);

  Real mMass;
  Real3 mPosition;
  Cog* mCog;
};

