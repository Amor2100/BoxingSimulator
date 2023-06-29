#include "BoxerX.h"
#include "Graphics/Graphics.h"
#include"Mathf.h"
#include "Boxer.h"
#include "Collision.h"
#include "Input/Input.h"
#include <cstdlib>
#include <ctime>



BoxerX::BoxerX()
{
	model = std::make_unique<Model>("Data/Model/BoxerX/boxerXwithDamage.mdl");
	scale.x = scale.y = scale.z = BoxerScale;
	radius = 0.25f;
	height = BoxerHeight;
	srand((unsigned int)time(NULL));
	EnemyBlock = true;
	hitEffect = new Effect("Data/Effect/RedSpark3.efk");
	TransitionIdleState();
	InsertRandomNumber();
}

BoxerX::~BoxerX()
{
	//delete hitEffect;
	//delete model;
}

void BoxerX::Update(float elapsedTime)
{
	switch (state)
	{
	case State::Idle:
		UpdateIdleState(elapsedTime);
		stateEnemyAttackIndicator = 0;
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		stateEnemyAttackIndicator = 0;
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		stateEnemyAttackIndicator = 0;
		break;
	case State::Blocking:
		UpdateBlockingState(elapsedTime);
		stateEnemyAttackIndicator = 0;
		break;
	case State::Blocked:
		UpdateBlockedState(elapsedTime);
		stateEnemyAttackIndicator = 0;
		break;
	case State::Pursuit:
		UpdatePursuitState(elapsedTime);
		stateEnemyAttackIndicator = 0;
		break;
	case State::FirstAttack:
		UpdateFirstAttackState(elapsedTime);
		stateEnemyAttackIndicator = 1;
		break;
	case State::SecondAttack:
		UpdateSecondAttackState(elapsedTime);
		stateEnemyAttackIndicator = 2;
		break;
	case State::ThirdAttack:
		UpdateThirdAttackState(elapsedTime);
		stateEnemyAttackIndicator = 3;
		break;
	case State::HeadAttack:
		UpdateHeadAttackState(elapsedTime);
		stateEnemyAttackIndicator = 4;
		break;
	case State::KneeAttack:
		UpdateKneeAttackState(elapsedTime);
		stateEnemyAttackIndicator = 5;
		break;
	}
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;
	Turn(elapsedTime, vx, vz, turnSpeed * 1.0f);

	randomNumber = 1 + rand() % 100;
	if (playerStam > playerMaxStamina)
	{
		playerStam = playerMaxStamina;
	}
	if (PlayerKnockedOut)
	{
		//　最後攻撃
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
	model->UpdateAnimation(elapsedTime);
	model->UpdateTransform(transform);
}

void BoxerX::OnDamaged()
{
	TransitionDamageState();
}

void BoxerX::DrawDebugPrimitive()
{
	Model::Node* Hips = model->FindNode("mixamorig:Hips");
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	debugRenderer->DrawCylinder(DirectX::XMFLOAT3(Hips->worldTransform._41, Hips->worldTransform._42 - 1.0f, Hips->worldTransform._43), radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	//debugRenderer->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));
}


void BoxerX::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
	Model::Node* node = model->FindNode(nodeName);
	if (node != nullptr)
	{
		DirectX::XMFLOAT3 nodePosition(
			node->worldTransform._41,
			node->worldTransform._42,
			node->worldTransform._43
		);
		Graphics::Instance().GetDebugRenderer()->DrawSphere(nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1));


		Boxer& player = Boxer::Instance();
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			nodePosition,
			nodeRadius,
			player.GetPosition(),
			player.GetRadius(),
			player.GetHeight(),
			outPosition
		))
		{
			if (PlayerBlock)
			{
				EnemyHitFailed = true;
			}
			else
			{
				switch (stateEnemyAttackIndicator)										// DAMAGE CHANGE
				{
				case 1:
					lastDamage = firstAttackDamage - (playerDef * 6.0f);
					break;
				case 2:
					lastDamage = secondAttackDamage - (playerDef * 6.0f);
					break;
				case 3:
					lastDamage = thirdAttackDamage - (playerDef * 10.0f);
					break;
				case 4:
					lastDamage = headAttackDamage - (playerDef * 8.0f);
					break;
				case 5:
					lastDamage = kneeAttackDamage - (playerDef * 5.0f);
					break;
				}
				if (player.ApplyDamage(lastDamage, 0.5f))								// ADD IMPULSE
				{
					DirectX::XMFLOAT3 vec;
					vec.x = outPosition.x - nodePosition.x;
					vec.z = outPosition.z - nodePosition.z;
					float length = sqrtf(vec.x * vec.x + vec.z * vec.z);
					vec.x /= length;
					vec.z /= length;

					float power = 3.0f;
					vec.x *= power;
					vec.z *= power;
					vec.y = 0.0f;

					player.AddImpulse(vec);
				}
			}
			{
				DirectX::XMFLOAT3 e = player.GetPosition();
				e.y += player.GetHeight() * 0.8f;
				hitEffect->Play(e, 0.075f);
			}
		}
	}
}

void BoxerX::MoveToTarget(float elapsedTime, float speedRate)
{
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	Move(vx, vz, moveSpeed * speedRate);
}

void BoxerX::TransitionRandomState()
{
	InsertRandomNumber();
}

void BoxerX::TransitionIdleState()
{
	EnemyBlock = false;
	EnemyHitFailed = false;
	state = State::Idle;

	model->PlayAnimation(Idle, true);
}

void BoxerX::UpdateIdleState(float elapsedTime)
{
	startTimer -= 0.01f;
	if (startTimer <= 0.0f)
	{
		startTimer = 0.0f;
		playerStam += idleStaminaIncrease;
		afkTime -= elapsedTime;
		shieldBroke -= elapsedTime;
		if (afkTime <= 0.0f)
		{
			Stunned = false;
			if (!PlayerKnockedOut)
			{
				TransitionPursuitState();
			}
		}
		if (health <= 0)
		{
			TransitionDeathState();
		}
		GamePad& gamePad = Input::Instance().GetGamePad();
		if (EnemyBlock)
		{
			if (playerStam >= 20.0f)
			{
				BlockTime = 1.0f;
				TransitionBlockingState();
			}
			else
			{
				EnemyBlock = false;
			}
		}
	}
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
}
void BoxerX::TransitionFirstAttackState()
{
	playerStam -= firstAttackCost;
	EnemyHitFailed = false;
	state = State::FirstAttack;
	model->PlayAnimation(LeadJab, false);
}

void BoxerX::UpdateFirstAttackState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime >= 0.18f && animationTime <= 0.3f)
	{
		CollisionNodeVsPlayer("mixamorig:LeftHand", leftHandRadius);
	}

	if (!model->IsPlayAnimation())
	{
		if (PlayerKnockedOut)
		{
			TransitionIdleState();
		}
		else
		{
			targetPosition = Boxer::Instance().GetPosition();

			float vx = targetPosition.x - position.x;
			float vy = targetPosition.y - position.y;
			float vz = targetPosition.z - position.z;
			float dist = sqrtf(vx * vx + vy * vy + vz * vz);
			if (dist > attackRange)
			{
				TransitionPursuitState();
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
	}
	if (health <= 0)
	{
		TransitionDeathState();
	}
}

void BoxerX::TransitionSecondAttackState()
{
	playerStam -= secondAttackCost;
	EnemyHitFailed = false;
	state = State::SecondAttack;
	model->PlayAnimation(ElbowPunch, false);
}

void BoxerX::UpdateSecondAttackState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime >= 0.40f && animationTime <= 0.52f)
	{
		CollisionNodeVsPlayer("mixamorig:RightForeArm", leftHandRadius);
	}

	if (!model->IsPlayAnimation())
	{
		if (PlayerKnockedOut)
		{
			TransitionIdleState();
		}
		else
		{
			targetPosition = Boxer::Instance().GetPosition();

			float vx = targetPosition.x - position.x;
			float vy = targetPosition.y - position.y;
			float vz = targetPosition.z - position.z;
			float dist = sqrtf(vx * vx + vy * vy + vz * vz);
			if (dist > attackRange)
			{
				TransitionPursuitState();
			}
			else
			{
				if (playerStam >= secondAttackCost)
				{
					TransitionThirdAttackState();
				}
				else
				{
					TransitionIdleState();
				}
			}
		}
	}
	if (health <= 0)
	{
		TransitionDeathState();
	}
}

void BoxerX::TransitionThirdAttackState()
{
	playerStam -= thirdAttackCost;
	EnemyHitFailed = false;
	state = State::ThirdAttack;
	model->PlayAnimation(Hook, false);
}

void BoxerX::UpdateThirdAttackState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime >= 0.58f && animationTime <= 0.7f)
	{
		CollisionNodeVsPlayer("mixamorig:LeftHand", leftHandRadius);
	}

	if (!model->IsPlayAnimation())
	{
		if (PlayerKnockedOut)
		{
			TransitionIdleState();
		}
		TransitionIdleState();
	}
	if (health <= 0)
	{
		TransitionDeathState();
	}
}

void BoxerX::TransitionHeadAttackState()
{
	playerStam -= headAttackCost;
	EnemyHitFailed = false;
	state = State::HeadAttack;
	model->PlayAnimation(Headbutt, false);
}

void BoxerX::UpdateHeadAttackState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime >= 0.44f && animationTime <= 0.65f)
	{
		CollisionNodeVsPlayer("mixamorig:HeadTop_End", leftHandRadius);
	}

	if (!model->IsPlayAnimation())
	{
		if (PlayerKnockedOut)
		{
			TransitionIdleState();
		}
		TransitionIdleState();
	}
	if (health <= 0)
	{
		TransitionDeathState();
	}
}

void BoxerX::TransitionKneeAttackState()
{
	playerStam -= kneeAttackCost;
	EnemyHitFailed = false;
	state = State::KneeAttack;
	model->PlayAnimation(IllegalKnee, false);
}

void BoxerX::UpdateKneeAttackState(float elapsedTime)
{
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime >= 0.44f && animationTime <= 0.65f)
	{
		CollisionNodeVsPlayer("mixamorig:RightLeg", leftHandRadius);
	}

	if (!model->IsPlayAnimation())
	{
		if (PlayerKnockedOut)
		{
			TransitionIdleState();
		}
		TransitionIdleState();
	}
	if (health <= 0)
	{
		TransitionDeathState();
	}
}

void BoxerX::TransitionDamageState()
{
	//InsertRandomNumber();
	state = State::Damage;
	switch (playerAttackState)
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
	case 4:
		if (!Stunned)
		{
			Stunned = true;
			afkTime = 1.0f;
		}
		AddHitStop(0.3f, 0.0f);
		model->PlayAnimation(HitToHead, false);				//4th Attack
		break;
	case 5:
		AddHitStop(0.3f, 0.2f);
		model->PlayAnimation(BodyHit, false);				//5th Attack
		break;
	}
}

void BoxerX::UpdateDamageState(float elapsedTime)
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
		TransitionDeathState();
	}
}

void BoxerX::TransitionDeathState()
{
	state = State::Death;
	model->PlayAnimation(Knocked, false);
	AddHitStop(0.8f, 0.0f);
}

void BoxerX::UpdateDeathState(float elapsedTime)
{
	EnemyKnockedOut = true;
}

void BoxerX::TransitionBlockingState()
{
	state = State::Blocking;
	model->PlayAnimation(Backward, true);
}

void BoxerX::UpdateBlockingState(float elapsedTime)
{
	BlockTime -= 0.01f;
	if (BlockTime <= 0.0f || playerStam <= 0)
	{
		if (playerStam <= 0.0f)
		{
			playerStam = 0;
		}
		BlockTime = 0.0f;
		EnemyBlock = false;
	}
	if (!EnemyBlock || playerStam <= 0)
	{
		TransitionIdleState();
	}
	if (PlayerHitFailed)
	{
		TransitionBlockedState();
	}
	if (health <= 0)
	{
		TransitionDeathState();
	}
}

void BoxerX::TransitionBlockedState()
{
	//--------------------------------------------------------
	//						Stamina Decrease
	//--------------------------------------------------------
	switch (playerAttackState)
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
		BlockTime = 0.0f;				//5th Attack
		shieldBroke = 1.0f;
		break;
	}

	state = State::Blocked;
	model->PlayAnimation(CenterBlock, false);
}

void BoxerX::UpdateBlockedState(float elapsedTime)
{
	//BlockTime -= 0.01f;
	if (BlockTime <= 0.0f || playerStam <= 0)
	{
		if (playerStam <= 0.0f)
		{
			playerStam = 0;
		}
		BlockTime = 0.0f;
		TransitionIdleState();
	}
	if (!model->IsPlayAnimation())
	{
		TransitionBlockingState();
	}
	if (health <= 0)
	{
		TransitionDeathState();
	}
}

void BoxerX::TransitionPursuitState()
{
	InsertRandomNumber();
	state = State::Pursuit;
	model->PlayAnimation(Walk, true);
}

void BoxerX::UpdatePursuitState(float elapsedTime)
{
	playerStam += moveStaminaIncrease;
	shieldBroke -= elapsedTime;
	if (shieldBroke <= 0.0f)
	{
		shieldBroke = 0.0f;
	}
	targetPosition = Boxer::Instance().GetPosition();
	MoveToTarget(elapsedTime, 1.0f);

	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	if (dist < attackRange)
	{
		//======================================================================
		//								Random Movement											// 0-100 CHANCE			
		//======================================================================
		switch (playerAttackState)
		{
		case 0:
			if (insertedNumber > 0 && insertedNumber <= Chance60)
			{
				if (playerStam >= firstAttackCost)
				{
					TransitionFirstAttackState();
					afkTime = Mathf::RandomRange((afterAttackLowest - (enemyDifficulty * 0.3f)), (afterAttackHighest - (enemyDifficulty * 0.3f)));
				}
				else
				{
					afkTime = Mathf::RandomRange(lowest, highest);
				}
			}
			else
			{
				if (playerStam >= headAttackCost)
				{
					TransitionHeadAttackState();
					afkTime = Mathf::RandomRange((afterAttackLowest - (enemyDifficulty * 0.3f)), (afterAttackHighest - (enemyDifficulty * 0.3f)));
				}
				else
				{
					afkTime = Mathf::RandomRange(lowest, highest);
				}
			}
			break;
		case 1:
			if (insertedNumber > 0 && insertedNumber <= 100)
			{
				if (enemyRandomNumber > Chance50)
				{
					if (playerStam > StamNeededToBlock)
					{
						if (BlockTime == 0.0f)
						{
							BlockTime = counterBlock;
						}
						EnemyBlock = true;
					}
					else
					{
						afkTime = Mathf::RandomRange(lowest, highest);
					}
				}
				else if (enemyRandomNumber <= Chance50 && enemyRandomNumber > 0)
				{
					if (playerStam >= headAttackCost)
					{
						TransitionHeadAttackState();
						afkTime = Mathf::RandomRange((afterAttackLowest - (enemyDifficulty * 0.3f)), (afterAttackHighest - (enemyDifficulty * 0.3f)));
					}
					else
					{
						afkTime = Mathf::RandomRange(lowest, highest);
					}
				}
			}
			break;
		case 2:
			if (insertedNumber > 0 && insertedNumber <= 100)
			{
				if (enemyRandomNumber > Chance50)
				{
					if (playerStam >= firstAttackCost)
					{
						TransitionFirstAttackState();
						afkTime = Mathf::RandomRange((afterAttackLowest - (enemyDifficulty * 0.3f)), (afterAttackHighest - (enemyDifficulty * 0.3f)));
					}
					else
					{
						afkTime = Mathf::RandomRange(lowest, highest);
					}
				}
			}
			break;
		case 3:
			if (insertedNumber > 0 && insertedNumber <= 100)
			{
				if (enemyRandomNumber > Chance50)
				{
					if (playerStam >= headAttackCost)
					{
						TransitionHeadAttackState();
						afkTime = Mathf::RandomRange((afterAttackLowest - (enemyDifficulty * 0.3f)), (afterAttackHighest - (enemyDifficulty * 0.3f)));
					}
					else
					{
						afkTime = Mathf::RandomRange(lowest, highest);
					}
				}
			}
			break;
		case 4:
			if (insertedNumber > 0 && insertedNumber <= 100)
			{
				if (enemyRandomNumber > Chance50)
				{
					if (playerStam >= firstAttackCost)
					{
						TransitionFirstAttackState();
						afkTime = Mathf::RandomRange((afterAttackLowest - (enemyDifficulty * 0.3f)), (afterAttackHighest - (enemyDifficulty * 0.3f)));
					}
					else
					{
						afkTime = Mathf::RandomRange(lowest, highest);
					}
				}
			}
			break;
		case 5:
			if (insertedNumber > 0 && insertedNumber <= 100)
			{
				if (enemyRandomNumber > 0)
				{
					if (playerStam >= firstAttackCost)
					{
						TransitionFirstAttackState();
						afkTime = Mathf::RandomRange((afterAttackLowest - (enemyDifficulty * 0.3f)), (afterAttackHighest - (enemyDifficulty * 0.3f)));
					}
					else
					{
						afkTime = Mathf::RandomRange(lowest, highest);
					}
				}
			}
			break;
		case 6:
			if (playerStam >= kneeAttackCost)
			{
				TransitionKneeAttackState();
				afkTime = Mathf::RandomRange((afterAttackLowest - (enemyDifficulty * 0.3f)), (afterAttackHighest - (enemyDifficulty * 0.3f)));
			}
			else
			{
				afkTime = Mathf::RandomRange(lowest, highest);
				TransitionIdleState();
			}
			break;
		case 7:
			if (insertedNumber > 0 && insertedNumber <= Chance60)
			{
				if (playerStam >= firstAttackCost)
				{
					TransitionFirstAttackState();
					afkTime = Mathf::RandomRange((afterAttackLowest - (enemyDifficulty * 0.3f)), (afterAttackHighest - (enemyDifficulty * 0.3f)));
				}
				else
				{
					afkTime = Mathf::RandomRange(lowest, highest);
				}
			}
			else
			{
				if (playerStam >= headAttackCost)
				{
					TransitionHeadAttackState();
					afkTime = Mathf::RandomRange((afterAttackLowest - (enemyDifficulty * 0.3f)), (afterAttackHighest - (enemyDifficulty * 0.3f)));
				}
				else
				{
					afkTime = Mathf::RandomRange(lowest, highest);
				}
			}
			break;
		}
	}

	if (EnemyBlock)
	{
		TransitionBlockingState();
	}
	if (health <= 0)
	{
		TransitionDeathState();
	}

}


void BoxerX::DrawDebugUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Enemy", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			ImGui::InputFloat3("Position", &position.x);
			ImGui::InputFloat3("Angle", &angle.x);
			//スケール
			ImGui::InputFloat("Health", &health);
			ImGui::InputFloat("shieldBroke", &shieldBroke);
			ImGui::InputFloat("AFK", &afkTime);
			ImGui::InputInt("enemyDifficulty", &enemyDifficulty);
			ImGui::InputFloat("playerMaxStamina", &playerMaxStamina);
			ImGui::InputFloat("PlayerDef", &playerDef);
			ImGui::InputFloat("EnemyDef", &enemyDef);
			ImGui::InputFloat("PlayerFirstAtk", &firstAttackDamage);
			ImGui::InputFloat("PlayerStam", &playerStam);
			ImGui::InputInt("InsertedNumber", &insertedNumber);
			ImGui::InputFloat("BlockTime", &BlockTime);
			ImGui::InputInt("Random", &enemyRandomNumber);
			ImGui::Checkbox("Counter", &CounterAttack);
			ImGui::Checkbox("Done", &CounterAttackDone);
			ImGui::Checkbox("Block", &EnemyBlock);
			ImGui::InputInt("AttackState", &stateEnemyAttackIndicator);
			ImGui::InputInt("PlayerAttackState", &playerAttackState);
			ImGui::Checkbox("PlayerHitFailed", &PlayerHitFailed);
			ImGui::Checkbox("PlayerKnocked", &PlayerKnockedOut);
		}
	}
	ImGui::End();
}

void BoxerX::InsertRandomNumber()
{
	insertedNumber = randomNumber;
}