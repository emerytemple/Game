#include <stdio.h>
#include <stdlib.h>

#include "scene.h"
#include "boot.h"

void newSceneManager(struct SceneManager *sm, int capacity)
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
	
	sm->maxSize = capacity;
	sm->top = -1;
	sm->currentScene = -1;
	sm->stack = (enum SceneNames *)malloc(sizeof(enum SceneNames) * sm->maxSize);
}

int size(struct SceneManager *sm) {
	return sm->top + 1;
}

bool isEmpty(struct SceneManager *sm)
{
	return sm->top == -1;
}

bool isFull(struct SceneManager *sm)
{
	return sm->top == sm->maxSize - 1;
}

void pushScene(struct SceneManager *sm, enum SceneNames newScene)
{
	if(isFull(sm)) { printf("scene stack overflow"); return; }

	// check if stack is empty
	
	if(!isEmpty(sm)) {
		int scene = sm->stack[sm->top];
		sm->scenes[scene].end();
	}

	sm->stack[++sm->top] = newScene;
	sm->currentScene = newScene;

	sm->scenes[newScene].start();
}

void switchScene(struct SceneManager *sm, enum SceneNames scene)
{
	popScene(sm);
	pushScene(sm, scene);
}

void popScene(struct SceneManager *sm)
{
	if(isEmpty(sm)) { printf("can't pop, empty stack");	}

	sm->currentScene = --sm->top;
}

void deleteSceneManager(struct SceneManager *sm)
{
	free(sm->stack);
	free(sm);
}

void printStack(struct SceneManager *sm)
{
	printf("max size = %d\n", sm->maxSize);
	printf("top = %d\n", sm->top);
	printf("current scene = %d\n", sm->currentScene);
	for(int i = 0; i <= sm->top; ++i) {
		int scene = sm->stack[i];
		printf("%d  %s\n", i,sm->scenes[scene].name);
	}
}

/*
void startScene(struct SceneManager *sm, enum SceneNames scene)
{
	sm->currentScene = scene;
	pushScene(sm->sceneStack, scene);
}
*/
