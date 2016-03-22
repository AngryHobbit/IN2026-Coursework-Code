#include "GameUtil.h"
#include "GameWorld.h"
#include "EnemyBullet.h"
#include "EnemySpaceship.h"
#include "BoundingSphere.h"

using namespace std;

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/**  Default constructor. */
EnemySpaceship::EnemySpaceship()
	: GameObject("EnemySpaceship"), mEnemyThrust(0)
{
}

/** Construct a spaceship with given position, velocity, acceleration, angle, and rotation. */
EnemySpaceship::EnemySpaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
	: GameObject("Spaceship", p, v, a, h, r), mEnemyThrust(0)
{
}

/** Copy constructor. */
EnemySpaceship::EnemySpaceship(const EnemySpaceship& s)
	: GameObject(s), mEnemyThrust(0)
{
}

/** Destructor. */
EnemySpaceship::~EnemySpaceship(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update this spaceship. */
void EnemySpaceship::Update(int t)
{
	// Call parent update function
	GameObject::Update(t);
}

/** Render this spaceship. */
void EnemySpaceship::Render(void)
{


	if (mEnemySpaceshipShape.get() != NULL)
	{
		mEnemySpaceshipShape->Render();
	}

	// If ship is thrusting
	if ((mEnemyThrust > 0) && (mEnemyThrusterShape.get() != NULL))
	{
		mEnemyThrusterShape->Render();
	}


	// Enable lighting
	glEnable(GL_LIGHTING);
	GameObject::Render();

		glColor3f(0.9, 0.0, 0.0);
		// Add vertices to draw an octagon
		glVertex3f(-7, -7, 0.0);
		glVertex3f(-10, 0, 0.0);
		glVertex3f(-7, 7, 0.0);
		glVertex3f(0, 10, 0.0);
		glVertex3f(7, 7, 0.0);
		glVertex3f(10, 0, 0.0);
		glVertex3f(7, -7, 0.0);
		glVertex3f(0, -10, 0.0);
		// Finish drawing lines
		glEnd();
		// Enable lighting
		glEnable(GL_LIGHTING);
}

/** Fire the rockets. */
void EnemySpaceship::Thrust(float t)
{
	mEnemyThrust = t;
	// Increase acceleration in the direction of ship
	mAcceleration.x = mEnemyThrust*cos(DEG2RAD*mAngle);
	mAcceleration.y = mEnemyThrust*sin(DEG2RAD*mAngle);
}

/** Set the rotation. */
void EnemySpaceship::Rotate(float r)
{
	mRotation = r;
}

/** Shoot a bullet. */
void EnemySpaceship::Shoot(void)
{
	// Check the world exists
	if (!mWorld) return;
	// Construct a unit length vector in the direction the spaceship is headed
	GLVector3f Enemyspaceship_heading(cos(DEG2RAD*mAngle), sin(DEG2RAD*mAngle), 0);
	Enemyspaceship_heading.normalize();
	// Calculate the point at the node of the spaceship from position and heading
	GLVector3f bullet_position = mPosition + (Enemyspaceship_heading * 4);
	// Calculate how fast the bullet should travel
	float bullet_speed = 30;
	// Construct a vector for the bullet's velocity
	GLVector3f bullet_velocity = mVelocity + Enemyspaceship_heading * bullet_speed;
	// Construct a new bullet
	shared_ptr<GameObject> enemybullet
		(new EnemyBullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
	enemybullet->SetBoundingShape(make_shared<BoundingSphere>(enemybullet->GetThisPtr(), 2.0f));
	enemybullet->SetShape(mEnemyBulletShape);
	// Add the new bullet to the game world
	mWorld->AddObject(enemybullet);

}

bool EnemySpaceship::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Asteroid") && o->GetType() != GameObjectType("SmallAsteroid") && o->GetType() != GameObjectType("Bullet") && o->GetType() != GameObjectType("EnemySpaceship")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void EnemySpaceship::OnCollision(const GameObjectList &objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}