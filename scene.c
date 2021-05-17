#include "scene.h"

#include <stdio.h>

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

int getCurrentScene(struct sceneStack *s)
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


void printScenes(struct Scene *scenes, int numScenes)
{
	for(int i = 0; i < numScenes; ++i) {
		printf("%s\n", scenes[i].name);
	}
}

/*
void initSceneManager(Scene *scenes, int numScenes)
{

}
*/
