#pragma region include
#include <chrono>
#include <list>
#include <random>
#include <Windows.h>
#pragma endregion

#pragma region Macro
#define NOW std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count()
#pragma endregion

#pragma region const variable
const int screenWidth = 120;
const int screenHeight = 30;
#pragma endregion

/// <summary>
/// direction for the movement
/// </summary>
enum class EDirection
{
	UP		= 0,
	RIGHT	= 1,
	DOWN	= 2,
	LEFT	= 3
};

/// <summary>
/// vector 2
/// </summary>
struct Vec2
{
public:
	/// <summary>
	/// X
	/// </summary>
	int X = 0;

	/// <summary>
	/// Y
	/// </summary>
	int Y = 0;
};

/// <summary>
/// clear the given screen with spaces
/// </summary>
/// <param name="pScreen">screen reference</param>
void ResetScreen(wchar_t* pScreen)
{
	if (pScreen == nullptr)
		return;

	for (int i = 0; i < screenWidth * screenHeight; i++)
		pScreen[i] = L' ';
}

/// <summary>
/// clear the world of all blocks
/// </summary>
/// <param name="world"></param>
void ResetWorld(std::list<Vec2>& world)
{
	while (world.size())
		world.pop_back();

	// add border of world left, right and bottom
	for (int y = 3; y < screenHeight; y++)
		for (int x = 0; x < screenWidth; x++)
			if (x == 0 || x == screenWidth - 1 || y == screenHeight - 1)
				world.push_back({ x, y });
}

/// <summary>
/// reset the snake to the start position
/// </summary>
/// <param name="snake">snake reference</param>
/// <param name="direction">direction reference</param>
void ResetSnake(std::list<Vec2>& snake, EDirection& direction)
{
	// clear snake
	while (snake.size())
		snake.pop_back();

	// set snake to starting position and size
	snake.push_back({ 40, 15 });
	snake.push_back({ 41, 15 });
	snake.push_back({ 42, 15 });
	snake.push_back({ 43, 15 });
	snake.push_back({ 44, 15 });

	// set start direction of snake movement
	direction = EDirection::LEFT;
}

/// <summary>
/// replace the cookie to a new position
/// </summary>
/// <param name="cookie">cookie reference</param>
/// <param name="snake">snake</param>
/// <param name="world">world</param>
void ReplaceCookie(Vec2& cookie, std::list<Vec2> snake, std::list<Vec2> world)
{
	bool isCookiePlaced = false;
	std::list<Vec2>::iterator it;

	// try until cookie can be placed
	while (!isCookiePlaced)
	{
		// give cookie random position
		cookie.X = std::rand() * NOW % screenWidth;
		cookie.Y = std::rand() * NOW % screenHeight;

		isCookiePlaced = true;

		// if cookie is placed in the header try again
		if (cookie.Y <= 2)
			isCookiePlaced = false;

		// check cookie position with snake position
		for (it = snake.begin(); it != snake.end(); ++it)
		{
			// if cookie is placed in snake try again
			if (it->X == cookie.X && it->Y == cookie.Y)
			{
				isCookiePlaced = false;
				break;
			}
		}

		// check cookie position with world blocks
		for (it = world.begin(); it != world.end(); ++it)
		{
			// if cookie is placed in a world block try again
			if (it->X == cookie.X && it->Y == cookie.Y)
			{
				isCookiePlaced = false;
				break;
			}
		}
	}
}

/// <summary>
/// main
/// </summary>
/// <returns>error code</returns>
int main()
{
	// create screen buffer with width and height and create world blocks
	wchar_t* pScreen = new wchar_t[screenWidth * screenHeight];
	std::list<Vec2> world;

	ResetScreen(pScreen);
	ResetWorld(world);

	// create console screen buffer and set to active screen
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// time variables
	long long thisFrame = NOW;
	long long lastFrame = NOW;
	float time = 0.0f;
	
	// gameplay variables
	bool isGameStarted = false;
	bool isDead = false;
	bool isIncreased = false;
	int score = 0;
	float speed = 0.5f;
	float startingSpeed = 0.5f;

	// player snake variables
	std::list<Vec2> snake;
	EDirection direction = EDirection::LEFT;
	ResetSnake(snake, direction);

	// iterator to not creating it every frame
	std::list<Vec2>::iterator it;

	// cookie
	Vec2 cookie = { 20, 3 };

	// key input variables
	bool isKeyUpDownThisFrame = false;
	bool isKeyUpDownLastFrame = false;
	bool isKeyRightDownThisFrame = false;
	bool isKeyRightDownLastFrame = false;
	bool isKeyDownDownThisFrame = false;
	bool isKeyDownDownLastFrame = false;
	bool isKeyLeftDownThisFrame = false;
	bool isKeyLeftDownLastFrame = false;

	// game loop
	while (true)
	{
		// if key escape pressed stop game loop
		if (0x8000 & GetAsyncKeyState(VK_ESCAPE))
			break;

		// time increases only if game started and not dead
		if (isGameStarted && !isDead)
			time += static_cast<float>(lastFrame - thisFrame) / 1000.0f;
		
		// save current time at beginning of frame
		thisFrame = NOW;

		// if space is pressed start game
		if (!isGameStarted && (0x8000 & GetAsyncKeyState(VK_SPACE)))
			isGameStarted = true;

		// if player is dead and pressed space
		if (isDead && (0x8000 & GetAsyncKeyState(VK_SPACE)))
		{
			// reset everything and restart game
			ResetWorld(world);
			ResetSnake(snake, direction);
			ReplaceCookie(cookie, snake, world);
			isDead = false;
			score = 0;
			speed = startingSpeed;
			isGameStarted = false;
		}

		// update input for arrows for current frame
		isKeyUpDownThisFrame = (0x8000 & GetAsyncKeyState(VK_UP)) != 0;
		isKeyRightDownThisFrame = (0x8000 & GetAsyncKeyState(VK_RIGHT)) != 0;
		isKeyDownDownThisFrame = (0x8000 & GetAsyncKeyState(VK_DOWN)) != 0;
		isKeyLeftDownThisFrame = (0x8000 & GetAsyncKeyState(VK_LEFT)) != 0;

		// change direction if a key was pressed this frame and last frame not
		if (isKeyUpDownThisFrame && !isKeyUpDownLastFrame)
			direction = EDirection::UP;
		if (isKeyRightDownThisFrame && !isKeyRightDownLastFrame)
			direction = EDirection::RIGHT;
		if (isKeyDownDownThisFrame && !isKeyDownDownLastFrame)
			direction = EDirection::DOWN;
		if (isKeyLeftDownThisFrame && !isKeyLeftDownLastFrame)
			direction = EDirection::LEFT;

		// if time passed is higher than speed (time needed for one block movement)
		// because of aspect ratio up and down movement is 50% slower
		if (((direction == EDirection::UP || direction == EDirection::DOWN) && time >= speed * 1.5f) ||
			((direction == EDirection::RIGHT || direction == EDirection::LEFT) && time >= speed))
		{
			// depending on direction add a position to the front of the snake
			switch (direction)
			{
			case EDirection::UP:
				snake.push_front({ snake.begin()->X, snake.begin()->Y - 1 });
				break;
			case EDirection::RIGHT:
				snake.push_front({ snake.begin()->X + 1, snake.begin()->Y });
				break;
			case EDirection::DOWN:
				snake.push_front({ snake.begin()->X, snake.begin()->Y + 1 });
				break;
			case EDirection::LEFT:
				snake.push_front({ snake.begin()->X - 1, snake.begin()->Y });
				break;
			default:
				break;
			}

			// if the snake is increasing do not remove last position
			if (isIncreased)
				isIncreased = false;
			else
				snake.pop_back();

			// reduce time by speed to have acurate movement
			time -= speed;

			// if the player moves into the header than dead
			if (snake.begin()->Y <= 2)
				isDead = true;

			// check snake head with snake body
			for (it = snake.begin(); it != snake.end(); ++it)
			{
				// do not check head with head
				if (it == snake.begin())
					continue;

				// if snake head collides with a body position than dead
				if (it->X == snake.begin()->X && it->Y == snake.begin()->Y)
				{
					isDead = true;
					break;
				}
			}

			// check snake head with world blocks
			for (it = world.begin(); it != world.end(); ++it)
			{
				// if snake head collides with a world block than dead
				if (it->X == snake.begin()->X && it->Y == snake.begin()->Y)
				{
					isDead = true;
					break;
				}
			}

			// if snake head collides with cookie
			if (snake.begin()->X == cookie.X && snake.begin()->Y == cookie.Y)
			{
				// increase snake body and score
				isIncreased = true;
				score++;

				// reduce time needed to move on block (faster) by 1% of current speed
				speed -= speed * 0.01f;

				// add world blcok at current cookie position and replace cookie to new position
				world.push_back({ cookie.X, cookie.Y });
				ReplaceCookie(cookie, snake, world);
			}
		}

		ResetScreen(pScreen);

		// add the header to the screen
		wsprintf(&pScreen[0], L"#########################################################################################################################");
		wsprintf(&pScreen[screenWidth + 5], L"SNAKE!!! CONTROL WITH ARROWS              TIME FOR ONE FIELD MOVE: %dms                 SCORE: %d", static_cast<int>(speed * 1000.0f), score);
		wsprintf(&pScreen[screenWidth * 2 - 1], L"#########################################################################################################################");

		// if the game is not started show start message
		if(!isGameStarted)
			wsprintf(&pScreen[screenWidth * 14 + 40], L"----------START GAME WITH SPACE----------");

		// add world blocks to screen
		for (it = world.begin(); it != world.end(); ++it)
			pScreen[it->Y * screenWidth + it->X] = L'#';

		// add cookie to screen
		pScreen[cookie.Y * screenWidth + cookie.X] = L'%';

		// check snake body
		for (it = snake.begin(); it != snake.end(); ++it)
		{
			// if snake head do nothing
			if (it == snake.begin())
				continue;

			// add snake body to screen depending on dead or not
			if (isDead)
				pScreen[it->Y * screenWidth + it->X] = L'+';
			else
				pScreen[it->Y * screenWidth + it->X] = L'O';
		}

		// if dead add dead snake head and end message to screen
		if (isDead)
		{
			pScreen[snake.begin()->Y * screenWidth + snake.begin()->X] = L'X';
			wsprintf(&pScreen[screenWidth * 14 + 38], L"----------RESTART GAME WITH SPACE----------");
		}
		// if not dead add snake head to screen
		else
		{
			pScreen[snake.begin()->Y * screenWidth + snake.begin()->X] = L'@';
		}

		// write screen to window
		WriteConsoleOutputCharacter(hConsole, pScreen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);

		// save key input as last frame input
		isKeyUpDownLastFrame = isKeyUpDownThisFrame;
		isKeyRightDownLastFrame = isKeyRightDownThisFrame;
		isKeyDownDownLastFrame = isKeyDownDownThisFrame;
		isKeyLeftDownLastFrame = isKeyLeftDownThisFrame;

		// save current time at end of frame
		lastFrame = NOW;
	}

	// clear memory
	delete pScreen;

	// return 0 for correct shutdown
	return 0;
}