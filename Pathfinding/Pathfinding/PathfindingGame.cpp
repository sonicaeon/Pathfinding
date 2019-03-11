#include "pch.h"
#include "PathfindingGame.h"
#include <ImGui\imgui_impl_dx11.h>
#include <GridHelper.h>
#include "BreadthFirstSearch.h"
#include "GreedyBestFirst.h"
#include "Dijkstra.h"
#include "AStar.h"
#include "StopWatch.h"

using namespace std;
using namespace DirectX;
using namespace Library;
using namespace Microsoft::WRL;

IMGUI_API LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Pathfinding
{
	bool startFlag = true;
	bool endFlag = false;
	int32_t gridHeight, gridWidth;
	XMVECTORF32 PathfindingGame::BackgroundColor = Colors::Black;

	HWND windowHandle;
	WNDCLASSEX window;
	static const wstring windowTitle = L"Failure!";
	static const wstring windowContent = L"No Path was Found!\nPerhaps try a different Graph?";

	// load the hardcoded grid located in directory: "...\Pathfinding\Pathfinding\Content"
	// TODO: add UI menu/file selection/search
	string filename = "Content\\Grid.grid";
	Graph graph = GridHelper::LoadGridFromFile(filename, gridWidth, gridHeight);
	deque<shared_ptr<Node>> path;
	set<shared_ptr<Node>> visitedSet;
	size_t nodesVisited = 0;

	// initialize to some default node upon starting since empty node will crash on update after initializing
	shared_ptr<Node> startNode = graph.At(0, 0);
	shared_ptr<Node> endNode = graph.At(9, 9);
	int64_t elapsedTime = 0;

	PathfindingGame::PathfindingGame(function<void*()> getWindowCallback, function<void(SIZE&)> getRenderTargetSizeCallback) :
		Game(getWindowCallback, getRenderTargetSizeCallback), mShowWindow(true), mAlgorithm(0), mHeuristic(0)
	{
	}

	void PathfindingGame::Initialize()
	{
		// Load all of the textures & components
		ComPtr<ID3D11Resource> textureResource;
		ComPtr<ID3D11Texture2D> texture;
		wstring textureName = L"Content\\Textures\\2D-Grass.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTextureGrass.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		textureName = L"Content\\Textures\\Wall-Tree4.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTextureWall.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		textureName = L"Content\\Textures\\Stone-Path.jpg";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTexturePath.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		textureName = L"Content\\Textures\\Start-Flag.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTextureStart.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		textureName = L"Content\\Textures\\End-Flag.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTextureEnd.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		SpriteManager::Initialize(*this);
		BlendStates::Initialize(mDirect3DDevice.Get());
		
		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mKeyboard.get());

		mImGui = make_shared<ImGuiComponent>(*this);
		mComponents.push_back(mImGui);
		mServices.AddService(ImGuiComponent::TypeIdClass(), mImGui.get());
		auto imGuiWndProcHandler = make_shared<UtilityWin32::WndProcHandler>(ImGui_ImplDX11_WndProcHandler);
		UtilityWin32::AddWndProcHandler(imGuiWndProcHandler);

		// Render space for the Grid and buttons
		auto gridRenderBlock = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			ImGui::Begin("Pathfinding", &mShowWindow, ImGuiWindowFlags_NoTitleBar);
			ImGui::SetWindowSize(ImVec2(750, 750));
			ImGui::SetWindowPos(ImVec2(0, 0));
			int x, y;
			int z = 0;

			for (y = 0; y < gridHeight; y++)
			{
				for (x = 0; x < gridWidth; x++)
				{
					// push unique button ID
					ImGui::PushID(z);
					if (graph.At(x, y)->Type() == NodeType::Normal)
					{

						if (graph.At(x, y) == startNode)
						{
							ImGui::ImageButton(mTextureStart.Get(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), -1);
						}
						else if (graph.At(x, y) == endNode)
						{
							ImGui::ImageButton(mTextureEnd.Get(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), -1);
						}
						else if (find(path.begin(), path.end(), graph.At(x, y)) != path.end())
						{
							ImGui::ImageButton(mTexturePath.Get(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), -1);
						}
						else if (ImGui::ImageButton(mTextureGrass.Get(), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), -1))
						{
							// add start and end node from User input from the GUI
							if (startFlag)
							{
								startNode = graph.At(x, y);
								startFlag = false;
								endFlag = true;
							}
							else if (endFlag)
							{
								endNode = graph.At(x, y);
								startFlag = true;
							}
						}
					}
					else if (graph.At(x, y)->Type() == NodeType::Wall)
					{
						// wall texture
						ImGui::ImageButton(mTextureWall.Get(), ImVec2(58, 56), ImVec2(0, 0), ImVec2(1, 1), 0);
					}
					z++;
					ImGui::PopID();
					ImGui::SameLine();
					if (x == gridWidth - 1)
						ImGui::NewLine(); // new line for grid
				}
			}
			ImGui::End();
		});
		mImGui->AddRenderBlock(gridRenderBlock);

		// Render space for a Menu, used to change options such as Heuristic and Pathfinding Algorithm Selections
		auto menuRenderBlock = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			ImGui::Begin("Menu", &mShowWindow, ImGuiWindowFlags_NoCollapse);
			ImGui::SetWindowSize(ImVec2(520, 1280));
			ImGui::SetWindowPos(ImVec2(760, 0));

			// constructor for algorithm's and timer
			BreadthFirstSearch BFS;
			GreedyBestFirst GBF;
			Dijkstra dijkstra;
			AStar AS;
			StopWatch timer;

			// display selected start and end nodes locations
			ImGui::Text("Start Node: (%d, %d)", startNode->Location().X, startNode->Location().Y);
			ImGui::SameLine();
			ImGui::Text("End Node: (%d, %d)", endNode->Location().X, endNode->Location().Y);

			// display selectable algorithms
			ImGui::TextColored(ImVec4(0.09f, 0.9f, 0.06f, 1.f), "Algorithms");
			ImGui::RadioButton("Breadth First Search", &mAlgorithm, 0); ImGui::SameLine();
			ImGui::RadioButton("Greedy Best-First", &mAlgorithm, 1); ImGui::SameLine();
			ImGui::RadioButton("Dijkstra's", &mAlgorithm, 2); ImGui::SameLine();
			ImGui::RadioButton("A*", &mAlgorithm, 3);

			// display selectable heuristics
			ImGui::TextColored(ImVec4(0.07f, 0.95f, 0.9f, 1.f), "Heuristics");
			ImGui::RadioButton("Manhattan Distance", &mHeuristic, 0); ImGui::SameLine();
			ImGui::RadioButton("Euclidean Distance", &mHeuristic, 1);

			// display path statistics
			ImGui::Text("Time Taken: %dms", elapsedTime);
			ImGui::Text("# of Nodes Visited: %d", nodesVisited);
			ImGui::Text("Total Path Cost: %d", path.size());

			// find path
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.09f, 0.9f, 0.06f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.09f, 0.9f, 0.06f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.f, 1.f, 1.f));
			if(ImGui::Button("Find Path"))
			{
				switch (mAlgorithm)
				{
				case 0:
					timer.Restart();
					path = BFS.FindPath(startNode, endNode, visitedSet);
					timer.Stop();
					elapsedTime = timer.ElapsedMilliseconds().count();
					break;
				case 1:
					timer.Restart();
					path = GBF.FindPath(startNode, endNode, visitedSet, mHeuristic);
					timer.Stop();
					elapsedTime = timer.ElapsedMilliseconds().count();
					break;
				case 2:
					timer.Restart();
					path = dijkstra.FindPath(startNode, endNode, visitedSet);
					timer.Stop();
					elapsedTime = timer.ElapsedMilliseconds().count();
					break;
				case 3:
					timer.Restart();
					path = AS.FindPath(startNode, endNode, visitedSet, mHeuristic);
					timer.Stop();
					elapsedTime = timer.ElapsedMilliseconds().count();
					break;
				default:
					break;
				}
				nodesVisited = visitedSet.size();
				visitedSet.clear();
				if (path.empty())
				{
					HWND mainWindow = ::FindWindow(NULL, L"Pathfinding");
					if (MessageBox(windowHandle, windowContent.c_str(), windowTitle.c_str(), MB_OK) == IDOK)
					{
						if (mainWindow)
						{
							// move to foreground
							::SetForegroundWindow(mainWindow);
						}
					}
				}
				
				
					
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Compute the path using the current settings.");

			ImGui::PopStyleColor(3);

			// Reset button
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.95f, 0.07f, 0.07f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.95f, 0.07f, 0.07f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.f, 1.f, 1.f));
			if (ImGui::Button("Reset"))
			{
				PathfindingGame::Reset();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset the current set values for Start, End Nodes to default and clear any Nodes in the Path or Visited Set.");

			ImGui::PopStyleColor(3);
			ImGui::End();
		});
		mImGui->AddRenderBlock(menuRenderBlock);

		Game::Initialize();
	}

	void PathfindingGame::Shutdown()
	{
		BlendStates::Shutdown();
		SpriteManager::Shutdown();

		Game::Shutdown();
	}

	void PathfindingGame::Update(const GameTime &gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape))
		{
			Exit();
		}

		Game::Update(gameTime);
	}

	void PathfindingGame::Draw(const GameTime &gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Game::Draw(gameTime);

		HRESULT hr = mSwapChain->Present(1, 0);

		// If the device was removed either by a disconnection or a driver upgrade, we must recreate all device resources.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			HandleDeviceLost();
		}
		else
		{
			ThrowIfFailed(hr, "IDXGISwapChain::Present() failed.");
		}
	}

	void PathfindingGame::Reset()
	{
		startFlag = true;
		endFlag = false;
		startNode = graph.At(0, 0);
		endNode = graph.At(9, 9);
		path.clear();
		elapsedTime = 0;
		visitedSet.clear();
		nodesVisited = 0;
	}

	void PathfindingGame::Exit()
	{
		PostQuitMessage(0);
	}
}