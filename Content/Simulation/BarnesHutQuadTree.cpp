#include "SimulationPrecompiled.hpp"


QuadTreeNode::QuadTreeNode()
{
  for(size_t i = 0; i < QuadTreeIndices::Size; ++i)
    mChildren[i] = nullptr;
  mPositionAverage = Real3::cZero;
  mMass = 0;
}

QuadTreeNode::~QuadTreeNode()
{
  for(size_t i = 0; i < QuadTreeIndices::Size; ++i)
    delete mChildren[i];
}

bool QuadTreeNode::IsInternal() const
{
  return !IsLeaf();
}

bool QuadTreeNode::IsLeaf() const
{
  return mChildren[0] == nullptr;
}

void QuadTreeNode::CreateChildren()
{
  ReturnIf(IsInternal(), , "Cannot create children when already an internal node.");

  Real3 halfSize = mSize / 2.0f;
  Real3 offset = mSize / 4.0f;
  Real3 offsets[QuadTreeIndices::Size];
  offsets[QuadTreeIndices::BottomLeft]  = Real3(-offset.x,-offset.y, 0);
  offsets[QuadTreeIndices::BottomRight] = Real3(+offset.x,-offset.y, 0);
  offsets[QuadTreeIndices::TopLeft]     = Real3(-offset.x,+offset.y, 0);
  offsets[QuadTreeIndices::TopRight]    = Real3(+offset.x,+offset.y, 0);

  for(size_t i = 0; i < QuadTreeIndices::Size; ++i)
  {
    mChildren[i] = new QuadTreeNode();
    mChildren[i]->mSize = halfSize;
    mChildren[i]->mPosition = mPosition + offsets[i];
  }
}

void QuadTreeNode::DebugDraw()
{
  auto executableState = ZZ::ExecutableState::GetCallingState();
  Zilch::ExceptionReport report;

  Zilch::BoundType* boxType = ZilchTypeId(ZeroEngine::DebugObb);
  Zilch::Handle handle = executableState->AllocateHeapObject(boxType, report, Zilch::HeapFlags::ReferenceCounted);
  //Zilch::DelegateType constructorType;
  //constructorType.Parameters.PushBack(Zilch::DelegateParameter(ZilchTypeId(Real3)));
  //constructorType.Parameters.PushBack(Zilch::DelegateParameter(ZilchTypeId(Real3)));
  //constructorType.Return = ZilchTypeId(Zilch::Void);
  Zilch::Function* constructor = (*boxType->GetOverloadedInheritedConstructors())[0];
  //Zilch::Function* constructor = boxType->GetConstructor(&constructorType, false);
  
  
  Zilch::Call call(constructor, executableState);
  call.SetHandle(Zilch::Call::This, handle);
  call.Set(0, mPosition);
  call.Set(1, mSize / 2.0f);
  call.Invoke();
  

  //Zilch::HandleOf<ZeroEngine::DebugObb> boxHandle = ZilchAllocate(ZeroEngine::DebugObb, mPosition, mSize / 2.0f);
  ZeroEngine::DebugObb* box = handle.Get<ZeroEngine::DebugObb*>();
  //box->SetPosition(mPosition);
  //box->SetHalfExtents(mSize / 2.0f);
  ZeroEngine::DebugDraw::Add(*box);

}

QuadTreeIndices::Enum QuadTreeNode::Classify(Real3Param point)
{
  
  if(point.x < mPosition.x)
  {
    if(point.y < mPosition.y)
      return QuadTreeIndices::BottomLeft;
    return QuadTreeIndices::TopLeft;
  }
  else
  {
    if(point.y < mPosition.y)
      return QuadTreeIndices::BottomRight;
    return QuadTreeIndices::TopRight;
  }
}

void QuadTreeNode::UpdateMass(BodyInfo& bodyInfo)
{
  mPositionAverage += bodyInfo.mPosition * bodyInfo.mMass;
  mMass += bodyInfo.mMass;
  
  mCenterOfMass = mPositionAverage / mMass;
}

//***************************************************************************
ZilchDefineType(BarnesHutQuadTree, builder, type)
{
  // This is required for component binding
  ZilchBindDestructor();
  ZilchBindConstructor();
  ZilchBindMethod(Initialize);
  
  // Note: All event connection methods must be bound
  ZilchBindMethod(OnLogicUpdate);
  ZilchBindMethod(OnAddBody);
  ZilchBindMethod(OnRemoveBody);

  ZilchBindFieldProperty(mActive);
  ZilchBindFieldProperty(G);
  ZilchBindFieldProperty(mDebugDrawLevel);
  ZilchBindFieldProperty(mMaxBodies);
  ZilchBindFieldProperty(mMaxDepth);
  ZilchBindFieldProperty(mTheta);
}

//***************************************************************************
BarnesHutQuadTree::BarnesHutQuadTree()
{
  this->mActive = true;
  this->G = 10;
  this->mMaxBodies = 10;
  this->mMaxDepth = 100;
  this->mBasePosition = Real3::cZero;
  this->mBaseSize = Real3(1000);
  this->mDebugDrawLevel = -1;
  this->mTheta = 0.5f;
}

//***************************************************************************
BarnesHutQuadTree::~BarnesHutQuadTree()
{
}

//***************************************************************************
void BarnesHutQuadTree::Initialize(ZeroEngine::CogInitializer* initializer)
{
  ZeroConnectThisTo(this->GetSpace(), "LogicUpdate", "OnLogicUpdate");
  ZeroConnectThisTo(this->GetSpace(), "AddBody", "OnAddBody");
  ZeroConnectThisTo(this->GetSpace(), "RemoveBody", "OnRemoveBody");
}

//***************************************************************************
void BarnesHutQuadTree::OnLogicUpdate(ZeroEngine::UpdateEvent* event)
{
  if(!this->mActive)
    return;

  ClearTree();
  BuildTree();

  for(size_t i = 0; i < mBodies.Size(); ++i)
  {
    BodyInfo bodyInfo;
    bodyInfo.Load(mBodies[i]);
    Real3 totalForce = ComputeForce(bodyInfo, mRoot);
    RigidBody* body = bodyInfo.mCog->has(RigidBody);
    body->ApplyForce(totalForce);
  }

  DebugDraw(mDebugDrawLevel, 0, mRoot);
}

void BarnesHutQuadTree::OnAddBody(NBodyEvent* e)
{
  Cog* cog = e->mBody;
  mBodies.PushBack(cog);

  
}

void BarnesHutQuadTree::OnRemoveBody(NBodyEvent* e)
{

}

void BarnesHutQuadTree::BuildTree()
{
  for(size_t i = 0; i < mBodies.Size(); ++i)
  {
    BodyInfo bodyInfo;
    bodyInfo.Load(mBodies[i]);
    Insert(bodyInfo);
  }
}

void BarnesHutQuadTree::ClearTree()
{
  delete mRoot;
  mRoot = nullptr;
}

void BarnesHutQuadTree::Insert(BodyInfo& bodyInfo)
{
  if(mRoot == nullptr)
  {
    mRoot = new QuadTreeNode();
    mRoot->mBodies.PushBack(bodyInfo);
    mRoot->UpdateMass(bodyInfo);
    mRoot->mPosition = mBasePosition;
    mRoot->mSize = mBaseSize;
    return;
  }

  Insert(bodyInfo, mRoot, 1);
}

void BarnesHutQuadTree::Insert(BodyInfo& bodyInfo, QuadTreeNode* node, int depth)
{
  node->UpdateMass(bodyInfo);

  if(node->IsInternal())
  {  
    QuadTreeIndices::Enum quadrant = node->Classify(bodyInfo.mPosition);
    Insert(bodyInfo, node->mChildren[quadrant], depth + 1);
    return;
  }

  node->mBodies.PushBack(bodyInfo);
  if((int)node->mBodies.Size() < mMaxBodies || depth >= mMaxDepth)
    return;

  node->CreateChildren();
  for(size_t i = 0; i < node->mBodies.Size(); ++i)
  {
    BodyInfo& childBody = node->mBodies[i];
    QuadTreeIndices::Enum quadrant = node->Classify(childBody.mPosition);
    Insert(childBody, node->mChildren[quadrant], depth + 1);
  }
  node->mBodies.Clear();
}

Real3 BarnesHutQuadTree::ComputeForce(BodyInfo& bodyInfo, QuadTreeNode* node)
{
  if(node == nullptr)
    return Real3::cZero;

  if(node->IsLeaf())
  {
    Real3 force = Real3::cZero;
    for(size_t i = 0; i < node->mBodies.Size(); ++i)
    {
      BodyInfo& otherBody = node->mBodies[i];
      if(otherBody.mCog != bodyInfo.mCog)
      {
        force += ComputeForce(bodyInfo.mPosition, otherBody.mPosition, bodyInfo.mMass, otherBody.mMass);
      }
    }
    return force;
  }

  Real width = node->mSize.x;
  Real distance = Math::Distance(bodyInfo.mPosition, node->mCenterOfMass);
  Real ratio = width / distance;
  if(ratio < this->mTheta)
    return ComputeForce(bodyInfo.mPosition, node->mCenterOfMass, bodyInfo.mMass, node->mMass);
  
  Real3 force = Real3::cZero;
  for(size_t i = 0; i < QuadTreeIndices::Size; ++i)
  {
    force += ComputeForce(bodyInfo, node->mChildren[i]);
  }
  return force;
}

Real3 BarnesHutQuadTree::ComputeForce(Real3Param posA, Real3Param posB, Real massA, Real massB)
{
  Real3 diff = posB - posA;

  Real numer = this->G * massA * massB;
  Real denom = Math::Length(diff);
  denom = denom * denom * denom;
  Real3 force = (numer / denom) * diff;
  return force;
}

void BarnesHutQuadTree::DebugDraw(int targetLevel, int currentLevel, QuadTreeNode* node)
{
  if(node == nullptr)
    return;

  if(targetLevel == -1 || currentLevel == targetLevel)
    node->DebugDraw();

  if(targetLevel == -1 || currentLevel < targetLevel)
  {
    for(size_t i = 0; i < QuadTreeIndices::Size; ++i)
      DebugDraw(targetLevel, currentLevel + 1, node->mChildren[i]);
  }
}

