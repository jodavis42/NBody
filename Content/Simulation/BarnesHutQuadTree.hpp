#pragma once

namespace QuadTreeIndices
{
  enum Enum {TopLeft = 0, TopRight, BottomLeft, BottomRight, Size};
}

class QuadTreeNode
{
public:
  QuadTreeNode();
  ~QuadTreeNode();

  bool IsInternal() const;
  bool IsLeaf() const;
  void CreateChildren();
  void DebugDraw();

  QuadTreeIndices::Enum Classify(Real3Param point);
  void UpdateMass(BodyInfo& bodyInfo);

  Real3 mPosition;
  Real3 mSize;
  QuadTreeNode* mChildren[QuadTreeIndices::Size];

  Real mMass;
  Real3 mCenterOfMass;
  Real3 mPositionAverage;
  Zilch::Array<BodyInfo> mBodies;
};

class BarnesHutQuadTree : public ZeroEngine::ZilchComponent
{
public:
  ZilchDeclareType(Zilch::TypeCopyMode::ReferenceType);
  
  BarnesHutQuadTree();
  ~BarnesHutQuadTree();
  
  void Initialize(ZeroEngine::CogInitializer* initializer);
  
  void OnLogicUpdate(ZeroEngine::UpdateEvent* event);

  void OnAddBody(NBodyEvent* e);
  void OnRemoveBody(NBodyEvent* e);

  void BuildTree();
  void ClearTree();
  void Insert(BodyInfo& bodyInfo);
  void Insert(BodyInfo& bodyInfo, QuadTreeNode* node, int depth);
  Real3 ComputeForce(BodyInfo& bodyInfo, QuadTreeNode* node);
  Real3 ComputeForce(Real3Param posA, Real3Param posB, Real massA, Real massB);

  void DebugDraw(int targetLevel, int currentLevel, QuadTreeNode* node);
  


  Real G;
  bool mActive;
  Zilch::Array<Cog*> mBodies;
  int mMaxDepth;
  int mMaxBodies;
  Real mTheta;
  Real3 mBaseSize;
  Real3 mBasePosition;
  int mDebugDrawLevel;


  QuadTreeNode* mRoot;
};
