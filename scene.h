
#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

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
/*
	void *data;

	void (*start)(struct App *app, struct sceneManager *sm);
	void (*update)(struct App *app, struct sceneManager *sm);
	void (*render)(struct App *app, struct sceneManager *sm);
	void (*end)(struct App *app, struct sceneManager *sm);
	// printState();
*/
};

struct SceneManager {
	int numScenes;
	struct Scene *scenes;

	int currentScene;
	struct Stack *sceneStack;
};

void printScenes(struct Scene *scenes, int numScenes);

#endif














// void initSceneManager(struct Scene *scenes, int size);

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






















