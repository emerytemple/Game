
#ifndef SCENE_H
#define SCENE_H

#include <stdbool.h>

enum SceneNames {
	SCENE_BOOT,
	SCENE_TITLE,
	SCENE_GAME,
	SCENE_LENGTH // change to SCENE_COUNT?
};

struct Scene {
	char *name;

	// void *data;

	void (*start)();
	void (*update)();
	void (*render)();
	void (*end)();
	// printState();
};

struct SceneManager {
	int numScenes;
	struct Scene *scenes;

	int maxSize;
	int top;
	enum SceneNames currentScene;
	enum SceneNames *stack;
};

void newSceneManager(struct SceneManager *sm, int capacity);
int size(struct SceneManager *sm);
bool isEmpty(struct SceneManager *sm);
bool isFull(struct SceneManager *sm);
void pushScene(struct SceneManager *sm, enum SceneNames scene);
void switchScene(struct SceneManager *sm, enum SceneNames scene);
void popScene(struct SceneManager *sm);
void deleteSceneManager(struct SceneManager *sm);
void printStack(struct SceneManager *sm);

// void startScene(struct SceneManager *sm, enum SceneNames scene);
// void printScenes(struct Scene *scenes, int numScenes);
// switch scene

#endif
