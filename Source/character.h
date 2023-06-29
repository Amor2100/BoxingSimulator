#pragma once

#include <DirectXMath.h>

class Character
{
public:
	Character() {};
	virtual ~Character() {};
	void UpdateTransform();

	const DirectX::XMFLOAT3& GetPosition() const { return position; };
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; };
	const DirectX::XMFLOAT3& GetAngle() const { return angle; };
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; };
	const DirectX::XMFLOAT3& GetScale() const { return scale; };
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; };

	float GetRadius() const { return radius; };

	bool IsGround() const { return isGround; };

	float GetHeight() const { return height; };


	bool ApplyDamage(float damage, float invicibleTime);

	void AddImpulse(const DirectX::XMFLOAT3& impulse);

	int GetHealth() const { return health; }
	int GetStamina() const { return playerStam; }

	int GetMaxHealth() const { return maxhealth; }
	int GetMaxStamina() const { return playerMaxStamina; }

	int SetPlayerAttackState(int atkState) { return this->playerAttackState = atkState; }
	int SetEnemyAttackState(int atkState) { return this->enemyAttackState = atkState; }
	int GetPlayerAttackState() { return this->statePlayerAttackIndicator; }
	int GetEnemyAttackState() { return this->stateEnemyAttackIndicator; }

	bool GetPlayerBlockState() { return this->PlayerBlock; }
	bool GetEnemyBlockState() { return this->EnemyBlock; }
	bool SetPlayerBlockState(bool blockState) { return this->PlayerBlock = blockState; }
	bool SetEnemyBlockState(bool blockState) { return this->EnemyBlock = blockState; }


	bool GetPlayerHitFailedState() { return this->PlayerHitFailed; }
	bool GetEnemyHitFailedState() { return this->EnemyHitFailed; }
	bool SetPlayerHitFailedState(bool hitFailed) { return this->PlayerHitFailed = hitFailed; }
	bool SetEnemyHitFailedState(bool hitFailed) { return this->EnemyHitFailed = hitFailed; }

	bool GetPlayerKnockedState() { return this->PlayerKnockedOut; }
	bool GetEnemyKnockedState() { return this->EnemyKnockedOut; }
	bool SetPlayerKnockedState(bool hitFailed) { return this->PlayerKnockedOut = hitFailed; }
	bool SetEnemyKnockedState(bool hitFailed) { return this->EnemyKnockedOut = hitFailed; }

	int GetPlayerRandomNumber() { return this->randomNumber; }
	int SetEnemyRandomNumber(int randomnum) { return this->enemyRandomNumber = randomnum; }

	void SetPlayerInfo(float healthInc, float def, float atkdmg, float stam) { this->health = (health + (health * 0.1f * healthInc)), this->maxhealth = health, this->playerDef = (0.1f * def), this->firstAttackDamage = (firstAttackDamage + (firstAttackDamage * 0.1f * atkdmg)), this->secondAttackDamage = (secondAttackDamage + (secondAttackDamage * 0.1f * atkdmg)), this->thirdAttackDamage = (thirdAttackDamage + (thirdAttackDamage * 0.1f * atkdmg)), this->headAttackDamage = (headAttackDamage + (headAttackDamage * 0.1f * atkdmg)), this->kneeAttackDamage = (kneeAttackDamage + (kneeAttackDamage * 0.1f * atkdmg)), this->playerMaxStamina = (playerMaxStamina + (playerMaxStamina * 0.1f * stam)), this->playerStam = playerMaxStamina, this->idleStaminaIncrease = (idleStaminaIncrease + (idleStaminaIncrease * 0.1f * stam)), this->moveStaminaIncrease = (moveStaminaIncrease + (moveStaminaIncrease * 0.1f * stam)), this->hitStaminaIncrease = (hitStaminaIncrease + (hitStaminaIncrease * 0.1f * stam)); };
	void SetEnemyInfo(float healthInc, float def, float atkdmg, float stam) { this->health = (health + (health * 0.1f * healthInc)), this->maxhealth = health, this->enemyDef = (0.1f * def), this->firstAttackDamage = (firstAttackDamage + (firstAttackDamage * 0.1f * atkdmg)), this->secondAttackDamage = (secondAttackDamage + (secondAttackDamage * 0.1f * atkdmg)), this->thirdAttackDamage = (thirdAttackDamage + (thirdAttackDamage * 0.1f * atkdmg)), this->headAttackDamage = (headAttackDamage + (headAttackDamage * 0.1f * atkdmg)), this->kneeAttackDamage = (kneeAttackDamage + (kneeAttackDamage * 0.1f * atkdmg)), this->playerMaxStamina = (playerMaxStamina + (playerMaxStamina * 0.1f * stam)), this->playerStam = playerMaxStamina, this->idleStaminaIncrease = (idleStaminaIncrease + (idleStaminaIncrease * 0.1f * stam)), this->moveStaminaIncrease = (moveStaminaIncrease + (moveStaminaIncrease * 0.1f * stam)), this->hitStaminaIncrease = (hitStaminaIncrease + (hitStaminaIncrease * 0.1f * stam)); };
	float SetEnemyDifficulty(float difficulty) { return this->enemyDifficulty = difficulty; }

	float SetEnemyDefense(float defense) { return this->enemyDef = defense; }
	float SetPlayerDefense(float defense) { return this->playerDef = defense; }
	float GetPlayerDefense() { return this->playerDef; }
	float GetEnemyDefense() { return this->enemyDef; }

	void SetShieldPosition(const DirectX::XMFLOAT3& position) { this->targetPosition = position; };
	bool GetStunedState() { return this->Stunned; }
	float GetShieldBroke() { return this->shieldBroke; }
	bool GetStartTimer() { return this->startTimer; }

	bool GetCameraShake() { return this->cameraShake; }
	void AddHitStop(float time, float speed);

	void CagingPosition();

protected:
	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	angle = { 0, 0, 0 };
	DirectX::XMFLOAT3	scale = { 0, 0, 0 };
	DirectX::XMFLOAT4X4	transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	float radius = 0.25f;
	virtual void OnLanding() {};
	float height = 1.7f;
	void UpdateInvicibleTimer(float elapsedTime);


	virtual void OnDamaged() {};
	virtual void OnDead() {};
protected:
	//void Move(float elapsedTime, float vx, float vz, float speed);
	void Move(float vx, float vz, float speed);
	void Turn(float elapsedTime, float vx, float vz, float speed);
	void Jump(float speed);
	void UpdateVelocity(float elapsedTime);

	float gravity = -1.0f;
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	bool isGround = false;
	float health = 100;
	float maxhealth = 100;
	float invicibleTimer = 1.0f;
	float friction = 0.5f;

	float acceleration = 1.0f;
	float maxMoveSpeed = 5.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;

	float airControl = 0.3f;

	float stepOffset = 1.0f;

	float slopeRate = 1.0f;

	int Health = 5;
	int maxHealth = 5;

	int playerAttackState = 0;
	int enemyAttackState = 0;
	int statePlayerAttackIndicator = 0;
	int stateEnemyAttackIndicator = 0;

	int randomNumber;
	int enemyRandomNumber;

	bool PlayerBlock = false;
	bool EnemyBlock = false;

	bool PlayerHitFailed = false;
	bool EnemyHitFailed = false;

	bool PlayerKnockedOut = false;
	bool EnemyKnockedOut = false;

	// Attack Stamina Cost
	float firstAttackCost = 15;
	float secondAttackCost = 10;
	float thirdAttackCost = 5;
	float headAttackCost = 40;
	float kneeAttackCost = 20;
	float idleStaminaIncrease = 0.7f;
	float moveStaminaIncrease = 0.3f;
	float hitStaminaIncrease = 0.7f;

	// PLAYER DAMAGE
	float firstAttackDamage = 6.0f;
	float secondAttackDamage = 6.0f;
	float thirdAttackDamage = 10.0f;
	float headAttackDamage = 8.0f;
	float kneeAttackDamage = 5.0f;

	// PLAYER INFO
	float playerDef = 0;
	float enemyDef = 0;
	float playerMaxStamina = 100;
	float playerStam = playerMaxStamina;

	// Enemy Difficulty
	int enemyDifficulty = 0;

	// Shield
	DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };

	// Stun
	bool Stunned = false;
	float shieldBroke = 0.0f;

	// StartTimer
	float startTimer = 3.0f;

	// Shake
	bool cameraShake = false;

	// Hit Stop
	void UpdateHitStop(float& elapsedTime);
	float stopTimer = 0.0f;
	float timeScale = 1.0f;
	int lastHit = 0;

private:
	void UpdateVerticalVelocity(float elapsedFrame);
	void UpdateVerticalMove(float elapsedTime);
	void UpdateHorizontalVelocity(float elapsedFrame);
	void UpdateHorizontalMove(float elapsedTime);
};