#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "character.h"
#include "Effect.h"

class Boxer : public Character
{
public:
	Boxer();
	~Boxer() override;

	void Update(float elapsedTime);

	void DrawDebugUI();

	//void Render(const RenderContext& rc, ModelShader* shader);

	void DrawDebugPrimitive();

	static Boxer& Instance();

	std::unique_ptr<Model> model = nullptr;

protected:


private:

	bool InputMove(float elapsedTime);
	DirectX::XMFLOAT3 GetMoveVec() const;
	float moveSpeed = 2.5f;

	void CollisionPlayerVsEnemies();

	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	bool InputAttack();
	bool InputHeadAttack();
	bool InputKneeAttack();

	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 20.0f;

	float landingFallSpeed = gravity * 5.0f;

	// Effect
	std::unique_ptr<Effect> hitEffect = nullptr;
	std::unique_ptr<Effect> trails = nullptr;
	std::unique_ptr<Effect> circle = nullptr;
	std::unique_ptr<Effect> blueCircle = nullptr;

	// Transition State
	void TransitionIdleState();
	void TransitionMoveState();
	void TransitionDamageState();
	void TransitionAttackState();
	void TransitionSecondAttackState();
	void TransitionThirdAttackState();
	void TransitionHeadAttackState();
	void TransitionKneeAttackState();
	void TransitionBlockingState();
	void TransitionBlockedState();
	void TransitionKnockedState();

	// Update State
	void UpdateIdleState(float elapsedTime);
	void UpdateMoveState(float elapsedTime);
	void UpdateDamageState(float elapsedTime);
	void UpdateAttackState(float elapsedTime);
	void UpdateSecondAttackState(float elapsedTime);
	void UpdateThirdAttackState(float elapsedTime);
	void UpdateHeadAttackState(float elapsedTime);
	void UpdateKneeAttackState(float elapsedTime);
	void UpdateBlockingState(float elapsedTime);
	void UpdateBlockedState(float elapsedTime);
	void UpdateKnockedState(float elapsedTime);

	void InputCombo();

	bool GetKnocked() { return this->Knocked; }
	void OnDamaged() override;

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
private:
	enum class State
	{
		Idle,
		Move,
		Damage,
		Attack,
		SecondAttack,
		ThirdAttack,
		HeadAttack,
		KneeAttack,
		Blocking,
		Blocked,
		Knocked,
	};
private:
	State state = State::Idle;

	float leftHandRadius = 0.15f;

	bool attackCollisionLeftHandFlag = false;
	bool attackCollisionRightHandFlag = false;
	bool attackCollisionHeadFlag = false;
	bool attackCollisionKneeFlag = false;

	bool nextAttack = false;
	DirectX::XMFLOAT3 hitPosition = { 0, 0, 0 };
	int combo = 0;
	float lastDamage = 0;
	float afkTime = 0.0f;

	// マジックナンバー対策
	float StartAngleY = 59.8f;
	float StartPosZ = 3.0f;
	float BoxerScale = 0.01f;
	float MinStamBlock = 20.0f;
	float LowestStamDamage = 5.0f;
	float HighestStamDamage = 10.0f;
};