// By: Emery Temple

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_gamecontroller.h>

#include "scene.h"

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;

enum AnalogInput {
	CONTROLLER_AXIS_LEFT_UP,
	CONTROLLER_AXIS_LEFT_DOWN,
	CONTROLLER_AXIS_LEFT_LEFT,
	CONTROLLER_AXIS_LEFT_RIGHT,
	CONTROLLER_AXIS_RIGHT_UP,
	CONTROLLER_AXIS_RIGHT_DOWN,
	CONTROLLER_AXIS_RIGHT_LEFT,
	CONTROLLER_AXIS_RIGHT_RIGHT,
	CONTROLLER_AXIS_TRIGGER_LEFT,
	CONTROLLER_AXIS_TRIGGER_RIGHT
};

struct App {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_GameController *controller;

	bool quit;

	u16 controllerState;
	u16 controllerAxisState;

	f32 deadZoneLow;
	f32 deadZoneHigh;
	f32 deadZoneTrigger;
	
	// temp
	f32 lx, ly;
	f32 rx, ry;
	f32 lt, rt;
};

bool initApp(struct App *app);
SDL_Texture *loadTexture(struct App *app, const char *path);
void closeApp(struct App *app);
void handleEvent(struct App *app, const SDL_Event *event);

void processControllerAxisEvent(struct App *app, const SDL_Event *event);
void applyRadialDeadZone(f32 *outx, f32 *outy, f32 x, f32 y, f32 deadZoneLow, f32 deadZoneHigh);

void DrawCircle(SDL_Renderer *renderer, s32 centreX, s32 centreY, s32 radius);

void printControllerButtonName(int button);

void printDeviceAddedEvent(const SDL_Event *event);
void printDeviceRemovedEvent(const SDL_Event *event);
void printDeviceRemappedEvent(const SDL_Event *event);

void printKeyboardEvent(const SDL_Event *event);

void printMouseButtonDownEvent(const SDL_Event *event);
void printMouseButtonUpEvent(const SDL_Event *event);
void printMouseWheelEvent(const SDL_Event *event);

void printWindowEvent(const SDL_Event *event);

////

struct Stack {
	int size;
	int top;
	struct Scene *scenes;
	float *items;
};

struct Stack *newStack(int capacity)
{
	struct Stack *s = (struct Stack*)malloc(sizeof(struct Stack));

	s->size = capacity;
	s->top = -1;
	s->items = (float *)malloc(sizeof(float) * capacity);

	return s;
}

int size(struct Stack *s)
{
	return s->top + 1;
}

bool isEmpty(struct Stack *s)
{
	return s->top == -1;
}

void push(struct Stack *s, float item)
{
	s->items[++s->top] = item;
}

float peek(struct Stack *s)
{
	if(!isEmpty(s)) {
		return s->items[s->top];
	}
}

float pop(struct Stack *s)
{
	if(isEmpty(s)) {
		printf("can't pop, empty stack");
	}

	return s->items[s->top--];
}

////

int main(int argc, char* argv[])
{

	struct Stack *s = newStack(5);
	push(s, 1.1);
	push(s, 2.2);
	push(s, 3.3);

	printf("The top element is %f\n", peek(s));
	printf("The stack size is %d\n", size(s));

	float a = pop(s);
	float b = pop(s);
	float c = pop(s);

	printf("%f  %f  %f\n", a, b, c);

	if(isEmpty(s)) {
		printf("empty\n");
	}
	else {
		printf("not empty\n");
	}

	struct App app = {
		.window = NULL,
		.renderer = NULL,
		.controller = NULL,
		.quit = false,
		.controllerState = 0,
		.controllerAxisState = 0,
		.deadZoneLow = 0.25,
		.deadZoneHigh = 0.75,
		.deadZoneTrigger = 0.1,
		.lx = 0.0,
		.ly = 0.0,
		.rx = 0.0,
		.ry = 0.0,
		.lt = 0.0,
		.rt = 0.0,
	};

	bool success = initApp(&app);
	if(!success) {
		printf("Failed to initialize!\n");
		return 1;
	}

	SDL_Texture *texture = loadTexture(&app, "images/download.png");
	SDL_Rect clip = { 0, 0, 45, 70 };

	SDL_Rect quad = {
		.x = 0,
		.y = 0,
		.w = clip.w,
		.h = clip.h
	};

	SDL_Event event;
	while(!app.quit) {

		while(SDL_PollEvent(&event)) {
			handleEvent(&app, &event);
		}

		// input
		// printf("controller = %d  %d\n", app.controllerAxisState, app.controllerState);

		// update
		u32 up = (app.controllerState >> SDL_CONTROLLER_BUTTON_DPAD_UP) & 1U;
		u32 down = (app.controllerState >> SDL_CONTROLLER_BUTTON_DPAD_DOWN) & 1U;
		u32 left = (app.controllerState >> SDL_CONTROLLER_BUTTON_DPAD_LEFT) & 1U;
		u32 right = (app.controllerState >> SDL_CONTROLLER_BUTTON_DPAD_RIGHT) & 1U;

		s32 x = right - left;
		s32 y = up - down;
		
		s32 dx = (x > 0) - (x < 0);
		s32 dy = (y > 0) - (y < 0);
		
		// printf("%d %d %d %d %d %d %d %d\n", up, down, left, right, x, y, dx, dy);

		quad.x += dx;
		quad.y += dy;

		// render
		SDL_SetRenderDrawColor(app.renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(app.renderer);

/*
		// left input square
		// red square
		SDL_Rect squareOutline = {20, 20, 400, 400};
		SDL_SetRenderDrawColor(app.renderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderDrawRect(app.renderer, &squareOutline);

		// red cross
		SDL_RenderDrawLine(app.renderer, 20, 220, 420, 220);
		SDL_RenderDrawLine(app.renderer, 220, 20, 220, 420);

		// red circle
		DrawCircle(app.renderer, 220, 220, 200);

		// blue circles
		SDL_SetRenderDrawColor(app.renderer, 0x00, 0x00, 0xFF, 0xFF);
		DrawCircle(app.renderer, 220, 220, 50);
		DrawCircle(app.renderer, 220, 220, 150);
		
		// green location
		SDL_SetRenderDrawColor(app.renderer, 0x00, 0xFF, 0x00, 0xFF);
		int xx = app.lx * 200;
		int yy = app.ly * 200;
		SDL_Rect point = { 220 + xx - 2, 220 - yy - 2, 4, 4 };
		SDL_RenderFillRect(app.renderer, &point);

		// white location
		SDL_SetRenderDrawColor(app.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		int nxx = app.nlx * 200;
		int nyy = app.nly * 200;
		SDL_Rect newPoint = { 220 + nxx - 2, 220 - nyy - 2, 4, 4 };
		SDL_RenderFillRect(app.renderer, &newPoint);

		// right input square
		// red square
		SDL_SetRenderDrawColor(app.renderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_Rect squareOutline2 = {450, 20, 400, 400};
		SDL_RenderDrawRect(app.renderer, &squareOutline2);

		// red cross
		SDL_RenderDrawLine(app.renderer, 450, 220, 850, 220);
		SDL_RenderDrawLine(app.renderer, 650, 20, 650, 420);

		// red circle
		DrawCircle(app.renderer, 650, 220, 200);

		// blue lines
		SDL_SetRenderDrawColor(app.renderer, 0x00, 0x00, 0xFF, 0xFF);
		f32 rad = 30.0 * (M_PI/180.0);
		int h = (int)(tan(rad) * 200.0);
		// printf("%f    %d\n", rad, h);
		SDL_RenderDrawLine(app.renderer, 450, 220 - h, 850, 220 + h);
		SDL_RenderDrawLine(app.renderer, 450, 220 + h, 850, 220 - h);
		SDL_RenderDrawLine(app.renderer, 650 - h, 20, 650 + h, 420);
		SDL_RenderDrawLine(app.renderer, 650 + h, 20, 650 - h, 420);

		// green location
		SDL_SetRenderDrawColor(app.renderer, 0x00, 0xFF, 0x00, 0xFF);
		int xx2 = app.nlx * 200;
		int yy2 = app.nly * 200;
		SDL_Rect point2 = { 650 + xx2 - 2, 220 - yy2 - 2, 4, 4 };
		SDL_RenderFillRect(app.renderer, &point2);

		// white location
		SDL_SetRenderDrawColor(app.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		int nxx2 = app.nlx * 200;
		int nyy2 = app.nly * 200;
		SDL_Rect newPoint2 = { 220 + nxx2 - 2, 220 - nyy2 - 2, 4, 4 };
		SDL_RenderFillRect(app.renderer, &newPoint2);
*/
		// SDL_RenderCopy(app.renderer, texture, &clip, &quad);
		SDL_RenderPresent(app.renderer);
	}

	SDL_DestroyTexture(texture);
	texture = NULL;

	closeApp(&app);

	return 0;
}

bool initApp(struct App *app)
{
	int success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	if(success != 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		printf("Warning: Linear texture filtering not enabled!\n");
	}

	app->window = SDL_CreateWindow("SDL Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	if(app->window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
	if(app->renderer == NULL) {
		printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	SDL_SetRenderDrawColor(app->renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & imgFlags)) {
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
		return false;
	}

	int numJoysticks = SDL_NumJoysticks();
	if(numJoysticks < 1) {
		printf("No gamepads connected!\n");
		return false;
	}
	
	// todo: handle multiple controllers
	app->controller = SDL_GameControllerOpen(0);
	if(app->controller == NULL) {
		printf("Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

SDL_Texture *loadTexture(struct App *app, const char *path)
{
	SDL_Surface *surface = IMG_Load(path);
	if(surface == NULL) {
		printf("Unable to load image %s! SDL Error: %s\n", path, IMG_GetError());
	}

	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0x00, 0xFF, 0xFF));

	SDL_Texture *texture = SDL_CreateTextureFromSurface(app->renderer, surface);
	if(texture == NULL) {
		printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
	}
	
	SDL_FreeSurface(surface);
	
	return texture;
}

void closeApp(struct App *app)
{
	SDL_GameControllerClose(app->controller);
	app->controller = NULL;

	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);
	app->window = NULL;
	app->renderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

void handleEvent(struct App *app, const SDL_Event *event)
{
	switch(event->type) {
		// case SDL_AUDIODEVICEADDED:
		// case SDL_AUDIODEVICEREMOVED:
		case SDL_CONTROLLERAXISMOTION:
			processControllerAxisEvent(app, event);
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			app->controllerState |= 1UL << event->cbutton.button; // set bit
			// printf("timestamp: %d, %d ", event->cbutton.timestamp, app->controllerState);
			// printControllerButtonName(event->cbutton.button);
			break;
		case SDL_CONTROLLERBUTTONUP:
			app->controllerState &= ~(1UL << event->cbutton.button); // clear bit
			// printf("timestamp: %d, %d ", event->cbutton.timestamp, app->controllerState);
			// printControllerButtonName(event->cbutton.button);
			break;
		case SDL_CONTROLLERDEVICEADDED:
			printDeviceAddedEvent(event);
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			printDeviceRemovedEvent(event);
			break;
		case SDL_CONTROLLERDEVICEREMAPPED:
			printDeviceRemappedEvent(event);
			break;
		// case SDL_KEYUP:
		case SDL_KEYDOWN:
			printKeyboardEvent(event);
			break;
		case SDL_MOUSEBUTTONDOWN:
			printMouseButtonDownEvent(event);
			break;
		case SDL_MOUSEBUTTONUP:
			printMouseButtonUpEvent(event);
			break;
		case SDL_MOUSEWHEEL:
			printMouseWheelEvent(event);
			break;
		case SDL_QUIT:
			app->quit = true;
			break;
		case SDL_WINDOWEVENT:
			// printWindowEvent(event);
			break;
		default:
			break;
	}
}

void processControllerAxisEvent(struct App *app, const SDL_Event *event)
{
	// printf("timestamp: %d, ", event->caxis.timestamp);

	const f32 STICK_MIN = 32768.0;
	const f32 STICK_MAX = 32767.0;
	f32 limit = (event->caxis.value < 0) ? STICK_MIN: STICK_MAX;

	switch(event->caxis.axis) {
		case SDL_CONTROLLER_AXIS_LEFTX:
			app->lx = (float) event->caxis.value / limit;
			break;
		case SDL_CONTROLLER_AXIS_LEFTY:
			app->ly = (float) -event->caxis.value / limit;
			break;
		case SDL_CONTROLLER_AXIS_RIGHTX:
			app->rx = (float) event->caxis.value / limit;
			break;
		case SDL_CONTROLLER_AXIS_RIGHTY:
			app->ry = (float) -event->caxis.value / limit;
			break;
		case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
			app->lt = (float) event->caxis.value / STICK_MAX;
			break;
		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
			app->rt = (float) event->caxis.value / STICK_MAX;
			break;
		default:
			break;
	}

	f32 nlx, nly;
	f32 nrx, nry;

	applyRadialDeadZone(&nlx, &nly, app->lx, app->ly, app->deadZoneLow, app->deadZoneHigh);
	applyRadialDeadZone(&nrx, &nry, app->rx, app->ry, app->deadZoneLow, app->deadZoneHigh);

	app->controllerAxisState = 0;

	f32 range = 120.0;
	f32 r2 = range / 2.0;

	f32 mag = sqrt(nlx*nlx + nly*nly);
	if(mag != 0.0) {
		f32 angle = atan2(nly, nlx) * 180.0 / M_PI;
		if((90.0 - r2) <= angle && angle <= (90.0 + r2)) {
			app->controllerAxisState |= 1 << CONTROLLER_AXIS_LEFT_UP;
		} 

		if((-90.0 - r2) <= angle && angle <= (-90.0 + r2)) {
			app->controllerAxisState |= 1 << CONTROLLER_AXIS_LEFT_DOWN;
		}

		if((180.0 - r2) <= angle || angle <= (-180.0 + r2)) {
			app->controllerAxisState |= 1 << CONTROLLER_AXIS_LEFT_LEFT;
		}

		if(-r2 <= angle && angle <= r2) {
			app->controllerAxisState |= 1 << CONTROLLER_AXIS_LEFT_RIGHT;
		}
	}

	f32 rmag = sqrt(nrx*nrx + nry*nry);
	if(rmag != 0.0) {
		f32 rangle = atan2(nry, nrx) * 180.0 / M_PI;
		if((90.0 - r2) <= rangle && rangle <= (90.0 + r2)) {
			app->controllerAxisState |= 1 << CONTROLLER_AXIS_RIGHT_UP;
		} 

		if((-90.0 - r2) <= rangle && rangle <= (-90.0 + r2)) {
			app->controllerAxisState |= 1 << CONTROLLER_AXIS_RIGHT_DOWN;
		}

		if((180.0 - r2) <= rangle || rangle <= (-180.0 + r2)) {
			app->controllerAxisState |= 1 << CONTROLLER_AXIS_RIGHT_LEFT;
		}

		if(-r2 <= rangle && rangle <= r2) {
			app->controllerAxisState |= 1 << CONTROLLER_AXIS_RIGHT_RIGHT;
		}
	}

	if(app->deadZoneTrigger <= app->lt) {
		app->controllerAxisState |= 1 << CONTROLLER_AXIS_TRIGGER_LEFT;
	}

	if(app->deadZoneTrigger <= app->rt) {
		app->controllerAxisState |= 1 << CONTROLLER_AXIS_TRIGGER_RIGHT;
	}

	// printf("axis state = %d,  %f  %f\n", app->controllerAxisState, nlx, nly);
}

void applyRadialDeadZone(f32 *outx, f32 *outy, f32 x, f32 y, f32 deadZoneLow, f32 deadZoneHigh)
{
	f32 mag = sqrt(x*x + y*y);

	if(mag > deadZoneLow) {
		f32 range = deadZoneHigh - deadZoneLow;
		f32 normalizedMag = fmin(1.0f, (mag - deadZoneLow) / range);
		f32 scale = normalizedMag/mag;

		*outx = x * scale;
		*outy = y * scale;
	} else {
		*outx = 0.0f;
		*outy = 0.0f;	
	}
}

void DrawCircle(SDL_Renderer *renderer, s32 centreX, s32 centreY, s32 radius)
{
   const s32 diameter = (radius * 2);

   s32 x = (radius - 1);
   s32 y = 0;
   s32 tx = 1;
   s32 ty = 1;
   s32 error = (tx - diameter);

   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

void printControllerButtonName(int button)
{
	printf("gamepad button ");
	switch(button) {
		case SDL_CONTROLLER_BUTTON_A:
			printf("A");
			break;
		case SDL_CONTROLLER_BUTTON_B:
			printf("B");
			break;
		case SDL_CONTROLLER_BUTTON_X:
			printf("X");
			break;
		case SDL_CONTROLLER_BUTTON_Y:
			printf("Y");
			break;
		case SDL_CONTROLLER_BUTTON_BACK:
			printf("back");
			break;
		case SDL_CONTROLLER_BUTTON_GUIDE:
			printf("guide");
			break;
		case SDL_CONTROLLER_BUTTON_START:
			printf("start");
			break;
		case SDL_CONTROLLER_BUTTON_LEFTSTICK:
			printf("left stick");
			break;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
			printf("right stick");
			break;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
			printf("left shoulder");
			break;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			printf("right shoulder");
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
			printf("dpad up");
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			printf("dpad down");
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			printf("dpad left");
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			printf("dpad right");
			break;
		default:
			break;
	}
	
	printf("\n");
}

void printDeviceAddedEvent(const SDL_Event *event)
{
	printf("timestamp: %d, ", event->cdevice.timestamp);
	printf("controller %d added\n", event->cdevice.which);
}

void printDeviceRemovedEvent(const SDL_Event *event)
{
	printf("timestamp: %d, ", event->cdevice.timestamp);
	printf("controller %d removed\n", event->cdevice.which);
}

void printDeviceRemappedEvent(const SDL_Event *event)
{
	printf("timestamp: %d, ", event->cdevice.timestamp);
	printf("controller %d remapped\n", event->cdevice.which);
}

void printKeyboardEvent(const SDL_Event *event)
{
	switch(event->key.keysym.sym) {
		case SDLK_UP:
			puts("up");
			break;
		
		case SDLK_DOWN:
			puts("down");
			break;
		
		case SDLK_LEFT:
			puts("left");
			break;
		
		case SDLK_RIGHT:
			puts("right");
			break;
		
		default:
			break;
	}
}

void printMouseButtonDownEvent(const SDL_Event *event)
{
	printf("timestamp: %d, ", event->button.timestamp);
	printf("mouse button ");
	switch(event->button.button) {
		case SDL_BUTTON_LEFT:
			printf("left ");
			break;
		case SDL_BUTTON_MIDDLE:
			printf("middle ");
			break;
		case SDL_BUTTON_RIGHT:
			printf("right ");
			break;
		case SDL_BUTTON_X1:
			printf("x1 ");
			break;
		case SDL_BUTTON_X2:
			printf("x2 ");
			break;
		default:
			printf("unrecognized");
			break;
	}
	printf("down at ");
	printf("(%d, %d)\n", event->button.x, event->button.y);
}

void printMouseButtonUpEvent(const SDL_Event *event)
{
	printf("timestamp: %d, ", event->button.timestamp);
	printf("mouse button ");
	switch(event->button.button) {
		case SDL_BUTTON_LEFT:
			printf("left ");
			break;
		case SDL_BUTTON_MIDDLE:
			printf("middle ");
			break;
		case SDL_BUTTON_RIGHT:
			printf("right ");
			break;
		case SDL_BUTTON_X1:
			printf("x1 ");
			break;
		case SDL_BUTTON_X2:
			printf("x2 ");
			break;
		default:
			printf("unrecognized");
			break;
	}
	printf("up at ");
	printf("(%d, %d)\n", event->button.x, event->button.y);
}

void printMouseWheelEvent(const SDL_Event *event)
{
	printf("timestamp: %d, ", event->wheel.timestamp);
	printf("mouse wheel ");
	
	int x = event->wheel.x;
	int y = event->wheel.y;
	if(SDL_MOUSEWHEEL_FLIPPED) {
		x = -x;
		y = -y;
	}
	
	printf("in direction (%d, %d)\n", x, y);
}

void printWindowEvent(const SDL_Event *event)
{
	printf("timestamp: %d\n", event->window.timestamp);
	switch(event->window.event) {
		case SDL_WINDOWEVENT_SHOWN:
			printf("window %d shown\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_HIDDEN:
			printf("window %d hidden\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_EXPOSED:
			printf("window %d exposed\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_MOVED:
			printf("window %d moved to %d, %d\n", event->window.windowID, event->window.data1, event->window.data2);
			break;
		case SDL_WINDOWEVENT_RESIZED:
			printf("window %d resized to %dx%d\n", event->window.windowID, event->window.data1, event->window.data2);
			break;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			printf("window %d size changed to %dx%d\n", event->window.windowID, event->window.data1, event->window.data2);
			break;
		case SDL_WINDOWEVENT_MINIMIZED:
			printf("window %d minimized\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_MAXIMIZED:
			printf("window %d maximized\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_RESTORED:
			printf("window %d restored\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_ENTER:
			printf("mouse entered window %d\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_LEAVE:
			printf("mouse left window %d\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			printf("window %d gained keyboard focus\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			printf("window %d lost keyboard focus\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_CLOSE:
			printf("window %d closed\n", event->window.windowID);
			// event.type = SDL_QUIT;
			// SDL_PushEvent(&event);
			break;
		case SDL_WINDOWEVENT_TAKE_FOCUS:
			printf("window %d is offered a focus\n", event->window.windowID);
			break;
		case SDL_WINDOWEVENT_HIT_TEST:
			printf("window %d has a special hit test\n", event->window.windowID);
			break;
		default:
			printf("window %d received unknown event %d\n", event->window.windowID, event->window.event);
			break;
	}
}





















