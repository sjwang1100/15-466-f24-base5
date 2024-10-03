#include "Mode.hpp"

#include "Scene.hpp"
#include "WalkMesh.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <random>
#include <ctime>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	enum ingredient_list {
		Patty,
		Cheese,
		Veggies
	};

	const float height[3] = { 0.06f, 0.015f, 0.018f };

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	// index of 0xFF means random
	void put_food_randomly(uint16_t ingre_index = 0xFFFF);
	
	void add_walk_mesh(uint8_t n = 3);
	float max_height = 0.0f;
	void put_ingre_to_burger(uint8_t index);

	Scene::Transform* l_bun = nullptr;
	Scene::Transform* u_bun = nullptr;

	struct Ingredient {
		Scene::Transform* ingre = nullptr;
		uint8_t index;
		WalkPoint x;
	};

	std::vector< Ingredient> ingredients;


	//player info:
	struct Player {
		WalkPoint at;
		//transform is at player's feet and will be yawed by mouse left/right motion:
		Scene::Transform *transform = nullptr;
		//camera is at player's head and will be pitched by mouse up/down motion:
		Scene::Camera *camera = nullptr;
	} player;
};
