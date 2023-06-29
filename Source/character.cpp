#include "character.h"
//#include "stage.h"
#include "Mathf.h"
#include "StageManager.h"

void Character::UpdateTransform()
{
	CagingPosition();

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//DirectX::XMMATRIX W = R * S * T;

	//DirectX::XMStoreFloat4x4(&transform, W);

	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;

	DirectX::XMMATRIX W = R * S * T;
	DirectX::XMStoreFloat4x4(&transform, W);

}

void Character::Jump(float speed)
{
	velocity.y = speed;
}

void Character::Move(float vx, float vz, float speed)
{
	//speed *= elapsedTime;
	//position.x += vx * speed;
	//position.z += vz * speed;

	moveVecX = vx;
	moveVecZ = vz;
	maxMoveSpeed = speed;

}

void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;
	length = sqrtf(length);


	//	進行方向を単位ベクトル化
	vx /= length;
	vz /= length;


	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	float dot = (vx * frontX) + (vz * frontZ);
	float rot = 1.0f - dot;
	if (rot > speed) rot = speed;

	float cross = (frontZ * vx) - (frontX * vz);

	if (cross < 0.0f)
	{
		angle.y -= rot;
	}
	else
	{
		angle.y += rot;
	}
}

bool Character::ApplyDamage(float damage, float invicibleTime)
{
	if (this->invicibleTimer > 0.0f)
	{
		return false;
	}
	if (damage == 0)
	{
		return false;
	}
	if (health <= 0)
	{
		return false;
	}
	health -= damage;
	invicibleTimer = invicibleTime;
	if (health <= 0)
	{
		OnDead();
	}
	else
	{
		OnDamaged();
	}
	return true;
}

void Character::UpdateInvicibleTimer(float elapsedTime)
{
	if (invicibleTimer > 0.0f)
	{
		invicibleTimer -= elapsedTime;
	}
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}



void Character::CagingPosition()
{
	if (position.z >= 3.1f)
	{
		position.z = 3.1f;
	}
	if (position.z <= -3.1f)
	{
		position.z = -3.1f;
	}
	if (position.x >= 3.4f)
	{
		position.x = 3.4f;
	}
	if (position.x <= -3.3f)
	{
		position.x = -3.3f;
	}
}


void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		float friction = this->friction * elapsedFrame;

		float AirControl = this->airControl;
		if (!isGround)
		{
			friction *= airControl;
		}
		if (length > friction)
		{
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			velocity.x -= vx * friction;
			velocity.z -= vz * friction;

		}
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	if (length <= maxMoveSpeed)
	{
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			float acceleration = this->acceleration * elapsedFrame;
			if (!isGround)
			{
				acceleration *= airControl;
			}


			velocity.x += moveVecX * acceleration;
			velocity.z += moveVecZ * acceleration;

			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				float vx = velocity.x / length;
				float vz = velocity.z / length;

				velocity.x = vx * maxMoveSpeed;
				velocity.z = vz * maxMoveSpeed;


				if (isGround && slopeRate > 0.0f)
				{
					velocity.y -= (length * slopeRate) * elapsedFrame;
				}
			}
		}
	}

	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
	//position.x += velocity.x * elapsedTime;
	//position.z += velocity.z * elapsedTime;
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		DirectX::XMFLOAT3 start = { position.x , position.y + stepOffset , position.z };
		DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset , position.z + mz };

		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&hit.position);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

			const float ofs = 0.001f;
			Dot = DirectX::XMVectorAdd(Dot, DirectX::XMLoadFloat(&ofs));

			DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
			DirectX::XMFLOAT3 collectPosition;
			DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

			HitResult hit2;
			if (!StageManager::Instance().RayCast(start, collectPosition, hit2))
			{
				position.x = collectPosition.x;
				position.z = collectPosition.z;
			}
			else
			{
				position.x = hit2.position.x;
				position.z = hit2.position.z;
			}
		}
		else
		{
			position.x += mx;
			position.z += mz;
		}
	}
}

void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	velocity.y += gravity;
}

void Character::UpdateVerticalMove(float elapsedTime)
{
	slopeRate = 0.0f;


	float my = velocity.y * elapsedTime;

	DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	if (my < 0.0f)
	{
		DirectX::XMFLOAT3 start = { position.x , position.y + stepOffset , position.z };
		//start.y += stepOffset;
		DirectX::XMFLOAT3 end = { position.x, position.y + my , position.z };
		//end.y += my;
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			normal = hit.normal;
			position = hit.position;
			angle.y += hit.rotation.y;

			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			position.y += my;
			isGround = false;
		}
	}
	else if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
	}
	{
		float ax = atan2f(normal.z, normal.y);
		float az = -atan2f(normal.x, normal.y);

		const float t = 0.2f;
		angle.x = Mathf::Lerp(angle.x, ax, t);
		angle.z = Mathf::Lerp(angle.z, az, t);

	}
}

void Character::UpdateVelocity(float elapsedTime)
{

	float elapsedFrame = 60.0f * elapsedTime;

	UpdateVerticalVelocity(elapsedFrame);

	UpdateHorizontalVelocity(elapsedFrame);

	UpdateVerticalMove(elapsedTime);

	UpdateHorizontalMove(elapsedTime);
}

void Character::AddHitStop(float time, float speed)
{
	stopTimer = time;
	timeScale = speed;
}

void Character::UpdateHitStop(float& elapsedTime)
{
	if (stopTimer > 0.0f)
	{
		stopTimer -= elapsedTime;

		if (stopTimer < 0.0f)
		{
			stopTimer = 0.0f;
		}

		elapsedTime *= timeScale;
	}
}