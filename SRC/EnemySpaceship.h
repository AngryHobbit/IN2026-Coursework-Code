#ifndef __ENEMYSPACESHIP_H__
#define __ENEMYSPACESHIP_H__

#include "GameUtil.h"
#include "GameObject.h"
#include "Shape.h"

class EnemySpaceship : public GameObject
{
public:
	EnemySpaceship();
	EnemySpaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r);
	EnemySpaceship(const EnemySpaceship& s);
	virtual ~EnemySpaceship(void);

	virtual void Update(int t);
	virtual void Render(void);

	virtual void Thrust(float t);
	virtual void Rotate(float r);
	virtual void Shoot(void);

	void SetEnemySpaceshipShape(shared_ptr<Shape> spaceship_shape) { mEnemySpaceshipShape = spaceship_shape; }
	void SetEnemyThrusterShape(shared_ptr<Shape> thruster_shape) { mEnemyThrusterShape = thruster_shape; }
	void SetEnemyBulletShape(shared_ptr<Shape> bullet_shape) { mEnemyBulletShape = bullet_shape; }

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList &objects);

	bool mShieldOn;
	bool mShieldLevel2;
	bool mShieldLevel3;


private:
	float mEnemyThrust;

	shared_ptr<Shape> mEnemySpaceshipShape;
	shared_ptr<Shape> mEnemyThrusterShape;
	shared_ptr<Shape> mEnemyBulletShape;
};

#endif