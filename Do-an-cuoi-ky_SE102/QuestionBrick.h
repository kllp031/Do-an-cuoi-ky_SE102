#pragma once
#include "Brick.h"
#include "GameObject.h"
#include "Mario.h"
#include "debug.h"

#define BRICK_BOUNCE_SPEED 0.1f
#define BRICK_BOUNCE_HEIGHT 10.0f
#define BRICK_BOUNCE_TIME 210
#define BRICK_STATE_BOUNCE 100

#define ITEM_TYPE_COIN 1
#define ITEM_TYPE_POWERUP 2
#define ITEM_TYPE_PSWITCH 3
#define ITEM_TYPE_1UP_MUSHROOM 4

class CQuestionBrick : public CBrick
{
protected:
    LPGAMEOBJECT item; // Pointer to Item for easier manipulation
    BOOLEAN hasItem;

    int itemType; // Type of item in the brick
    BOOLEAN isHit;

    DWORD bounceStart;
    BOOLEAN isBouncingFinished;

    LPGAMEOBJECT Koopa;

    int activationCount;    
    int maxActivations;

public:
    CQuestionBrick(float x, float y, int type, int itemType, int maxActivations = 1) : CBrick(x, y, type)
    {
        item = nullptr;
        hasItem = false;
        this->itemType = itemType;

        isHit = false;

        bounceStart = 0;
        isBouncingFinished = false;

        Koopa = nullptr;

		activationCount = 0;
		this->maxActivations = maxActivations;
    }
    void Render();
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);

    void OnNoCollision(DWORD dt) {
        return;
    }

    void OnCollisionWith(LPCOLLISIONEVENT e);

    BOOLEAN IsHit() { return isHit; }

    void SetState(int state);
    void Activate();
    void ActivateItem();
	void SetKoopa(LPGAMEOBJECT koopa) { Koopa = koopa; }

    CMario* GetPlayer(); 
};