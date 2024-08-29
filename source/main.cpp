#include <citro2d.h>

// the 3ds has different screen width, but the same screen height.
const int TOP_SCREEN_WIDTH = 400;
const int BOTTOM_SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

u32 whiteColor;
u32 blackColor;
u32 greenColor;
u32 redColor;
u32 blueColor;

const int SPEED = 10;

int ballVelocityX = 5;
int ballVelocityY = 5;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
	float h;
	unsigned int color;
} Rectangle;

Rectangle spriteBounds;
Rectangle ball;
Rectangle bottomBounds;

bool hasCollision(Rectangle bounds, Rectangle ball)
{
	return bounds.x < ball.x + ball.w && bounds.x + bounds.w > ball.x &&
		   bounds.y < ball.y + ball.h && bounds.y + bounds.h > ball.y;
}

void update(u32 keyDown)
{
	// Respond to user input
	u32 keyHeld = hidKeysHeld();

	if (keyDown & KEY_A)
	{
		if (bottomBounds.color == redColor)
			bottomBounds.color = blueColor;
		else
			bottomBounds.color = redColor;
	}

	if (keyHeld & KEY_LEFT && spriteBounds.x > 0)
	{
		spriteBounds.x -= SPEED;
	}

	else if (keyHeld & KEY_RIGHT && spriteBounds.x < TOP_SCREEN_WIDTH - spriteBounds.w)
	{
		spriteBounds.x += SPEED;
	}
	else if (keyHeld & KEY_UP && spriteBounds.y > 0)
	{
		spriteBounds.y -= SPEED;
	}
	else if (keyHeld & KEY_DOWN && spriteBounds.y < SCREEN_HEIGHT - spriteBounds.h)
	{
		spriteBounds.y += SPEED;
	}

	if (ball.x < 0 || ball.x > TOP_SCREEN_WIDTH - ball.w)
	{
		ballVelocityX *= -1;

		ball.color = greenColor;
	}

	else if (ball.y < 0 || ball.y > SCREEN_HEIGHT - ball.h)
	{
		ballVelocityY *= -1;

		ball.color = redColor;
	}

	else if (hasCollision(spriteBounds, ball))
	{
		ballVelocityX *= -1;
		ballVelocityY *= -1;

		ball.color = blueColor;
	}

	ball.x += ballVelocityX;
	ball.y += ballVelocityY;
}

int main(int argc, char *argv[])
{
	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Create top and bottom screens
	C3D_RenderTarget *topScreen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget *bottomScreen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	// Load the sprite
	C2D_SpriteSheet sheet = C2D_SpriteSheetLoad("romfs:/gfx/alien_1.t3x");
	C2D_Image sprite = C2D_SpriteSheetGetImage(sheet, 0);

	// Create colors
	whiteColor = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
	blackColor = C2D_Color32(0x00, 0x00, 0x00, 0x00);
	greenColor = C2D_Color32(0x00, 0xFF, 0x00, 0xFF);
	redColor = C2D_Color32(0xFF, 0x00, 0x00, 0xFF);
	blueColor = C2D_Color32(0x00, 0x00, 0xFF, 0xFF);

	spriteBounds = {TOP_SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 32, 32, whiteColor};
	ball = {TOP_SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 0, 20, 20, whiteColor};
	bottomBounds = {BOTTOM_SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 32, 32, blueColor};

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		u32 keyDown = hidKeysDown();

		if (keyDown & KEY_START)
			break;

		update(keyDown);

		// Render the top screen
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(topScreen, blackColor);
		C2D_SceneBegin(topScreen);

		// (float x, float y, float z, float  w, float h, u32 clr)
		C2D_DrawRectSolid(ball.x, ball.y, ball.z, ball.w, ball.h, ball.color);

		// ( 	C2D_Image img, float x, float y, float depth, const C2D_ImageTint *tint C2D_OPTIONALnullptr, float scaleX C2D_OPTIONAL1.0f, float scaleY C2D_OPTIONAL1.0f)
		C2D_DrawImageAt(sprite, spriteBounds.x, spriteBounds.y, 0, NULL, 1, 1);

		C3D_FrameEnd(0);

		// Render the bottom screen
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(bottomScreen, blackColor);
		C2D_SceneBegin(bottomScreen);

		C2D_DrawRectSolid(bottomBounds.x, bottomBounds.y, bottomBounds.z, bottomBounds.w, bottomBounds.h, bottomBounds.color);

		C3D_FrameEnd(0);
	}

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}
