#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "EnemySpaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "SmallAsteroid.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
	mPowerUpCount = 0;
	mGameStarted = false;
	mCSA = 2;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	//Animation *asteroid3_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid2", 128, 8192, 128, 128, "asteroid2_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");


	// Create a spaceship and add it to the world
	//mGameWorld->AddObject(CreateSpaceship());
	// Create a enemy spaceship and add it to the world
	mGameWorld->AddObject(CreateEnemySpaceship());
	SetTimer(1000, ENEMY_SHOOT);
	// Create some asteroids and add them to the world
	CreateAsteroids(5);

	CreatePowerUp(1);
	//Create the GUI
	CreateGUI();

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player 
	mPlayer.AddListener(thisPtr);

	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	if (!mGameStarted)
	{
		switch (key)
		{
		case ' ':
			// Create a spaceship and add it to the world
			mScoreKeeper.mScore = 0;
			mScoreKeeper.FireScoreChanged();
			mPlayer.mLives = 3;
			mGameWorld->AddObject(CreateSpaceship());
			SetTimer(9000, INVINCIBLE);
			mLevel = 0;
			
			mGameWorld->FlagForRemoval(mEnemySpaceship);

			//mGameWorld->  GetType() == GameObjectType("PowerUp"));
			//mGameWorld->AddObject(CreateEnemySpaceship());
			//SetTimer(1000, ENEMY_SHOOT);
			//mGameWorld->AddObject(CreateEnemySpaceship());
			// Create some asteroids and add them to the world
			//CreateAsteroids(6);
			//CreatePowerUp(1);
			mGameStarted = true;
			mStartLabel->SetVisible(false);
			mHighScoreLabel->SetVisible(false);
			mHighScoreNumLabel->SetVisible(false);  
			break;
		default:
			break;
		}
	}
	else
	{
		switch (key)
		{
		case ' ':
			mSpaceship->Shoot();
			break;
		default:
			break;
		}
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	if (mGameStarted)
	{
		switch (key)
		{
			// If up arrow key is pressed start applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(20); break;
			// If left arrow key is pressed start rotating anti-clockwise
		case GLUT_KEY_LEFT: mSpaceship->Rotate(180); break;
			// If right arrow key is pressed start rotating clockwise
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(-180); break;
			// Default case - do nothing
		default: break;
		}
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	if (mGameStarted)
	{
		switch (key)
		{
			// If up arrow key is released stop applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
			// If left arrow key is released stop rotating
		case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
			// If right arrow key is released stop rotating
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
			// Default case - do nothing
		default: break;
		}
	}
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("EnemySpaceship"))
	{
		if (!mGameStarted)
		{
			shared_ptr<GameObject> explosion = CreateExplosion();
			explosion->SetPosition(mEnemySpaceship->GetPosition());
			explosion->SetRotation(mEnemySpaceship->GetRotation());
			mGameWorld->AddObject(explosion);
			mGameWorld->AddObject(CreateEnemySpaceship());
		}
	}

	if (object->GetType() == GameObjectType("PowerUp"))
	{
		if (!mGameStarted)
		{
			if (mEnemySpaceship->mShieldOn == true)
			{
				if (mEnemySpaceship->mShieldLevel2 == true)
				{
					mEnemySpaceship->mShieldLevel3 = true;
				}
				mEnemySpaceship->mShieldLevel2 = true;
			}
			mEnemySpaceship->mShieldOn = true;
		}
		else
		{
			if (mSpaceship->mShieldOn == true)
			{
				if (mSpaceship->mShieldLevel2 == true)
				{
					mSpaceship->mShieldLevel3 = true;
				}
				mSpaceship->mShieldLevel2 = true;
			}
			mSpaceship->mShieldOn = true;
		}
		SetTimer(3000, CREATE_POWERUP);
	}

	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;

		
		for (uint i = 0; i < mCSA ; i++)
		{
			shared_ptr<GameObject> smallAsteroid = CreateSmallAsteroids(1);
			smallAsteroid->SetPosition(object->GetPosition());
			smallAsteroid->SetRotation(object->GetRotation());
			mGameWorld->AddObject(smallAsteroid);
		}
		
		if (mAsteroidCount <= 0) 
		{ 
			SetTimer(1000, START_NEXT_LEVEL); 
		}
	}
	
	if (object->GetType() == GameObjectType("SmallAsteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;

		if (mAsteroidCount <= 0)
		{
			SetTimer(1000, START_NEXT_LEVEL);
		}
	}
	
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == ENEMY_SHOOT)
	{
		if (!mGameStarted)
		{
			mEnemySpaceship->Thrust(rand() % 10 + (-2));
			mEnemySpaceship->Rotate(rand() % 120 + (-60));
			mEnemySpaceship->Shoot();
			SetTimer(1000, ENEMY_SHOOT);
		}
		if (mGameStarted)
		{
			mEnemySpaceship->Thrust(rand() % 10 + (-2));
			mEnemySpaceship->Rotate(mSpaceship->GetPosition().x - mSpaceship->GetPosition().y);
			mEnemySpaceship->Shoot();
			SetTimer(1000, ENEMY_SHOOT);

		}
	}
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 3 + (2 * mLevel);
		CreateAsteroids(num_asteroids);
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
		SetTimer(3000, SHOW_GAME_START);
	}

	if (value == SHOW_GAME_START)
	{
		mGameOverLabel->SetVisible(false);
		mLivesLabel->SetVisible(false);
		mScoreLabel->SetVisible(false);
		mScoreKeeper.mScore = 0;
		mLevel = 0;
		CreateGUI();
		mGameStarted = false;
		mGameWorld->AddObject(CreateEnemySpaceship());
		SetTimer(1000, ENEMY_SHOOT);
	}

	if (value == CREATE_POWERUP)
	{
		CreatePowerUp(1);
	}

	if (value == INVINCIBLE)
	{
		mSpaceship->mInvincible = false;
	}
}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

shared_ptr<GameObject> Asteroids::CreateEnemySpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mEnemySpaceship = make_shared<EnemySpaceship>();
	mEnemySpaceship->SetBoundingShape(make_shared<BoundingSphere>(mEnemySpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> enemy_bullet_shape = make_shared<Shape>("bullet.shape");
	mEnemySpaceship->SetEnemyBulletShape(enemy_bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mEnemySpaceship->SetSprite(spaceship_sprite);
	mEnemySpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mEnemySpaceship->SetPosition(GLVector3f(0, 25, 0));
	// Return the spaceship so it can be added to the world
	return mEnemySpaceship;

}

void Asteroids::CreatePowerUp(const uint num_powerUp)
{
	mPowerUpCount = num_powerUp;

	for (uint i = 0; i < num_powerUp; i++) 
	{
		shared_ptr<GameObject> powerUp = make_shared<PowerUp>();
		powerUp->SetBoundingShape(make_shared<BoundingSphere>(powerUp->GetThisPtr(), 5.0f));
		powerUp->SetScale(0.2f);
		mGameWorld->AddObject(powerUp);
	}
}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}

shared_ptr<GameObject> Asteroids::CreateSmallAsteroids(const uint num_asteroids)
{
	mAsteroidCount = mAsteroidCount + num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> smallAsteroid = make_shared<SmallAsteroid>();
		smallAsteroid->SetBoundingShape(make_shared<BoundingSphere>(smallAsteroid->GetThisPtr(), 4.0f));
		smallAsteroid->SetSprite(asteroid_sprite);
		smallAsteroid->SetScale(0.1f);
		mGameWorld->AddObject(smallAsteroid);

		return smallAsteroid;

	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));


	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	//Start Label
	mStartLabel = make_shared<GUILabel>("Press Space To Start");

	mStartLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);

	mStartLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	
	shared_ptr<GUIComponent> start_component = static_pointer_cast<GUIComponent>(mStartLabel);
	mGameDisplay->GetContainer()->AddComponent(start_component, GLVector2f(0.5f, 0.6f));

	//HighScore Label
	mHighScoreLabel = make_shared<GUILabel>("The Last HighScore Was: ");

	mHighScoreLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);

	mHighScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);

	shared_ptr<GUIComponent> highScore_component = static_pointer_cast<GUIComponent>(mHighScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(highScore_component, GLVector2f(0.5f, 0.45f));

	ifstream highScore("HighScore.txt");
	string highscore;
	highScore >> highscore;
	//HighScoreNum Label
	mHighScoreNumLabel = make_shared<GUILabel>(highscore);

	mHighScoreNumLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);

	mHighScoreNumLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);

	shared_ptr<GUIComponent> highScoreNum_component = static_pointer_cast<GUIComponent>(mHighScoreNumLabel);
	mGameDisplay->GetContainer()->AddComponent(highScoreNum_component, GLVector2f(0.5f, 0.40));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component = static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER);
		mSpaceship->mShieldOn = true;
		mSpaceship->mInvincible = true;
		SetTimer(9000, INVINCIBLE);
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);

		//mScoreKeeper.mScore = mScoreKeeper.mScore;
		//mScoreKeeper.FireScoreChanged();
		ofstream highScore;
		highScore.open("HighScore.txt");
		highScore << mScoreKeeper.mScore;
		highScore.close();
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}




