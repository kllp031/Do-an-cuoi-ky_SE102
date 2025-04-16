#include "PiranhaPlant.h"
#include "PlayScene.h"

CPiranhaPlant::CPiranhaPlant(float x, float y, CFireBullet* fB) : CEnemy(x, y)
{
	this->x = x;
	this->y = y;
	originalY = y;
	this->fireBullet = fB;
	SetState(PIRANHA_STATE_HIDE);
	isShooting = false;									// Initialize shooting state
}

void CPiranhaPlant::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - PIRANHA_BBOX_WIDTH / 2;
	top = y - PIRANHA_BBOX_HEIGHT / 2;
	right = left + PIRANHA_BBOX_WIDTH;
	bottom = top + PIRANHA_BBOX_HEIGHT;
}

void CPiranhaPlant::SetState(int state)
{
	stateStartTime = GetTickCount64();
	this->state = state;

	switch (state) {
	case PIRANHA_STATE_HIDE:
		fireBullet->SetState(FIRE_BULLET_STATE_INACTIVE); // Reset the bullet state
		fireBullet->SetPosition(x, y);
		isShooting = false; // Reset shooting state
		vy = 0;
		break;
	case PIRANHA_STATE_RISE:
		fireBullet->SetState(FIRE_BULLET_STATE_INACTIVE); // Reset the bullet state
		fireBullet->SetPosition(x, y);
		isShooting = false; // Reset shooting state
		vy = -PIRANHA_MOVE_SPEED;
		break;
	case PIRANHA_STATE_SNIP:
		vy = 0;
		break;
	case PIRANHA_STATE_DIVE:
		fireBullet->SetState(FIRE_BULLET_STATE_INACTIVE); // Reset the bullet state
		fireBullet->SetPosition(x, y);
		isShooting = false; // Reset shooting state
		vy = PIRANHA_MOVE_SPEED;
		break;
	default:
		break;
	}
}

int CPiranhaPlant::GetSnippingDirection()
{
	// Get the current scene and player
	CGame* game = CGame::GetInstance();
	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(currentScene->GetPlayer());

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	// Check the position of the Piranha Plant relative to Mario
	if (marioX < x) {
		if (marioY < y)
			return 0;		// UP_LEFT
		else
			return 1;		// DOWN_LEFT
	}
	else {
		if (marioY < y)
			return 2;		// UP_RIGHT
		else
			return 3;		// DOWN_RIGHT
	}
}

void CPiranhaPlant::Render()
{
	if (state == PIRANHA_STATE_HIDE)
		return;

	int aniId = -1;

	if (state == PIRANHA_STATE_RISE || state == PIRANHA_STATE_DIVE) {
		int direction = GetSnippingDirection();
		if (direction == 0 || direction == 1)
			aniId = PIRANHA_ANI_LEFT_RISE_DIVE;
		else
			aniId = PIRANHA_ANI_RIGHT_RISE_DIVE;
	}
	else if (state == PIRANHA_STATE_SNIP) {
		int direction = GetSnippingDirection();
		if (direction == 0)
			aniId = PIRANHA_ANI_UP_LEFT;
		else if (direction == 1)
			aniId = PIRANHA_ANI_DOWN_LEFT;
		else if (direction == 2)
			aniId = PIRANHA_ANI_UP_RIGHT;
		else
			aniId = PIRANHA_ANI_DOWN_RIGHT;
	}

	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniId)->Render(x, y);

	RenderBoundingBox();
}

int CPiranhaPlant::GetDirectionRange() {
	// Get Mario's position
	CGame* game = CGame::GetInstance();
	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(currentScene->GetPlayer());

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	float relativeX = fabs(marioX - x);

	if (GetSnippingDirection() == 0 || GetSnippingDirection() == 1) {
		if (relativeX > FAR_POINT_LEFT_RIGHT || (relativeX < NEAR_POINT_MAX_LEFT && relativeX > NEAR_POINT_MIN_LEFT))
		{
			return IN_RANGE_OF_LEFT;
		}
	}
	else {
		if (relativeX > FAR_POINT_LEFT_RIGHT || (relativeX < NEAR_POINT_MAX_RIGHT && relativeX > NEAR_POINT_MIN_RIGHT))
		{
			return IN_RANGE_OF_RIGHT;
		}
	}
}

void CPiranhaPlant::Shoot(int direction)
{
	if (fireBullet == NULL) return;

	CGame* game = CGame::GetInstance();
	CPlayScene* currentScene = dynamic_cast<CPlayScene*>(game->GetCurrentScene());
	CMario* mario = dynamic_cast<CMario*>(currentScene->GetPlayer());

	float marioX, marioY;
	mario->GetPosition(marioX, marioY);

	float relativeX = fabs(marioX - x);
	int snipDirection = GetSnippingDirection();

	if (snipDirection == 0)
		fireBullet->SetState(FIRE_BULLET_STATE_LEFT_SHOOT_HIGH);
	else if (snipDirection == 2)
		fireBullet->SetState(FIRE_BULLET_STATE_RIGHT_SHOOT_HIGH);
	else if (snipDirection == 1) {
		if (relativeX < FAR_POINT_LEFT_RIGHT || relativeX > 75)
			fireBullet->SetState(FIRE_BULLET_STATE_LEFT_SHOOT_FAR);
		else
			fireBullet->SetState(FIRE_BULLET_STATE_LEFT_SHOOT_LOW);
	}
	else {
		if (relativeX > FAR_POINT_LEFT_RIGHT || relativeX > 75)
			fireBullet->SetState(FIRE_BULLET_STATE_RIGHT_SHOOT_FAR);
		else
			fireBullet->SetState(FIRE_BULLET_STATE_RIGHT_SHOOT_LOW);
	}

	isShooting = true;
	//DebugOut(L"[INFO] Piranha plant shoot bullet at direction: %d\n", fireBullet->GetState());
}

void CPiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	ULONGLONG now = GetTickCount64();

	int directionRange = GetDirectionRange();
	if (directionRange == IN_RANGE_OF_LEFT || directionRange == IN_RANGE_OF_RIGHT)
	{
		if (state == PIRANHA_STATE_HIDE) {
			SetState(PIRANHA_STATE_RISE); // Transition to rise first
		}
		else if (this->state == PIRANHA_STATE_RISE) {
			SetState(PIRANHA_STATE_RISE); // Stay rising
		}
	}

	if (!isShooting && fireBullet != NULL) {
		fireBullet->SetPosition(x, y); // Update the position of the fire bullet
	}

	// Mario is in the range of snipping
	switch (state) {
	case PIRANHA_STATE_HIDE:
		if (now - stateStartTime > PIRANHA_HIDE_TIMEOUT) {
			SetState(PIRANHA_STATE_RISE);
		}
		break;

	case PIRANHA_STATE_RISE:
		y += vy * dt;
		if (fabs(y - originalY) >= PIRANHA_BBOX_HEIGHT_RISE) {
			y = originalY - PIRANHA_BBOX_HEIGHT_RISE;
			SetState(PIRANHA_STATE_SNIP);
		}
		break;

	case PIRANHA_STATE_SNIP:
		if (isShooting == false) {
			Shoot(GetSnippingDirection());
			DebugOut(L"[INFO] Piranha plant shoot bullet at direction: %d\n", fireBullet->GetState());
		}
		if (now - stateStartTime > PIRANHA_SNIP_TIMEOUT) {		
			SetState(PIRANHA_STATE_DIVE);
		}
		break;

	case PIRANHA_STATE_DIVE:
		y += vy * dt;
		if (fabs(y - originalY) >= PIRANHA_BBOX_HEIGHT_RISE) {
			y = originalY + PIRANHA_BBOX_HEIGHT_RISE;
			SetState(PIRANHA_STATE_HIDE);
		}
		break;

	default:
		break;
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}