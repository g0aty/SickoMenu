#include "pch-il2cpp.h"
#include "dino.hpp"
#include "imgui/imgui.h"
#include "state.hpp"
#include "gui-helpers.hpp"
#include <vector>
#include <random>
#include <string>
#include <format>
#include <algorithm>

namespace Dino
{
	enum class GameState {
		Ready,
		Playing,
		GameOver
	};

	enum class ObstacleType {
		SmallCactus,
		LargeCactus,
		DoubleCactus,
		Pterodactyl
	};

	struct Obstacle {
		float x;
		float y; // Offset from ground (0 for cacti, >0 for flying)
		float width;
		float height;
		ObstacleType type;
	};

	static GameState state = GameState::Ready;
	static float dinoY = 0.0f; // 0 = on ground, >0 = airborne
	static float dinoVelY = 0.0f;
	static bool isDucking = false;
	static float animTimer = 0.0f;
	static int animFrame = 0;

	static float gameSpeed = 220.0f; // px / sec
	static float score = 0.0f;
	static int highScore = 0;
	static float spawnTimer = 0.0f;
	static float nextSpawnInterval = 1.8f;

	static std::vector<Obstacle> obstacles;
	static std::vector<float> groundDots; // Decorative moving ground dots

	const float GRAVITY = 1300.0f;
	const float JUMP_FORCE = 440.0f;
	const float BASE_SPEED = 220.0f;

	const float DINO_WIDTH = 22.0f;
	const float DINO_HEIGHT = 26.0f;
	const float DINO_DUCK_HEIGHT = 15.0f;
	const float DINO_X = 40.0f;

	static void ResetGame()
	{
		state = GameState::Ready;
		dinoY = 0.0f;
		dinoVelY = 0.0f;
		isDucking = false;
		gameSpeed = BASE_SPEED;
		score = 0.0f;
		spawnTimer = 0.0f;
		nextSpawnInterval = 1.2f;
		obstacles.clear();

		groundDots.clear();
		for (int i = 0; i < 15; ++i) {
			groundDots.push_back(static_cast<float>(i * 35 + (rand() % 15)));
		}
	}

	static void SpawnObstacle(float canvasWidth)
	{
		int r = rand() % 100;
		Obstacle obs;
		obs.x = canvasWidth + 20.0f;

		if (score > 300 && r < 25) {
			obs.type = ObstacleType::Pterodactyl;
			obs.width = 24.0f;
			obs.height = 16.0f;
			obs.y = (rand() % 2 == 0) ? 22.0f : 45.0f; // High or low flight
		}
		else if (r < 50) {
			obs.type = ObstacleType::SmallCactus;
			obs.width = 14.0f;
			obs.height = 24.0f;
			obs.y = 0.0f;
		}
		else if (r < 80) {
			obs.type = ObstacleType::LargeCactus;
			obs.width = 18.0f;
			obs.height = 32.0f;
			obs.y = 0.0f;
		}
		else {
			obs.type = ObstacleType::DoubleCactus;
			obs.width = 28.0f;
			obs.height = 25.0f;
			obs.y = 0.0f;
		}

		obstacles.push_back(obs);
	}

	static void DrawDino(ImDrawList* drawList, ImVec2 pos, float yOffset, bool ducking, bool dead, ImU32 color)
	{
		float h = ducking ? DINO_DUCK_HEIGHT : DINO_HEIGHT;
		float w = ducking ? 30.0f : DINO_WIDTH;
		ImVec2 basePos = ImVec2(pos.x + DINO_X, pos.y - yOffset);

		// Dino Body Box
		ImVec2 dinoTopLeft = ImVec2(basePos.x, basePos.y - h);
		ImVec2 dinoBottomRight = ImVec2(basePos.x + w, basePos.y);

		drawList->AddRectFilled(dinoTopLeft, dinoBottomRight, color, 2.0f);

		// Head & Snout
		if (!ducking) {
			ImVec2 headTL = ImVec2(basePos.x + 8.0f, basePos.y - DINO_HEIGHT - 6.0f);
			ImVec2 headBR = ImVec2(basePos.x + DINO_WIDTH + 6.0f, basePos.y - DINO_HEIGHT + 6.0f);
			drawList->AddRectFilled(headTL, headBR, color, 1.0f);

			// Eye (inverted color)
			ImU32 eyeColor = dead ? IM_COL32(230, 40, 40, 255) : IM_COL32(40, 40, 40, 255);
			drawList->AddRectFilled(ImVec2(basePos.x + 16.0f, basePos.y - DINO_HEIGHT - 3.0f),
				ImVec2(basePos.x + 19.0f, basePos.y - DINO_HEIGHT), eyeColor);
		}
		else {
			// Ducking Snout
			ImVec2 headTL = ImVec2(basePos.x + 15.0f, basePos.y - DINO_DUCK_HEIGHT - 4.0f);
			ImVec2 headBR = ImVec2(basePos.x + 34.0f, basePos.y - DINO_DUCK_HEIGHT + 4.0f);
			drawList->AddRectFilled(headTL, headBR, color, 1.0f);

			ImU32 eyeColor = dead ? IM_COL32(230, 40, 40, 255) : IM_COL32(40, 40, 40, 255);
			drawList->AddRectFilled(ImVec2(basePos.x + 26.0f, basePos.y - DINO_DUCK_HEIGHT - 2.0f),
				ImVec2(basePos.x + 29.0f, basePos.y - DINO_DUCK_HEIGHT + 1.0f), eyeColor);
		}

		// Legs (Animated when running on ground)
		if (yOffset <= 0.1f && !dead && state == GameState::Playing) {
			if (animFrame == 0) {
				drawList->AddLine(ImVec2(basePos.x + 4.0f, basePos.y), ImVec2(basePos.x + 4.0f, basePos.y + 6.0f), color, 2.0f);
				drawList->AddLine(ImVec2(basePos.x + 14.0f, basePos.y), ImVec2(basePos.x + 18.0f, basePos.y + 4.0f), color, 2.0f);
			}
			else {
				drawList->AddLine(ImVec2(basePos.x + 4.0f, basePos.y), ImVec2(basePos.x + 8.0f, basePos.y + 4.0f), color, 2.0f);
				drawList->AddLine(ImVec2(basePos.x + 14.0f, basePos.y), ImVec2(basePos.x + 14.0f, basePos.y + 6.0f), color, 2.0f);
			}
		}
		else {
			// Static legs
			drawList->AddLine(ImVec2(basePos.x + 5.0f, basePos.y), ImVec2(basePos.x + 5.0f, basePos.y + 5.0f), color, 2.0f);
			drawList->AddLine(ImVec2(basePos.x + 15.0f, basePos.y), ImVec2(basePos.x + 15.0f, basePos.y + 5.0f), color, 2.0f);
		}
	}

	static void DrawObstacle(ImDrawList* drawList, ImVec2 groundOrigin, const Obstacle& obs, ImU32 color)
	{
		ImVec2 obTL = ImVec2(groundOrigin.x + obs.x, groundOrigin.y - obs.y - obs.height);
		ImVec2 obBR = ImVec2(groundOrigin.x + obs.x + obs.width, groundOrigin.y - obs.y);

		if (obs.type == ObstacleType::Pterodactyl) {
			// Pterodactyl wing animation / shape
			drawList->AddTriangleFilled(
				ImVec2(obTL.x, obBR.y - 8.0f),
				ImVec2(obBR.x, obBR.y - 4.0f),
				ImVec2(obTL.x + 12.0f, obTL.y),
				color
			);
			drawList->AddRectFilled(ImVec2(obTL.x + 4.0f, obBR.y - 10.0f), ImVec2(obBR.x - 2.0f, obBR.y - 2.0f), color, 1.0f);
		}
		else {
			// Cactus Shape
			drawList->AddRectFilled(obTL, obBR, color, 2.0f);

			// Cactus side arms
			if (obs.width >= 16.0f) {
				ImVec2 arm1TL = ImVec2(obTL.x - 4.0f, obTL.y + 6.0f);
				ImVec2 arm1BR = ImVec2(obTL.x, obTL.y + 14.0f);
				drawList->AddRectFilled(arm1TL, arm1BR, color, 1.0f);

				ImVec2 arm2TL = ImVec2(obBR.x, obTL.y + 4.0f);
				ImVec2 arm2BR = ImVec2(obBR.x + 4.0f, obTL.y + 12.0f);
				drawList->AddRectFilled(arm2TL, arm2BR, color, 1.0f);
			}
		}
	}

	void Render()
	{
		if (!State.ShowDino) return;

		ImGui::SetNextWindowSize(ImVec2(440 * State.dpiScale, 240 * State.dpiScale), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowBgAlpha(0.92f);

		if (!ImGui::Begin("Chrome Dino", &State.ShowDino, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::End();
			return;
		}

		float dt = ImGui::GetIO().DeltaTime;
		if (dt > 0.1f) dt = 0.1f; // Cap delta time for stability

		ImVec2 canvasPos = ImGui::GetCursorScreenPos();
		ImVec2 canvasSize = ImGui::GetContentRegionAvail();

		if (canvasSize.x < 100.0f || canvasSize.y < 100.0f) {
			ImGui::End();
			return;
		}

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Background canvas rect
		ImU32 bgCol = IM_COL32(24, 26, 32, 255);
		ImU32 textCol = IM_COL32(230, 230, 235, 255);
		ImU32 dinoCol = IM_COL32(80, 210, 120, 255); // SickoMenu Green accent
		ImU32 groundCol = IM_COL32(100, 110, 130, 255);
		ImU32 obstacleCol = IM_COL32(230, 90, 90, 255);

		// Draw Canvas Box
		drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), bgCol, 4.0f);
		drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), groundCol, 4.0f, 0, 1.5f);

		float groundY = canvasPos.y + canvasSize.y - 35.0f;
		ImVec2 groundOrigin = ImVec2(canvasPos.x, groundY);

		// Keyboard & Canvas Interaction
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
			// Jump physics
			if (jumpPressed && dinoY <= 0.05f) {
				dinoVelY = JUMP_FORCE;
			}

			// Apply gravity
			dinoY += dinoVelY * dt;
			dinoVelY -= GRAVITY * dt;

			if (dinoY <= 0.0f) {
				dinoY = 0.0f;
				dinoVelY = 0.0f;
			}

			// Update speed & score
			score += dt * (gameSpeed * 0.08f);
			if (score > highScore) highScore = static_cast<int>(score);
			gameSpeed = BASE_SPEED + (std::min)(400.0f, score * 0.35f);

			// Animation timer
			animTimer += dt;
			if (animTimer >= 0.1f) {
				animTimer = 0.0f;
				animFrame = 1 - animFrame;
			}

			// Move ground dots
			for (auto& dotX : groundDots) {
				dotX -= gameSpeed * dt;
				if (dotX < 0.0f) dotX += canvasSize.x + (rand() % 30);
			}

			// Spawn obstacles
			spawnTimer += dt;
			if (spawnTimer >= nextSpawnInterval) {
				spawnTimer = 0.0f;
				nextSpawnInterval = (1.1f + (static_cast<float>(rand() % 100) / 100.0f)) * (BASE_SPEED / gameSpeed);
				SpawnObstacle(canvasSize.x);
			}

			// Move & check collision for obstacles
			float dinoHeight = isDucking ? DINO_DUCK_HEIGHT : DINO_HEIGHT;
			float dinoWidth = isDucking ? 30.0f : DINO_WIDTH;

			float dinoLeft = DINO_X + 2.0f;
			float dinoRight = DINO_X + dinoWidth - 2.0f;
			float dinoBottom = dinoY;
			float dinoTop = dinoY + dinoHeight;

			for (size_t i = 0; i < obstacles.size(); ) {
				obstacles[i].x -= gameSpeed * dt;

				float obsLeft = obstacles[i].x + 2.0f;
				float obsRight = obstacles[i].x + obstacles[i].width - 2.0f;
				float obsBottom = obstacles[i].y;
				float obsTop = obstacles[i].y + obstacles[i].height;

				// AABB Collision check
				if (dinoRight > obsLeft && dinoLeft < obsRight && dinoTop > obsBottom && dinoBottom < obsTop) {
					state = GameState::GameOver;
				}

				if (obstacles[i].x + obstacles[i].width < -20.0f) {
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

		// --- DRAWING ---

		// Ground line
		drawList->AddLine(ImVec2(canvasPos.x + 5.0f, groundY), ImVec2(canvasPos.x + canvasSize.x - 5.0f, groundY), groundCol, 2.0f);

		// Ground dots
		for (const auto& dotX : groundDots) {
			if (dotX >= 5.0f && dotX <= canvasSize.x - 5.0f) {
				drawList->AddRectFilled(ImVec2(canvasPos.x + dotX, groundY + 4.0f), ImVec2(canvasPos.x + dotX + 4.0f, groundY + 6.0f), groundCol);
			}
		}

		// Obstacles
		for (const auto& obs : obstacles) {
			DrawObstacle(drawList, groundOrigin, obs, obstacleCol);
		}

		// Dino
		DrawDino(drawList, groundOrigin, dinoY, isDucking, state == GameState::GameOver, dinoCol);

		// Score & HighScore Header
		std::string scoreText = std::format("HI {:05d}  {:05d}", highScore, static_cast<int>(score));
		drawList->AddText(ImVec2(canvasPos.x + canvasSize.x - 145.0f, canvasPos.y + 10.0f), textCol, scoreText.c_str());

		// Overlay Messages
		if (state == GameState::Ready) {
			std::string msg = "PRESS SPACE / CLICK TO JUMP";
			ImVec2 txtSz = ImGui::CalcTextSize(msg.c_str());
			drawList->AddText(ImVec2(canvasPos.x + (canvasSize.x - txtSz.x) * 0.5f, canvasPos.y + canvasSize.y * 0.35f), textCol, msg.c_str());
		}
		else if (state == GameState::GameOver) {
			std::string msg1 = "G A M E   O V E R";
			std::string msg2 = "Press Space or Click to Restart";
			ImVec2 txtSz1 = ImGui::CalcTextSize(msg1.c_str());
			ImVec2 txtSz2 = ImGui::CalcTextSize(msg2.c_str());

			drawList->AddText(ImVec2(canvasPos.x + (canvasSize.x - txtSz1.x) * 0.5f, canvasPos.y + canvasSize.y * 0.32f), IM_COL32(240, 80, 80, 255), msg1.c_str());
			drawList->AddText(ImVec2(canvasPos.x + (canvasSize.x - txtSz2.x) * 0.5f, canvasPos.y + canvasSize.y * 0.48f), textCol, msg2.c_str());
		}

		ImGui::End();
	}
}
