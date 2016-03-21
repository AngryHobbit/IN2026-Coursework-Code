#include <stdlib.h>
#include "GameUtil.h"
#include "SmallAsteroid.h"
#include "BoundingShape.h"

SmallAsteroid::SmallAsteroid(void) : GameObject("SmallAsteroid")
{
	mAngle = rand() % 360;
	mRotation = rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 15.0 * cos(DEG2RAD*mAngle);
	mVelocity.y = 15.0 * sin(DEG2RAD*mAngle);
	mVelocity.z = 0.0;
}

SmallAsteroid::~SmallAsteroid(void)
{
}

bool SmallAsteroid::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() == GameObjectType("PowerUp")) return false;
	if (o->GetType() == GameObjectType("Asteroid")) return false;
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void SmallAsteroid::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}
