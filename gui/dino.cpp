#include "pch-il2cpp.h"
#include "dino.hpp"
#include "dino_sprite.h"
#include "imgui/imgui.h"
#include "state.hpp"
#include "gui-helpers.hpp"
#include "game.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <d3d11.h>
#include <vector>
#include <random>
#include <string>
#include <format>
#include <algorithm>

extern ID3D11Device* pDevice;

namespace Dino
{
	enum class GameState {
		Ready,
		Playing,
		GameOver
	};

	enum class ObstacleType {
		SmallCactus1,
		SmallCactus2,
		SmallCactus3,
		LargeCactus1,
		LargeCactus2,
		LargeCactus3,
		Pterodactyl
	};

	struct Obstacle {
		float x;
		float y; // Height offset from ground
		float width;
		float height;
		ObstacleType type;
	};

	static ID3D11ShaderResourceView* g_DinoTextureSRV = nullptr;
	static int g_SpriteWidth = 2404;
	static int g_SpriteHeight = 130;

	static GameState state = GameState::Ready;
	static float dinoY = 0.0f; // 0 = on ground
	static float dinoVelY = 0.0f;
	static bool isDucking = false;
	static float animTimer = 0.0f;
	static int animFrame = 0;

	static float gameSpeed = 240.0f; // px / sec
	static float score = 0.0f;
	static int highScore = 0;
	static float spawnTimer = 0.0f;
	static float nextSpawnInterval = 1.6f;
	static float groundScrollX = 0.0f;

	static bool wasAutoOpenedInLobby = false;

	static std::vector<Obstacle> obstacles;

	const float GRAVITY = 1200.0f;
	const float JUMP_FORCE = 460.0f;
	const float BASE_SPEED = 240.0f;

	// Scaled dimensions (0.5x scale)
	const float DINO_WIDTH = 44.0f;
	const float DINO_HEIGHT = 47.0f;
	const float DINO_DUCK_HEIGHT = 30.0f;
	const float DINO_DUCK_WIDTH = 59.0f;
	const float DINO_X = 40.0f;

	static void LoadDinoTexture(ID3D11Device* device)
	{
		if (g_DinoTextureSRV != nullptr || device == nullptr) return;

		int w = 0, h = 0, channels = 0;
		unsigned char* img_data = stbi_load_from_memory(DINO_SPRITE_PNG, DINO_SPRITE_PNG_SIZE, &w, &h, &channels, 4);
		if (!img_data) return;

		g_SpriteWidth = w;
		g_SpriteHeight = h;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = w;
		desc.Height = h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA subResource;
		ZeroMemory(&subResource, sizeof(subResource));
		subResource.pSysMem = img_data;
		subResource.SysMemPitch = w * 4;

		ID3D11Texture2D* pTexture = nullptr;
		if (SUCCEEDED(device->CreateTexture2D(&desc, &subResource, &pTexture)))
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;

			device->CreateShaderResourceView(pTexture, &srvDesc, &g_DinoTextureSRV);
			pTexture->Release();
		}

		stbi_image_free(img_data);
	}

	static void DrawSprite(ImDrawList* drawList, ImVec2 pos, ImVec2 size, float x1, float y1, float x2, float y2, ImU32 tint = IM_COL32_WHITE)
	{
		if (g_DinoTextureSRV == nullptr) return;
		ImVec2 uv0(x1 / (float)g_SpriteWidth, y1 / (float)g_SpriteHeight);
		ImVec2 uv1(x2 / (float)g_SpriteWidth, y2 / (float)g_SpriteHeight);
		drawList->AddImage((ImTextureID)g_DinoTextureSRV, pos, ImVec2(pos.x + size.x, pos.y + size.y), uv0, uv1, tint);
	}

	static void ResetGame()
	{
		state = GameState::Ready;
		dinoY = 0.0f;
		dinoVelY = 0.0f;
		isDucking = false;
		gameSpeed = BASE_SPEED;
		score = 0.0f;
		spawnTimer = 0.0f;
		nextSpawnInterval = 1.4f;
		groundScrollX = 0.0f;
		obstacles.clear();
	}

	static void SpawnObstacle(float canvasWidth)
	{
		int r = rand() % 100;
		Obstacle obs;
		obs.x = canvasWidth + 20.0f;

		if (score > 250 && r < 25) {
			obs.type = ObstacleType::Pterodactyl;
			obs.width = 46.0f;
			obs.height = 40.0f;
			obs.y = (rand() % 2 == 0) ? 20.0f : 52.0f;
		}
		else if (r < 60) {
			int sub = rand() % 3;
			obs.type = (sub == 0) ? ObstacleType::SmallCactus1 : (sub == 1 ? ObstacleType::SmallCactus2 : ObstacleType::SmallCactus3);
			obs.width = 17.0f;
			obs.height = 35.0f;
			obs.y = 0.0f;
		}
		else {
			int sub = rand() % 3;
			if (sub == 2) {
				obs.type = ObstacleType::LargeCactus3;
				obs.width = 50.0f;
				obs.height = 49.0f;
			}
			else {
				obs.type = (sub == 0) ? ObstacleType::LargeCactus1 : ObstacleType::LargeCactus2;
				obs.width = 25.0f;
				obs.height = 49.0f;
			}
			obs.y = 0.0f;
		}

		obstacles.push_back(obs);
	}

	static void DrawDinoSprite(ImDrawList* drawList, ImVec2 groundOrigin, float yOffset, bool ducking, bool dead, ImU32 tint)
	{
		if (ducking) {
			float x1 = (animFrame == 0) ? 1866.0f : 1984.0f;
			float x2 = x1 + 118.0f;
			ImVec2 pos(groundOrigin.x + DINO_X, groundOrigin.y - yOffset - DINO_DUCK_HEIGHT);
			DrawSprite(drawList, pos, ImVec2(DINO_DUCK_WIDTH, DINO_DUCK_HEIGHT), x1, 36.0f, x2, 96.0f, tint);
		}
		else if (dead) {
			ImVec2 pos(groundOrigin.x + DINO_X, groundOrigin.y - yOffset - DINO_HEIGHT);
			DrawSprite(drawList, pos, ImVec2(DINO_WIDTH, DINO_HEIGHT), 1690.0f, 2.0f, 1778.0f, 96.0f, tint);
		}
		else if (state == GameState::Ready || yOffset > 0.1f) {
			ImVec2 pos(groundOrigin.x + DINO_X, groundOrigin.y - yOffset - DINO_HEIGHT);
			DrawSprite(drawList, pos, ImVec2(DINO_WIDTH, DINO_HEIGHT), 1338.0f, 2.0f, 1426.0f, 96.0f, tint);
		}
		else {
			float x1 = (animFrame == 0) ? 1514.0f : 1602.0f;
			float x2 = x1 + 88.0f;
			ImVec2 pos(groundOrigin.x + DINO_X, groundOrigin.y - yOffset - DINO_HEIGHT);
			DrawSprite(drawList, pos, ImVec2(DINO_WIDTH, DINO_HEIGHT), x1, 2.0f, x2, 96.0f, tint);
		}
	}

	static void DrawObstacleSprite(ImDrawList* drawList, ImVec2 groundOrigin, const Obstacle& obs, ImU32 tint)
	{
		ImVec2 pos(groundOrigin.x + obs.x, groundOrigin.y - obs.y - obs.height);

		switch (obs.type) {
		case ObstacleType::SmallCactus1:
			DrawSprite(drawList, pos, ImVec2(obs.width, obs.height), 446.0f, 2.0f, 480.0f, 72.0f, tint);
			break;
		case ObstacleType::SmallCactus2:
			DrawSprite(drawList, pos, ImVec2(obs.width, obs.height), 480.0f, 2.0f, 514.0f, 72.0f, tint);
			break;
		case ObstacleType::SmallCactus3:
			DrawSprite(drawList, pos, ImVec2(obs.width, obs.height), 514.0f, 2.0f, 548.0f, 72.0f, tint);
			break;
		case ObstacleType::LargeCactus1:
			DrawSprite(drawList, pos, ImVec2(obs.width, obs.height), 652.0f, 2.0f, 702.0f, 100.0f, tint);
			break;
		case ObstacleType::LargeCactus2:
			DrawSprite(drawList, pos, ImVec2(obs.width, obs.height), 702.0f, 2.0f, 752.0f, 100.0f, tint);
			break;
		case ObstacleType::LargeCactus3:
			DrawSprite(drawList, pos, ImVec2(obs.width, obs.height), 752.0f, 2.0f, 852.0f, 100.0f, tint);
			break;
		case ObstacleType::Pterodactyl:
		{
			float x1 = (animFrame == 0) ? 260.0f : 352.0f;
			float x2 = x1 + 92.0f;
			DrawSprite(drawList, pos, ImVec2(obs.width, obs.height), x1, 2.0f, x2, 82.0f, tint);
			break;
		}
		}
	}

	static void DrawScoreDigits(ImDrawList* drawList, ImVec2 topRightPos, int val, int numDigits, ImU32 tint)
	{
		std::string str = std::format("{:0{}d}", val, numDigits);
		float digitWidth = 10.0f;
		float digitHeight = 10.5f;

		for (size_t i = 0; i < str.size(); ++i) {
			int digit = str[i] - '0';
			if (digit < 0 || digit > 9) digit = 0;

			float x1 = 954.0f + digit * 20.0f;
			float x2 = x1 + 20.0f;
			ImVec2 dPos(topRightPos.x + i * (digitWidth + 1.0f), topRightPos.y);

			DrawSprite(drawList, dPos, ImVec2(digitWidth, digitHeight), x1, 2.0f, x2, 23.0f, tint);
		}
	}

	void Render()
	{
		// Auto-show in lobby / Auto-hide in game logic
		if (State.ShowDinoInLobby) {
			if (IsInLobby()) {
				if (!State.ShowDino && !wasAutoOpenedInLobby) {
					State.ShowDino = true;
					wasAutoOpenedInLobby = true;
				}
			}
			else if (IsInGame()) {
				if (State.ShowDino && wasAutoOpenedInLobby) {
					State.ShowDino = false;
					wasAutoOpenedInLobby = false;
				}
			}
			else {
				if (wasAutoOpenedInLobby) {
					wasAutoOpenedInLobby = false;
				}
			}
		}

		if (!State.ShowDino) return;

		// Ensure texture is loaded
		if (g_DinoTextureSRV == nullptr && pDevice != nullptr) {
			LoadDinoTexture(pDevice);
		}

		ImGui::SetNextWindowSize(ImVec2(460 * State.dpiScale, 240 * State.dpiScale), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowBgAlpha(0.95f);

		if (!ImGui::Begin("Chrome Dino", &State.ShowDino, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::End();
			return;
		}

		float dt = ImGui::GetIO().DeltaTime;
		if (dt > 0.1f) dt = 0.1f;

		ImVec2 canvasPos = ImGui::GetCursorScreenPos();
		ImVec2 canvasSize = ImGui::GetContentRegionAvail();

		if (canvasSize.x < 120.0f || canvasSize.y < 120.0f) {
			ImGui::End();
			return;
		}

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Colors
		ImU32 bgCol = IM_COL32(32, 34, 42, 255);
		ImU32 borderCol = IM_COL32(80, 90, 110, 255);
		ImU32 spriteTint = IM_COL32(235, 238, 245, 255);

		// Background box
		drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), bgCol, 6.0f);
		drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), borderCol, 6.0f, 0, 1.5f);

		float groundY = canvasPos.y + canvasSize.y - 30.0f;
		ImVec2 groundOrigin = ImVec2(canvasPos.x, groundY);

		// Controls
		bool windowFocused = ImGui::IsWindowFocused();
		bool jumpPressed = windowFocused && (ImGui::IsKeyPressed(VK_SPACE) || ImGui::IsKeyPressed(VK_UP) || ImGui::IsKeyPressed('W'));
		bool clickPressed = ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		isDucking = windowFocused && (ImGui::IsKeyDown(VK_DOWN) || ImGui::IsKeyDown('S'));

		if (state == GameState::Ready) {
			if (jumpPressed || clickPressed) {
				state = GameState::Playing;
				dinoVelY = JUMP_FORCE;
			}
		}
		else if (state == GameState::Playing) {
			if (jumpPressed && dinoY <= 0.05f) {
				dinoVelY = JUMP_FORCE;
			}

			dinoY += dinoVelY * dt;
			dinoVelY -= GRAVITY * dt;

			if (dinoY <= 0.0f) {
				dinoY = 0.0f;
				dinoVelY = 0.0f;
			}

			score += dt * (gameSpeed * 0.08f);
			if (score > highScore) highScore = static_cast<int>(score);
			gameSpeed = BASE_SPEED + (std::min)(450.0f, score * 0.4f);

			// Ground track scroll
			groundScrollX += gameSpeed * dt;
			if (groundScrollX >= 1200.0f) groundScrollX -= 1200.0f;

			animTimer += dt;
			if (animTimer >= 0.09f) {
				animTimer = 0.0f;
				animFrame = 1 - animFrame;
			}

			spawnTimer += dt;
			if (spawnTimer >= nextSpawnInterval) {
				spawnTimer = 0.0f;
				nextSpawnInterval = (1.1f + (static_cast<float>(rand() % 100) / 100.0f)) * (BASE_SPEED / gameSpeed);
				SpawnObstacle(canvasSize.x);
			}

			// Accurate tight hitboxes
			float dinoLeft = DINO_X + 10.0f;
			float dinoRight = DINO_X + (isDucking ? DINO_DUCK_WIDTH - 8.0f : DINO_WIDTH - 10.0f);
			float dinoBottom = dinoY + 4.0f;
			float dinoTop = dinoY + (isDucking ? DINO_DUCK_HEIGHT - 4.0f : DINO_HEIGHT - 6.0f);

			for (size_t i = 0; i < obstacles.size(); ) {
				obstacles[i].x -= gameSpeed * dt;

				float obsLeft = obstacles[i].x + 5.0f;
				float obsRight = obstacles[i].x + obstacles[i].width - 5.0f;
				float obsBottom = obstacles[i].y + (obstacles[i].type == ObstacleType::Pterodactyl ? 6.0f : 0.0f);
				float obsTop = obstacles[i].y + obstacles[i].height - (obstacles[i].type == ObstacleType::Pterodactyl ? 6.0f : 8.0f);

				// Collision AABB
				if (dinoRight > obsLeft && dinoLeft < obsRight && dinoTop > obsBottom && dinoBottom < obsTop) {
					state = GameState::GameOver;
				}

				if (obstacles[i].x + obstacles[i].width < -30.0f) {
					obstacles.erase(obstacles.begin() + i);
				}
				else {
					++i;
				}
			}
		}
		else if (state == GameState::GameOver) {
			if (jumpPressed || clickPressed) {
				ResetGame();
				state = GameState::Playing;
				dinoVelY = JUMP_FORCE;
			}
		}

		// --- RENDERING SPRITES ---

		// Scrolling Ground Track (2400x24 HD sheet -> 1200x12 scaled)
		float trackWidth = 1200.0f;
		float trackHeight = 12.0f;
		float startX = -fmodf(groundScrollX, trackWidth);

		for (float x = startX; x < canvasSize.x; x += trackWidth) {
			DrawSprite(drawList, ImVec2(canvasPos.x + x, groundY - 2.0f), ImVec2(trackWidth, trackHeight), 2.0f, 104.0f, 2402.0f, 128.0f, spriteTint);
		}

		// Obstacles
		for (const auto& obs : obstacles) {
			DrawObstacleSprite(drawList, groundOrigin, obs, spriteTint);
		}

		// Dino
		DrawDinoSprite(drawList, groundOrigin, dinoY, isDucking, state == GameState::GameOver, spriteTint);

		// Header Score Display (HI 00000  00000)
		ImVec2 scorePos(canvasPos.x + canvasSize.x - 170.0f, canvasPos.y + 12.0f);
		if (highScore > 0) {
			// Draw HI symbol (20x10.5)
			DrawSprite(drawList, scorePos, ImVec2(20.0f, 10.5f), 1154.0f, 2.0f, 1194.0f, 23.0f, spriteTint);
			DrawScoreDigits(drawList, ImVec2(scorePos.x + 24.0f, scorePos.y), highScore, 5, spriteTint);
		}
		DrawScoreDigits(drawList, ImVec2(scorePos.x + 95.0f, scorePos.y), static_cast<int>(score), 5, spriteTint);

		// Overlay Messages
		if (state == GameState::Ready) {
			std::string msg = "PRESS SPACE OR CLICK TO START";
			ImVec2 txtSz = ImGui::CalcTextSize(msg.c_str());
			drawList->AddText(ImVec2(canvasPos.x + (canvasSize.x - txtSz.x) * 0.5f, canvasPos.y + canvasSize.y * 0.38f), IM_COL32(200, 210, 225, 255), msg.c_str());
		}
		else if (state == GameState::GameOver) {
			// Game Over Sprite (384x21 -> 256x14 for crisp proportional aspect ratio)
			float goWidth = (std::min)(256.0f, canvasSize.x - 40.0f);
			float goHeight = goWidth * (21.0f / 384.0f);
			ImVec2 goPos(canvasPos.x + (canvasSize.x - goWidth) * 0.5f, canvasPos.y + canvasSize.y * 0.28f);
			DrawSprite(drawList, goPos, ImVec2(goWidth, goHeight), 1294.0f, 29.0f, 1678.0f, 50.0f, spriteTint);

			// Restart Button Sprite (72x64 -> 36x32)
			ImVec2 rstPos(canvasPos.x + (canvasSize.x - 36.0f) * 0.5f, canvasPos.y + canvasSize.y * 0.46f);
			DrawSprite(drawList, rstPos, ImVec2(36.0f, 32.0f), 2.0f, 2.0f, 74.0f, 66.0f, spriteTint);
		}

		ImGui::End();
	}
}
