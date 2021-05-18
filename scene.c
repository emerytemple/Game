#include <stdio.h>
#include <stdlib.h>

#include "scene.h"
#include "boot.h"

struct sceneStack *newSceneStack(int capacity)
{
	struct sceneStack *s = (struct sceneStack*)malloc(sizeof(struct sceneStack));

	s->size = capacity;
	s->top = -1;
	s->scenes = (int *)malloc(sizeof(int) * capacity);

	return s;
}

int size(struct sceneStack *s)
{
	return s->top + 1;
}

bool isEmpty(struct sceneStack *s)
{
	return s->top == -1;
}

void pushScene(struct sceneStack *s, int scene)
{
	s->scenes[++s->top] = scene;
}

int getCurrentScene(struct sceneStack *s) // peekScene
{
	if(!isEmpty(s)) {
		return s->scenes[s->top];
	}
}

int popScene(struct sceneStack *s)
{
	if(isEmpty(s)) {
		printf("can't pop, empty stack");
	}

	return s->scenes[s->top--];
}

void deleteSceneStack(struct sceneStack *s)
{
	free(s->scenes);
	free(s);
}

/*
void printScenes(struct Scene *scenes, int numScenes)
{
	for(int i = 0; i < numScenes; ++i) {
		printf("%s\n", scenes[i].name);
	}
}
*/

void newSceneManager(struct SceneManager *sm)
{
	// NOTE (emery): must be the same as SceneNames enum in scene.h
	struct Scene scenes[] = {
		{
			.name = "boot",
			.start = boot_start,
			.update = boot_update,
			.render = boot_render,
			.end = boot_end,
		},
		{ .name = "title" },
		{ .name = "game" },
	};

	sm->numScenes = SCENE_LENGTH;
	sm->scenes = scenes;
	
	sm->currentScene = -1;
	sm->sceneStack = newSceneStack(SCENE_LENGTH);
}

void startScene(struct SceneManager *sm, enum SceneNames scene)
{
	sm->currentScene = scene;
	pushScene(sm->sceneStack, scene);
}

