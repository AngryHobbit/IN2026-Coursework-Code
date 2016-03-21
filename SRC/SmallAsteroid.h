#ifndef __SMALLASTEROID_H__
#define __SMALLASTEROID_H__

#include "GameObject.h"

class SmallAsteroid : public GameObject
{
public:
	SmallAsteroid(void);
	~SmallAsteroid(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif
