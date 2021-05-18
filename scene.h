
#ifndef SCENE_H
#define SCENE_H

#include <stdbool.h>

enum SceneNames {
	SCENE_BOOT,
	SCENE_TITLE,
	SCENE_GAME,
	SCENE_LENGTH
};

struct sceneStack {
	int size;
	int top;

	int *scenes;
};

struct sceneStack *newSceneStack(int capacity);
int size(struct sceneStack *s);
bool isEmpty(struct sceneStack *s);
void pushScene(struct sceneStack *s, int scene);
int getCurrentScene(struct sceneStack *s);
int popScene(struct sceneStack *s);
void deleteSceneStack(struct sceneStack *s);

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

	int currentScene;
	struct sceneStack *sceneStack; // make enum array/stack?
	// merge sceneStack into SceneManager?
};

void newSceneManager(struct SceneManager *sm);
void startScene(struct SceneManager *sm, enum SceneNames scene);

// void printScenes(struct Scene *scenes, int numScenes);

#endif















// switch scene












/*
// in main:

Scene *scenes = {
	{
		.name = "boot",
		.start = boot_start,
		.end = boot_end,
	},
	{
		.name = "title",
	},
	{
		.name = "game", // for now, to test
	},
};

initSceneManager(scenes, 3);

// in boot:

void boot_start()
{
}

void boot_end()
{
}

*/






















