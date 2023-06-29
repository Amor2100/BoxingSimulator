#pragma once
#include "Graphics/Model.h"
#include "Enemy.h"
#include "Effect.h"

class BoxerX : public Enemy
{
public:
	BoxerX();
	~BoxerX() override;

	void Update(float elapsedTime) override;
	//void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	void DrawDebugPrimitive() override;
	void DrawDebugUI();

	std::unique_ptr<Model> model = nullptr;

private:

	void TransitionIdleState();

	void UpdateIdleState(float elapsedTime);

	void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

	// Transition State
	void TransitionDamageState();
	void TransitionDeathState();
	void TransitionBlockingState();
	void TransitionBlockedState();
	void TransitionPursuitState();
	void TransitionFirstAttackState();
	void TransitionSecondAttackState();
	void TransitionThirdAttackState();
	void TransitionHeadAttackState();
	void TransitionKneeAttackState();
	void TransitionRandomState();

	// Update State
	void UpdateDamageState(float elapsedTime);
	void UpdateDeathState(float elapsedTime);
	void UpdateBlockingState(float elapsedTime);
	void UpdateBlockedState(float elapsedTime);
	void UpdatePursuitState(float elapsedTime);
	void UpdateFirstAttackState(float elapsedTime);
	void UpdateSecondAttackState(float elapsedTime);
	void UpdateThirdAttackState(float elapsedTime);
	void UpdateHeadAttackState(float elapsedTime);
	void UpdateKneeAttackState(float elapsedTime);

	void MoveToTarget(float elapsedTime, float speedRate);

	int numberGenerator() {};
	void InsertRandomNumber();



private:

	enum class State
	{
		Idle,
		Death,
		Damage,
		Blocking,
		Blocked,
		Pursuit,
		FirstAttack,
		SecondAttack,
		ThirdAttack,
		HeadAttack,
		KneeAttack,
	};

	enum Animation
	{
		Idle,
		Walk,
		Left,
		Right,
		Backward,
		Headbutt,
		LeadJab,
		ElbowPunch,
		Hook,
		CenterBlock,
		HitToHead,
		KidneyHit,
		BodyHit,
		IllegalKnee,
		Knocked,
		LightHit,
	};


protected:
	void OnDamaged() override;

private:
	State state = State::Idle;
	DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };
	float turnSpeed = DirectX::XMConvertToRadians(360);

	float moveSpeed = 2.2f;
	float attackRange = 1.0f;
	float afkTime = 1.0f;
	float leftHandRadius = 0.15f;
	Effect* hitEffect = nullptr;
	int randomNumber = 0;
	int insertedNumber = 0;
	float BlockTime = 1.0f;
	int playerStateBlocker = 0;
	float lastDamage = 0;

	// COUNTER ATTACK
	float counterBlock = 0.7f;
	bool CounterAttack = false;
	bool CounterAttackDone = false;

	// AFK Time
	float lowest = 0.5f;
	float highest = 2.5f;
	float afterAttackLowest = 0.6f;
	float afterAttackHighest = 0.9f;

	// マジックナンバー対策
	float BoxerScale = 0.01f;
	float BoxerHeight = 1.7f;
	float LowestStamDamage = 5.0f;
	float HighestStamDamage = 10.0f;
	float Chance50 = 50.0f;
	float Chance60 = 60.0f;
	float StamNeededToBlock = 30.0f;
};