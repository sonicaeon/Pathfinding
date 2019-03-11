#pragma once

#include "Game.h"
#include "DrawableGameComponent.h"
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <wrl.h>

namespace Library
{
	class KeyboardComponent;
	class ImGuiComponent;
}

namespace Pathfinding
{
	class PathfindingGame : public Library::Game
	{
	public:
		PathfindingGame(std::function<void*()> getWindowCallback, std::function<void(SIZE&)> getRenderTargetSizeCallback);

		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

		void Reset();

	private:
		void Exit();

		static DirectX::XMVECTORF32 BackgroundColor;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureGrass;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureWall;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexturePath;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureStart;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureEnd;
		std::shared_ptr<Library::KeyboardComponent> mKeyboard;
		std::shared_ptr<Library::ImGuiComponent> mImGui;

		bool mShowWindow;
		int mAlgorithm;
		int mHeuristic;
	};
}