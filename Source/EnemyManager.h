#pragma once
#include <vector>
#include "Enemy.h"
#include <set>

class EnemyManager
{
private:
	EnemyManager() {};
	~EnemyManager() {};

public:
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}
	void Update(float elapsedTime);
	//void Render(ID3D11DeviceContext* dc, Shader* shader);
	void Register(Enemy* enemy);
	void Clear();
	void DrawDebugPrimitive();
	int GetEnemyCount() const { return static_cast<int> (enemies.size()); }
	Enemy* GetEnemy(int index) { return enemies.at(index); }
	void Remove(Enemy* enemy);
private:
	void CollisionEnemyVsEnemies();
	std::vector<Enemy*>	enemies;
	std::set<Enemy*> removes;
};