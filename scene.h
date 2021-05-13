
#ifndef _SCENE_H_
#define _SCENE_H_


struct SceneManager {
	const int numScenes;
	const struct Scene *scenes;

	struct Scene *currentScene; // should this be an int, to use with an enum?
	char *nextSceneName;
};

// void initSceneManager(Scene *scenes, int numScenes);

struct Scene {
	const char *name;

	void (*start)(struct App *app, struct sceneManager *sm);
	void (*update)(struct App *app, struct sceneManager *sm);
	void (*render)(struct App *app, struct sceneManager *sm);
	void (*end)(struct App *app, struct sceneManager *sm);
};

struct Level {
};

void testprint();

// init
// input
// update
// render
// close

#endif




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






















