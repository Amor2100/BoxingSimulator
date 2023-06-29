#include "imgui.h"
#include "Boxer.h"
#include "Input/Input.h"
#include "camera.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include <cstdlib>
#include <ctime>

static Boxer* instance = nullptr;

Boxer& Boxer::Instance()
{
	return *instance;
}

Boxer::Boxer()
{
	srand((unsigned int)time(NULL));
	instance = this;
	model = std::make_unique<Model>("Data/Model/Robot/boxerfullanim.mdl");
	angle.y = StartAngleY;
	position.z = StartPosZ;
	scale.x = scale.y = scale.z = BoxerScale;
	hitEffect = std::make_unique<Effect>("Data/Effect/BlueSpark3.efk");
	circle = std::make_unique<Effect>("Data/Effect/BlueShield.efk");
	blueCircle = std::make_unique<Effect>("Data/Effect/BlueCircle.efk");
	TransitionIdleState();
}


Boxer::~Boxer()
{
	//delete model;
	//delete hitEffect;
	//delete circle;
}


void Boxer::Update(float elapsedTime)
{
	switch (state)
	{
	case State::Idle:
		UpdateIdleState(elapsedTime);
		statePlayerAttackIndicator = 0;
		break;
	case State::Move:
		UpdateMoveState(elapsedTime);
		statePlayerAttackIndicator = 0;
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		statePlayerAttackIndicator = 0;
		break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		statePlayerAttackIndicator = 1;
		break;
	case State::SecondAttack:
		UpdateSecondAttackState(elapsedTime);
		statePlayerAttackIndicator = 2;
		break;
	case State::ThirdAttack:
		UpdateThirdAttackState(elapsedTime);
		statePlayerAttackIndicator = 3;
		break;
	case State::HeadAttack:
		UpdateHeadAttackState(elapsedTime);
		statePlayerAttackIndicator = 4;
		break;
	case State::KneeAttack:
		UpdateKneeAttackState(elapsedTime);
		statePlayerAttackIndicator = 5;
		break;
	case State::Blocking:
		UpdateBlockingState(elapsedTime);
		statePlayerAttackIndicator = 6;
		break;
	case State::Blocked:
		UpdateBlockedState(elapsedTime);
		statePlayerAttackIndicator = 7;
		break;
	case State::Knocked:
		UpdateKnockedState(elapsedTime);
		statePlayerAttackIndicator = 0;
		break;
	}

	CollisionPlayerVsEnemies();
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	if (!Stunned)
	{
		if (!EnemyKnockedOut && !PlayerKnockedOut && startTimer <= 0.0f)
		{
			if (stopTimer == 0.0f)
			{
				Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
			}
		}
	}
	if (playerStam > playerMaxStamina)
	{
		playerStam = playerMaxStamina;
	}
	if (EnemyKnockedOut)
	{
		// 最後攻撃
		if (lastHit == 0)
		{
			lastHit = 1;
		}
	}
	if (lastHit == 1)
	{
		AddHitStop(0.8f, 0.0f);
		lastHit = 2;
	}

	UpdateVelocity(elapsedTime);
	UpdateInvicibleTimer(elapsedTime);
	UpdateTransform();
	UpdateHitStop(elapsedTime);
	model->UpdateTransform(transform);
	model->UpdateAnimation(elapsedTime);
}


bool Boxer::InputMove(float elapsedTime)
{
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	Move(moveVec.x, moveVec.z, moveSpeed);
	//Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
	return moveVec.x != .0f || moveVec.y != .0f || moveVec.z != .0f;
}


bool Boxer::InputAttack()
{
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		return true;
	}
	return false;
}

bool Boxer::InputHeadAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (GetAsyncKeyState('C') & 0x8000)
	{
		return true;
	}
	return false;
}

bool Boxer::InputKneeAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (GetAsyncKeyState('V') & 0x8000)
	{
		return true;
	}
	return false;
}



void Boxer::TransitionIdleState()
{
	PlayerHitFailed = false;
	state = State::Idle;
	model->PlayAnimation(Idle, true);
}


void Boxer::UpdateIdleState(float elapsedTime)
{
	startTimer -= 0.01f;
	if (startTimer <= 0.0f)
	{
		startTimer = 0.0f;
		if (!EnemyKnockedOut)
		{
			if (!PlayerKnockedOut)
			{
				playerStam += idleStaminaIncrease;
				afkTime -= elapsedTime;
				shieldBroke -= elapsedTime;
				if (afkTime <= 0.0f)
				{
					cameraShake = false;
					Stunned = false;
					afkTime = 0.0f;
					if (InputMove(elapsedTime))
					{
						TransitionMoveState();
						//return;
					}
					if (playerStam >= firstAttackCost)														// Player Attack
					{
						if (InputAttack())
						{
							TransitionAttackState();
						}
					}
					if (playerStam >= headAttackCost)
					{
						if (InputHeadAttack())
						{
							TransitionHeadAttackState();
						}
					}
					if (playerStam >= kneeAttackCost)
					{
						if (InputKneeAttack())
						{
							TransitionKneeAttackState();
						}
					}
					if (PlayerBlock)																	// Player Defense
					{
						if (LowestStamDamage >= 5.0f)													// 最低スタミナダメージ
						{
							TransitionBlockingState();
						}
					}
					Mouse& mouse = Input::Instance().GetMouse();
					if (shieldBroke <= 0.0f)
					{
						if (GetKeyState(VK_RBUTTON) & 0x800)
						{
							if (playerStam >= MinStamBlock)
							{
								PlayerBlock = true;
							}
						}
						else
						{
							PlayerBlock = false;
						}
					}
				}
			}
			if (shieldBroke <= 0.0f)
			{
				shieldBroke = 0.0f;
			}
			if (playerStam <= 0.0f)
			{
				playerStam = 0;
				PlayerBlock = false;
			}
			if (health <= 0)
			{
				TransitionKnockedState();
			}
		}
	}
}

void Boxer::TransitionMoveState()
{
	PlayerHitFailed = false;
	state = State::Move;
	model->PlayAnimation(Walk, true);
}

void Boxer::UpdateMoveState(float elapsedTime)
{
	playerStam += moveStaminaIncrease;
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	if (!PlayerKnockedOut)
	{
		if (!InputMove(elapsedTime))
		{
			TransitionIdleState();
		}
	}

	if (playerStam >= firstAttackCost)														// Player Attack
	{
		if (InputAttack())
		{
			TransitionAttackState();
		}
	}
	if (playerStam >= headAttackCost)
	{
		if (InputHeadAttack())
		{
			TransitionHeadAttackState();
		}
	}
	if (playerStam >= kneeAttackCost)
	{
		if (InputKneeAttack())
		{
			TransitionKneeAttackState();
		}
	}
	if (PlayerBlock)																	// Player Defense
	{
		if (playerStam >= LowestStamDamage)
		{
			TransitionBlockingState();
		}
	}
	Mouse& mouse = Input::Instance().GetMouse();
	if (GetKeyState(VK_RBUTTON) & 0x800)
	{
		if (playerStam >= MinStamBlock)
		{
			PlayerBlock = true;
		}
	}
	else
	{
		PlayerBlock = false;
	}
	if (playerStam <= 0.0f)
	{
		playerStam = 0;
		PlayerBlock = false;
	}
	if (health <= 0)
	{
		TransitionKnockedState();
	}
}

void Boxer::OnDamaged()
{
	TransitionDamageState();
}

void Boxer::TransitionDamageState()
{
	playerStam += hitStaminaIncrease;
	state = State::Damage;
	switch (enemyAttackState)
	{
	case 1:
		model->PlayAnimation(LightHit, false);				//1st Attack
		break;
	case 2:
		model->PlayAnimation(KidneyHit, false);				//2nd Attack
		break;
	case 3:
		model->PlayAnimation(BodyHit, false);				//3rd Attack
		break;
	case 4:																					//Mini Stun
		if (!Stunned)
		{
			afkTime = 1.0f;
			Stunned = true;
		}
		cameraShake = true;
		model->PlayAnimation(HitToHead, false);				//4th Attack
		break;
	case 5:
		model->PlayAnimation(BodyHit, false);				//5th Attack
		break;
	}
}


void Boxer::UpdateDamageState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	playerStam += hitStaminaIncrease;
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
	if (health <= 0)
	{
		TransitionKnockedState();
	}
}

void Boxer::TransitionAttackState()
{
	playerStam -= firstAttackCost;
	randomNumber = 1 + rand() % 100;
	PlayerHitFailed = false;
	nextAttack = false;
	state = State::Attack;
	model->PlayAnimation(LeadJab, false);
}

void Boxer::UpdateAttackState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	if (InputAttack())
	{
		nextAttack = true;
	}
	if (!model->IsPlayAnimation())
	{
		if (!nextAttack)
		{
			TransitionIdleState();
		}
		else
		{
			if (playerStam >= secondAttackCost)
			{
				TransitionSecondAttackState();
			}
			else
			{
				TransitionIdleState();
			}
		}
	}

	float animationTime = model->GetCurrentAnimationSeconds();
	attackCollisionLeftHandFlag = false;
	if (animationTime >= 0.18f && animationTime <= 0.3f)
	{
		attackCollisionLeftHandFlag = true;
	}
	else
	{
		attackCollisionLeftHandFlag = false;
	}

	if (attackCollisionLeftHandFlag)
	{
		CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
	}
	if (health <= 0)
	{
		TransitionKnockedState();
	}
}

void Boxer::TransitionSecondAttackState()
{
	playerStam -= secondAttackCost;
	randomNumber = 1 + rand() % 100;
	PlayerHitFailed = false;
	nextAttack = false;
	state = State::SecondAttack;
	model->PlayAnimation(ElbowPunch, false);
}

void Boxer::UpdateSecondAttackState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	if (!model->IsPlayAnimation())
	{
		if (!nextAttack)
		{
			TransitionIdleState();
		}
		else
		{
			if (playerStam >= thirdAttackCost)
			{
				TransitionThirdAttackState();
			}
			else
			{
				TransitionIdleState();
			}
		}
	}

	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime > 0.2f)
	{
		if (InputAttack())
		{
			nextAttack = true;
		}
	}
	if (animationTime >= 0.40f && animationTime <= 0.52f)
	{
		attackCollisionRightHandFlag = true;
	}
	else
	{
		attackCollisionRightHandFlag = false;
	}

	if (attackCollisionRightHandFlag)
	{
		CollisionNodeVsEnemies("mixamorig:RightForeArm", leftHandRadius);
	}
	if (health <= 0)
	{
		TransitionKnockedState();
	}
}

void Boxer::TransitionThirdAttackState()
{
	playerStam -= thirdAttackCost;
	randomNumber = 1 + rand() % 100;
	PlayerHitFailed = false;
	nextAttack = false;
	state = State::ThirdAttack;
	model->PlayAnimation(Hook, false);
}

void Boxer::UpdateThirdAttackState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}

	float animationTime = model->GetCurrentAnimationSeconds();
	attackCollisionLeftHandFlag = false;
	if (animationTime >= 0.58f && animationTime <= 0.7f)
	{
		attackCollisionLeftHandFlag = true;
	}
	else
	{
		attackCollisionLeftHandFlag = false;
	}

	if (attackCollisionLeftHandFlag)
	{
		CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
	}
	if (health <= 0)
	{
		TransitionKnockedState();
	}
}

void Boxer::TransitionHeadAttackState()
{
	playerStam -= headAttackCost;
	randomNumber = 1 + rand() % 100;
	PlayerHitFailed = false;
	state = State::HeadAttack;
	model->PlayAnimation(Headbutt, false);
}

void Boxer::UpdateHeadAttackState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}

	float animationTime = model->GetCurrentAnimationSeconds();
	attackCollisionHeadFlag = false;
	if (animationTime >= 0.44f && animationTime <= 0.65f)
	{
		attackCollisionHeadFlag = true;
	}
	else
	{
		attackCollisionHeadFlag = false;
	}

	if (attackCollisionHeadFlag)
	{
		CollisionNodeVsEnemies("mixamorig:HeadTop_End", leftHandRadius);
	}
	if (health <= 0)
	{
		TransitionKnockedState();
	}
}

void Boxer::TransitionKneeAttackState()
{
	playerStam -= kneeAttackCost;
	randomNumber = 1 + rand() % 100;
	PlayerHitFailed = false;
	state = State::KneeAttack;
	model->PlayAnimation(IllegalKnee, false);
}
void Boxer::UpdateKneeAttackState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}

	float animationTime = model->GetCurrentAnimationSeconds();
	attackCollisionKneeFlag = false;
	if (animationTime >= 0.43f && animationTime <= 0.54f)
	{
		attackCollisionKneeFlag = true;
	}
	else
	{
		attackCollisionKneeFlag = false;
	}

	if (attackCollisionKneeFlag)
	{
		CollisionNodeVsEnemies("mixamorig:RightLeg", leftHandRadius);
	}
	if (health <= 0)
	{
		TransitionKnockedState();
	}
}

void Boxer::TransitionBlockingState()
{
	//circle->Play(DirectX::XMFLOAT3(position.x, position.y, position.z), 0.4f);
	state = State::Blocking;
	model->PlayAnimation(Idle, true);
}
void Boxer::UpdateBlockingState(float elapsedTime)
{
	Mouse& mouse = Input::Instance().GetMouse();
	if (GetKeyState(VK_RBUTTON) & 0x800)
	{
		if (playerStam >= MinStamBlock)
		{
			PlayerBlock = true;
		}
	}
	else
	{
		PlayerBlock = false;
	}
	if (playerStam <= 0.0f)
	{
		playerStam = 0;
		PlayerBlock = false;
	}
	if (EnemyHitFailed)
	{
		TransitionBlockedState();
	}

	if (!PlayerBlock)
	{
		TransitionIdleState();
	}
	if (health <= 0)
	{
		TransitionKnockedState();
	}
}

void Boxer::TransitionBlockedState()
{
	//circle->Play(DirectX::XMFLOAT3(position.x, position.y, position.z), 0.4f);
	switch (enemyAttackState)
	{
	case 1:
		playerStam -= LowestStamDamage;				//1st Attack
		break;
	case 2:
		playerStam -= LowestStamDamage;				//2nd Attack
		break;
	case 3:
		playerStam -= HighestStamDamage;			//3rd Attack
		break;
	case 4:
		playerStam -= LowestStamDamage;				//4th Attack
		break;
	case 5:
		PlayerBlock = false;				//5th Attack
		shieldBroke = 1.0f;
		break;
	}
	state = State::Blocked;
	model->PlayAnimation(CenterBlock, false);
}
void Boxer::UpdateBlockedState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		if (PlayerBlock)
		{
			TransitionBlockingState();
		}
		else
		{
			TransitionIdleState();
		}
	}
	if (playerStam <= 0.0f)
	{
		playerStam = 0;
	}
	if (health <= 0)
	{
		TransitionKnockedState();
	}
}

void Boxer::TransitionKnockedState()
{
	state = State::Knocked;
	model->PlayAnimation(Knocked, false);
	AddHitStop(0.8f, 0.0f);
}
void Boxer::UpdateKnockedState(float elapsedTime)
{
	PlayerKnockedOut = true;
}



void Boxer::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	Model::Node* Hips = model->FindNode("mixamorig:Hips");
	debugRenderer->DrawCylinder(DirectX::XMFLOAT3(Hips->worldTransform._41, Hips->worldTransform._42 - 1.0f, Hips->worldTransform._43), radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	if (attackCollisionLeftHandFlag)
	{
		Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
		debugRenderer->DrawSphere(DirectX::XMFLOAT3(leftHandBone->worldTransform._41, leftHandBone->worldTransform._42, leftHandBone->worldTransform._43), leftHandRadius, DirectX::XMFLOAT4(1, 0, 0, 1));
	}
	if (attackCollisionRightHandFlag)
	{
		Model::Node* leftHandBone = model->FindNode("mixamorig:RightForeArm");
		debugRenderer->DrawSphere(DirectX::XMFLOAT3(leftHandBone->worldTransform._41, leftHandBone->worldTransform._42, leftHandBone->worldTransform._43), leftHandRadius, DirectX::XMFLOAT4(1, 0, 0, 1));
	}
	if (attackCollisionHeadFlag)
	{
		Model::Node* leftHandBone = model->FindNode("mixamorig:HeadTop_End");
		debugRenderer->DrawSphere(DirectX::XMFLOAT3(leftHandBone->worldTransform._41, leftHandBone->worldTransform._42, leftHandBone->worldTransform._43), leftHandRadius, DirectX::XMFLOAT4(1, 0, 0, 1));
	}
	if (attackCollisionKneeFlag)
	{
		Model::Node* leftHandBone = model->FindNode("mixamorig:RightLeg");
		debugRenderer->DrawSphere(DirectX::XMFLOAT3(leftHandBone->worldTransform._41, leftHandBone->worldTransform._42, leftHandBone->worldTransform._43), leftHandRadius, DirectX::XMFLOAT4(1, 0, 0, 1));
	}
}



DirectX::XMFLOAT3 Boxer::GetMoveVec() const
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();


	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();





	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

	if (cameraRightLength > 0.0f)
	{
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}


	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}





	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	vec.y = 0.0f;
	return vec;
}


void Boxer::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectCylinderVsCylinder(position, radius, height, enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(), outPosition))
		{
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);

			if (normal.y > 0.8f)
			{
				Jump(jumpSpeed * 0.5f);
			}

			else
			{
				enemy->SetPosition(outPosition);
			}
		}
	}
}


void Boxer::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
	Model::Node* node = model->FindNode(nodeName);

	DirectX::XMFLOAT3 nodePosition;
	nodePosition.x = node->worldTransform._41;
	nodePosition.y = node->worldTransform._42;
	nodePosition.z = node->worldTransform._43;

	EnemyManager& enemyManager = EnemyManager::Instance();


	int enemyCount = enemyManager.GetEnemyCount();
	for (int j = 0; j < enemyCount; ++j)
	{
		Enemy* enemy = enemyManager.GetEnemy(j);
		DirectX::XMFLOAT3 outPosition;
		if (!EnemyKnockedOut)
		{
			if (Collision::IntersectSphereVsCylinder(nodePosition, 0.15f, enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(), outPosition))
			{
				if (EnemyBlock)
				{
					PlayerHitFailed = true;
				}
				else
				{
					switch (statePlayerAttackIndicator)							// DAMAGE CHANGE
					{
					case 1:
						lastDamage = firstAttackDamage - (enemyDef * 6.0f);
						break;
					case 2:
						lastDamage = secondAttackDamage - (enemyDef * 6.0f);
						break;
					case 3:
						lastDamage = thirdAttackDamage - (enemyDef * 10.0f);
						break;
					case 4:
						lastDamage = headAttackDamage - (enemyDef * 8.0f);
						break;
					case 5:
						lastDamage = kneeAttackDamage - (enemyDef * 5.0f);
						break;
					}
					if (enemy->ApplyDamage(lastDamage, 0.5f))
					{
						switch (statePlayerAttackIndicator)						// HIT STOP EFFECT
						{
						case 1:
							break;
						case 2:
							break;
						case 3:
							break;
						case 4:
							AddHitStop(0.3f, 0.0f);
							break;
						case 5:
							AddHitStop(0.3f, 0.2f);
							break;
						}
						InputCombo();
						{
							DirectX::XMFLOAT3 impulse;							// ADD IMPULSE
							const float power = 3.0f;
							const DirectX::XMFLOAT3& e = enemy->GetPosition();
							const DirectX::XMFLOAT3& p = nodePosition;
							float vx = e.x - p.x;
							float vz = e.z - p.z;
							float length = sqrtf(vx * vx + vz * vz);
							vx /= length;
							vz /= length;
							impulse.x = vx * power;
							impulse.y = 0.5f * power;
							impulse.z = vz * power;

							enemy->AddImpulse(impulse);
						}
					}
				}
				{
					DirectX::XMFLOAT3 e = enemy->GetPosition();
					e.y += enemy->GetHeight() * 0.8f;
					hitEffect->Play(e, 0.1f);
				}
			}
		}

	}
}

void Boxer::DrawDebugUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			ImGui::InputFloat3("Position", &position.x);
			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//スケール
			ImGui::InputFloat3("Scale", &scale.x);
			ImGui::InputFloat("Health", &health);
			ImGui::InputFloat("shieldBroke", &shieldBroke);
			ImGui::InputFloat("EnemyDef", &enemyDef);
			ImGui::InputFloat("playerMaxStamina", &playerMaxStamina);
			ImGui::InputFloat("PlayerDef", &playerDef);
			ImGui::InputFloat("PlayerFirstAtk", &firstAttackDamage);
			ImGui::InputFloat("PlayerStam", &playerStam);
			ImGui::InputFloat("AFK", &afkTime);
			ImGui::InputInt("Combo", &combo);
			ImGui::InputInt("Randomnumber", &randomNumber);
			ImGui::InputInt("AttackState", &statePlayerAttackIndicator);
			ImGui::Checkbox("LeftAttack", &attackCollisionLeftHandFlag);
			ImGui::Checkbox("RightAttack", &attackCollisionRightHandFlag);
			ImGui::Checkbox("EnemyBlock", &EnemyBlock);
			ImGui::Checkbox("PlayerBlock", &PlayerBlock);
		}
	}
	ImGui::End();
}

void Boxer::InputCombo()
{
	combo += 1;
}