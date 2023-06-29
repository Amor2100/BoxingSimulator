#include "Graphics/Graphics.h"
#include "Graphics/Model.h"

// コンストラクタ
Model::Model(const char* filename)
{
	// リソース読み込み
	resource = std::make_shared<ModelResource>();
	resource->Load(Graphics::Instance().GetDevice(), filename);

	// ノード
	const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

	nodes.resize(resNodes.size());
	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = resNodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;

		if (dst.parent != nullptr)
		{
			dst.parent->children.emplace_back(&dst);
		}
	}

	// 行列計算
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}

// 変換行列計算
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : nodes)
	{
		// ローカル行列算出
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ワールド行列算出
		DirectX::XMMATRIX ParentTransform;
		if (node.parent != nullptr)
		{
			ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
		}
		else
		{
			ParentTransform = Transform;
		}
		DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

		// 計算結果を格納
		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
	}
}

// デバッグ情報表示
void Model::DrawDebugGUI()
{
	if (ImGui::TreeNode("Materials"))
	{
		// デバッグ用にシェーダー定数文字列のリスト生成
		constexpr const char* ModelShaderNames[] =
		{
			"Default",
			"ShadowmapCaster",
			"Phong",
			"WaterSurface",
		};
		//	シェーダー定数とシェーダー名リストとのサイズが違うとエラーを出す
		static_assert(ARRAYSIZE(ModelShaderNames) != static_cast<int>(ModelShaderId::Max) - 1, "ModelShaderNames Size Error!");
		//	材質変更
		for (const ModelResource::Material& material : resource->GetMaterials())
		{
			ModelResource::Material& mat = const_cast<ModelResource::Material&>(material);
			if (ImGui::TreeNode(mat.name.c_str()))
			{
				//	シェーダー
				ImGui::Combo("Shader", &mat.shaderId, ModelShaderNames, static_cast<int>(ModelShaderId::Max), 5);

				//	テクスチャ名
				ImGui::Text("DiffuseTexture Name");
				ImGui::Text(mat.textureFilename.c_str());

				//	マテリアルカラー
				ImGui::ColorEdit4("Material Color", &mat.color.x);

				//	マテリアルパラメーター
				switch (static_cast<ModelShaderId>(mat.shaderId))
				{
					case ModelShaderId::Phong:
					{
						ImGui::SliderFloat3("ka", &mat.phong.ka.x, 0.0f, 1.0f);
						ImGui::SliderFloat3("kd", &mat.phong.kd.x, 0.0f, 1.0f);
						ImGui::SliderFloat3("ks", &mat.phong.ks.x, 0.0f, 1.0f);
						ImGui::SliderFloat("shiness", &mat.phong.shiness, 0.0001f, 128.0f);
						break;
					}
					case ModelShaderId::WaterSurface:
					{
						ImGui::SliderFloat3("ka", &mat.waterSurface.phong.ka.x, 0.0f, 1.0f);
						ImGui::SliderFloat3("kd", &mat.waterSurface.phong.kd.x, 0.0f, 1.0f);
						ImGui::SliderFloat3("ks", &mat.waterSurface.phong.ks.x, 0.0f, 1.0f);
						ImGui::SliderFloat("shiness", &mat.waterSurface.phong.shiness, 0.0001f, 128.0f);
						ImGui::SliderFloat2("waveScroll1", &mat.waterSurface.waveScroll1.x, -0.1f, 0.1f);
						ImGui::SliderFloat2("waveScroll2", &mat.waterSurface.waveScroll2.y, -0.1f, 0.1f);
						break;
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}

void Model::UpdateAnimation(float elapsedTime)
{
	if (!IsPlayAnimation()) return;


	float blendRate = 1.0f;
	if (animationBlendTime < animationBlendSeconds)
	{
		animationBlendTime += elapsedTime;
		if (animationBlendTime > animationBlendSeconds)
		{
			animationBlendTime = animationBlendSeconds;
		}
		blendRate = animationBlendTime / animationBlendSeconds;
	}



	const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
	const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (currentAnimationSeconds >= keyframe0.seconds && currentAnimationSeconds < keyframe1.seconds)
		{
			float rate = (currentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];



				if (blendRate < 1.0f)
				{

					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&node.scale);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&node.rotate);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&node.translate);

					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);


					DirectX::XMStoreFloat3(&node.scale, S);
					DirectX::XMStoreFloat4(&node.rotate, R);
					DirectX::XMStoreFloat3(&node.translate, T);





				}
				else
				{
					DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
					DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);

					DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
					DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
					DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);


					DirectX::XMStoreFloat3(&node.scale, S);
					DirectX::XMStoreFloat4(&node.rotate, R);
					DirectX::XMStoreFloat3(&node.translate, T);

				}
			}
			break;
		}
	}
	//currentAnimationSeconds += elapsedTime;
	//if (currentAnimationSeconds >= animation.secondsLength)
	//{
	//	currentAnimationSeconds -= animation.secondsLength;
	//}
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
		return;
	}
	currentAnimationSeconds += elapsedTime * 1.3f;

	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoopFlag)
		{
			currentAnimationSeconds -= animation.secondsLength;
		}
		else
		{
			currentAnimationSeconds = animation.secondsLength;
			animationEndFlag = true;
		}
	}




}

void Model::PlayAnimation(int index, bool loop, float blendSeconds)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;
	animationLoopFlag = loop;
	animationEndFlag = false;
	animationBlendTime = 0.0f;
	animationBlendSeconds = blendSeconds;
}

bool Model::IsPlayAnimation() const
{
	if (currentAnimationIndex < 0) return false;
	if (currentAnimationIndex >= resource->GetAnimations().size()) return false;
	return true;
}


Model::Node* Model::FindNode(const char* name)
{
	for (Node& node : nodes)
	{
		if (::strcmp(node.name, name) == 0)
		{
			return &node;
		}
	}

	return nullptr;
}