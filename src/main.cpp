#include <vector>
#include <birb2d/Logger.hpp>
#include <birb2d/Renderwindow.hpp>
#include <birb2d/Timestep.hpp>
#include <birb2d/Values.hpp>
#include <birb2d/Entity.hpp>
#include <birb2d/Physics.hpp>
#include <birb2d/Utils.hpp>

int main(int argc, char **argv)
{
	Birb::Window window("Birb Strike: Global Defensive", Birb::Vector2int(1280, 720), 75, false);
	Birb::TimeStep timeStep;
	timeStep.Init(&window);

	/* Gameloop variables */
	bool GameRunning = true;

	Birb::Entity currentLevel("Current level");

	SDL_Texture* levelBackgroundTexture = Birb::Resources::LoadTexture("./res/sprites/map1.png");
	TTF_Font* mainFont = Birb::Resources::LoadFont("./res/fonts/mononoki/mononoki-Bold.ttf", 20);
	std::vector<Birb::Entity*> textEntities;

	/* Interpolation text */
	Birb::Entity interpolationText("Interpolation", Birb::Vector2int(10, 10), Birb::EntityComponent::TextComponent("Interpolation: 0", mainFont, &Birb::Colors::Black));
	textEntities.push_back(&interpolationText);

	/* Frametime text */
	Birb::Entity frameTimeText("Frametime", Birb::Vector2int(10, 30), Birb::EntityComponent::TextComponent("Frametime: 0", mainFont, &Birb::Colors::Black));
	textEntities.push_back(&frameTimeText);

	/* Refreshrate text */
	Birb::Entity refreshrateText("Refreshrate", Birb::Vector2int(10, 50), Birb::EntityComponent::TextComponent("Refreshrate: 0", mainFont, &Birb::Colors::Black));
	textEntities.push_back(&refreshrateText);

	/* Timer text */
	Birb::Entity timerText("Frametime", Birb::Vector2int(10, 90), Birb::EntityComponent::TextComponent("Timer: 0", mainFont, &Birb::Colors::Black));
	textEntities.push_back(&timerText);

	/* Run counter text */
	Birb::Entity runCounterText("Runcount", Birb::Vector2int(10, 110), Birb::EntityComponent::TextComponent("Runs: 0", mainFont, &Birb::Colors::Black));
	textEntities.push_back(&runCounterText);

	/* Pixel data reading test */
	SDL_Surface* levelPathDots = SDL_LoadBMP("./res/sprites/map1_path.bmp");
	int w=0, h=0, p=0;
	Uint8* pixels = Birb::Resources::CopyTexturePixels(levelPathDots, &w, &h, &p);
	std::vector<Birb::Vector2int> pathPixelPositions;
	Birb::Vector2int startPoint;
	
	SDL_Color color = Birb::utils::TexturePixelToColor(pixels, Birb::Vector2int(108, 4), 128);
	//std::cout << "Color r: " << (int)color.r << std::endl;
	//std::cout << "Color g: " << (int)color.g << std::endl;
	//std::cout << "Color b: " << (int)color.b << std::endl;
	//std::cout << "Color a: " << (int)color.a << std::endl;

	if (pixels)
	{
		printf("width=%d, height=%d, pitch=%d\n", w, h, p);
		SDL_Color tmpColor;

		for (int i = 0; i < 128; i++)
		{
			for (int j = 0; j < 72; j++)
			{
				// Some weird BGRA format
				tmpColor = Birb::utils::TexturePixelToColor(pixels, Birb::Vector2int(i, j), 128);

				if (tmpColor.r != 0)
				{
					//std::cout << "[" << i << ", " << j << "] R: " << (int)tmpColor.r << ", G: " << (int)tmpColor.g << ", B: " << (int)tmpColor.b << std::endl;
					pathPixelPositions.push_back(Birb::Vector2int(i * 10, j * 10 + 1));
				}
				else if (tmpColor.g == 255)
				{
					//std::cout << "[" << i << ", " << j << "] R: " << (int)tmpColor.r << ", G: " << (int)tmpColor.g << ", B: " << (int)tmpColor.b << std::endl;
					startPoint = Birb::Vector2int(i * 10, j * 10);
				}
			}
		}
	}

	//Birb::Vector2int closestPoint = Birb::Math::FindClosestPoint(startPoint, pathPixelPositions);
	std::vector<Birb::Vector2int> roadPath = Birb::utils::SortPath(startPoint, pathPixelPositions);;
	std::cout << "Road length: " << roadPath.size() << std::endl;

	Birb::Vector2int dotPosition = startPoint;
	int nextPosition = 1;
	float interpolation = 0;

	Birb::Entity firstLevelBackground("Level 1 background", Birb::Rect(0, 0, window.window_dimensions.x, window.window_dimensions.y), levelBackgroundTexture);

	currentLevel = firstLevelBackground;

	Birb::Timer pathTimer;
	pathTimer.Start();
	double lastTime = 0;
	double delta = 0;
	std::vector<float> deltaList;
	double deltaTotal;
	double avgDelta;

	while (GameRunning)
	{
		timeStep.Start();
		while (timeStep.Running())
		{
			/* Handle input stuff */
			while (window.PollEvents())
			{
				window.EventTick(window.event, &GameRunning);
			}

			timeStep.Step();
		}

		window.Clear();

		/* Update debugging texts */
		interpolationText.SetText("Interpolation: " + std::to_string(interpolation));
		frameTimeText.SetText("Frametime: " + std::to_string(timeStep.deltaTime));
		refreshrateText.SetText("Refreshrate: " + std::to_string(window.refresh_rate));
		runCounterText.SetText("Runs: " + std::to_string(deltaList.size()));
		

		/* Move the green dot */
		if (nextPosition <= roadPath.size() - 1)
		{
			interpolation += timeStep.deltaTime * 2.0f;
			dotPosition = Birb::Math::Lerp(roadPath[nextPosition - 1], roadPath[nextPosition], interpolation);
		}

		if (interpolation >= 1 && nextPosition < roadPath.size())
		{
			if (interpolation < 2.0f )
			{
				interpolation = interpolation - 1.0f;
				nextPosition++;
			}
			else
			{
				interpolation = interpolation - 2.0f;
				nextPosition += 2;
			}
		}
		else if (nextPosition >= roadPath.size())
		{
			std::cout << "Path finished in " << pathTimer.ElapsedSeconds() << " seconds" << std::endl;
			//pathDone = true;
			pathTimer.Stop();
			delta = pathTimer.ElapsedSeconds() - lastTime;

			lastTime = pathTimer.ElapsedSeconds();

			if (delta != lastTime)
				deltaList.push_back(delta);

			/* Randomize a new refreshrate */
			window.refresh_rate = Birb::utils::randomInt(5, 400);
			Birb::Debug::Log("New random target refreshrate: " + std::to_string(window.refresh_rate));

			deltaTotal = 0;
			for (int i = 0; i < deltaList.size(); i++)
			{
				deltaTotal += deltaList[i];
			}
			avgDelta = deltaTotal / deltaList.size();

			nextPosition = 1;
			pathTimer.Start();
		}

		if (lastTime != 0 && delta != lastTime)
			timerText.SetText("Timer: " + std::to_string(pathTimer.ElapsedSeconds()) + " (Last time: " + std::to_string(lastTime) + ", Delta: " + std::to_string(delta) + ", Avg. Delta: " + std::to_string(avgDelta) + ")");
		else if (lastTime != 0)
			timerText.SetText("Timer: " + std::to_string(pathTimer.ElapsedSeconds()) + " (Last time: " + std::to_string(lastTime) + ")");
		else
			timerText.SetText("Timer: " + std::to_string(pathTimer.ElapsedSeconds()));

		/* Do rendering things here */
		Birb::Render::DrawEntity(currentLevel);
		//Birb::Render::DrawCircle(Birb::Colors::Red, startPoint, 4);
		for (int i = 0; i < roadPath.size(); i++)
			Birb::Render::DrawCircle(Birb::Colors::Red, roadPath[i], 4);

		for (int i = 0; i < roadPath.size() - 1; i++)
			Birb::Render::DrawLine(Birb::Colors::Red, roadPath[i], roadPath[i + 1]);

		/* Text components */
		for (int i = 0; i < textEntities.size(); i++)
			Birb::Render::DrawEntity(*textEntities[i]);

		Birb::Render::DrawCircle(Birb::Colors::Green, dotPosition, 6);
		/* ------------------------ */

		window.Display();

		timeStep.End();
	}

	window.Cleanup();
	SDL_Quit();

	return 0;
}
