#pragma once
#include <vector>
#include "Stage.h"

class StageManager
{
private:
	StageManager() {};
	~StageManager() {};

public:
	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}
	void Update(float elapsedTime);
	void Register(Stage* stage);
	void Clear();
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

private:
	std::vector<Stage*> stages;
};