#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <random>
#include <ctime>

GLuint phonebank_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > phonebank_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("lower_bun.pnct"));
	phonebank_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

GLuint cheese_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > cheese_meshes(LoadTagDefault, []() -> MeshBuffer const* {
	MeshBuffer const* ret = new MeshBuffer(data_path("cheese.pnct"));
	cheese_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

GLuint patty_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > patty_meshes(LoadTagDefault, []() -> MeshBuffer const* {
	MeshBuffer const* ret = new MeshBuffer(data_path("patty.pnct"));
	patty_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

GLuint u_bun_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > upper_bun_meshes(LoadTagDefault, []() -> MeshBuffer const* {
	MeshBuffer const* ret = new MeshBuffer(data_path("upper_bun.pnct"));
	u_bun_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

GLuint veggies_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > veggies_meshes(LoadTagDefault, []() -> MeshBuffer const* {
	MeshBuffer const* ret = new MeshBuffer(data_path("veggies.pnct"));
	veggies_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

GLuint wm_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > wm_meshes(LoadTagDefault, []() -> MeshBuffer const* {
	MeshBuffer const* ret = new MeshBuffer(data_path("walkmesh.pnct"));
	wm_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > phonebank_scene(LoadTagDefault, []() -> Scene const* {
	return new Scene(data_path("empty.scene"), [&](Scene& scene, Scene::Transform* transform, std::string const& mesh_name) {
		});
});

//Load< Scene > phonebank_scene(LoadTagDefault, []() -> Scene const * {
//	return new Scene(data_path("phone-bank.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
//		Mesh const &mesh = phonebank_meshes->lookup(mesh_name);
//
//		scene.drawables.emplace_back(transform);
//		Scene::Drawable &drawable = scene.drawables.back();
//
//		drawable.pipeline = lit_color_texture_program_pipeline;
//
//		drawable.pipeline.vao = phonebank_meshes_for_lit_color_texture_program;
//		drawable.pipeline.type = mesh.type;
//		drawable.pipeline.start = mesh.start;
//		drawable.pipeline.count = mesh.count;
//
//	});
//});

WalkMesh const *walkmesh = nullptr;
Load< WalkMeshes > phonebank_walkmeshes(LoadTagDefault, []() -> WalkMeshes const * {
	WalkMeshes *ret = new WalkMeshes(data_path("walkmesh.w"));
	walkmesh = &ret->lookup("Plane.001");
	return ret;
});

PlayMode::PlayMode() : scene(*phonebank_scene) {
	Mesh mesh1 = wm_meshes->lookup("Plane.001");

	auto newTrans1 = new Scene::Transform();
	scene.drawables.emplace_back(newTrans1);
	Scene::Drawable& drawable1 = scene.drawables.back();

	drawable1.pipeline = lit_color_texture_program_pipeline;
	drawable1.pipeline.vao = wm_meshes_for_lit_color_texture_program;
	drawable1.pipeline.type = mesh1.type;
	drawable1.pipeline.start = mesh1.start;
	drawable1.pipeline.count = mesh1.count;

	drawable1.transform->position = glm::vec3(1.0f, 0.0f, -0.5f);

	add_walk_mesh();

	{
		Mesh mesh = phonebank_meshes->lookup("Sphere");

		auto newTrans = new Scene::Transform();
		scene.drawables.emplace_back(newTrans);
		Scene::Drawable& drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;
		drawable.pipeline.vao = phonebank_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;
		drawable.transform->position = glm::vec3(-2.5f, 0.0f, 1.4f);
		drawable.transform->rotation *= glm::angleAxis(
			glm::radians(-90.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));

		//create a player transform:
		//scene.transforms.emplace_back();
		player.transform = drawable.transform;// &scene.transforms.back();
	}
	Mesh mesh = upper_bun_meshes->lookup("Sphere");

	auto newTrans = new Scene::Transform();
	scene.drawables.emplace_back(newTrans);
	Scene::Drawable& drawable = scene.drawables.back();

	drawable.pipeline = lit_color_texture_program_pipeline;
	drawable.pipeline.vao = u_bun_meshes_for_lit_color_texture_program;
	drawable.pipeline.type = mesh.type;
	drawable.pipeline.start = mesh.start;
	drawable.pipeline.count = mesh.count;
	drawable.transform->parent = player.transform;
	drawable.transform->position = glm::vec3(0.0f, 0.0f, 0.0f);

	u_bun = drawable.transform;

	put_food_randomly();

	//create a player camera attached to a child of the player transform:
	scene.transforms.emplace_back();
	scene.cameras.emplace_back(&scene.transforms.back());
	player.camera = &scene.cameras.back();
	player.camera->fovy = glm::radians(60.0f);
	player.camera->near = 0.01f;
	player.camera->transform->parent = player.transform;

	//player's eyes are 1.8 units above the ground:
	player.camera->transform->position = glm::vec3(0.0f, -1.0f, 0.3f);

	//rotate camera facing direction (-z) to player facing direction (+y):
	player.camera->transform->rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//start player walking at nearest walk point:
	player.at = walkmesh->nearest_walk_point(player.transform->position);

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			glm::vec3 upDir = walkmesh->to_world_smooth_normal(player.at);
			player.transform->rotation = glm::angleAxis(-motion.x * player.camera->fovy, upDir) * player.transform->rotation;

			float pitch = glm::pitch(player.camera->transform->rotation);
			pitch += motion.y * player.camera->fovy;
			//camera looks down -z (basically at the player's feet) when pitch is at zero.
			pitch = std::min(pitch, 0.95f * 3.1415926f);
			pitch = std::max(pitch, 0.05f * 3.1415926f);
			player.camera->transform->rotation = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));

			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	//player walking:
	{
		//combine inputs into a move:
		constexpr float PlayerSpeed = 3.0f;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		//get move in world coordinate system:
		glm::vec3 remain = player.transform->make_local_to_world() * glm::vec4(move.x, move.y, 0.0f, 0.0f);

		//using a for() instead of a while() here so that if walkpoint gets stuck in
		// some awkward case, code will not infinite loop:
		for (uint32_t iter = 0; iter < 10; ++iter) {
			if (remain == glm::vec3(0.0f)) break;
			WalkPoint end;
			float time;
			walkmesh->walk_in_triangle(player.at, remain, &end, &time);
			player.at = end;
			if (time == 1.0f) {
				//finished within triangle:
				remain = glm::vec3(0.0f);
				break;
			}
			//some step remains:
			remain *= (1.0f - time);
			//try to step over edge:
			glm::quat rotation;
			if (walkmesh->cross_edge(player.at, &end, &rotation)) {
				//stepped to a new triangle:
				player.at = end;
				//rotate step to follow surface:
				remain = rotation * remain;
			} else {
				//ran into a wall, bounce / slide along it:
				glm::vec3 const &a = walkmesh->vertices[player.at.indices.x];
				glm::vec3 const &b = walkmesh->vertices[player.at.indices.y];
				glm::vec3 const &c = walkmesh->vertices[player.at.indices.z];
				glm::vec3 along = glm::normalize(b-a);
				glm::vec3 normal = glm::normalize(glm::cross(b-a, c-a));
				glm::vec3 in = glm::cross(normal, along);

				//check how much 'remain' is pointing out of the triangle:
				float d = glm::dot(remain, in);
				if (d < 0.0f) {
					//bounce off of the wall:
					remain += (-1.25f * d) * in;
				} else {
					//if it's just pointing along the edge, bend slightly away from wall:
					remain += 0.01f * d * in;
				}
			}
		}

		if (remain != glm::vec3(0.0f)) {
			std::cout << "NOTE: code used full iteration budget for walking." << std::endl;
		}

		//update player's position to respect walking:
		player.transform->position = walkmesh->to_world_point(player.at);

		uint8_t x = 0;
		for (Ingredient i : ingredients) {
			if (player.transform->position.x > (i.ingre->position.x - 0.2f) && player.transform->position.x < (i.ingre->position.x + 0.2f)) {
				if (player.transform->position.y > (i.ingre->position.y - 0.2f) && player.transform->position.y < (i.ingre->position.y + 0.2f)) {
					put_ingre_to_burger(x);
					put_food_randomly();
				}
			}
			x++;
		}

		{ //update player's rotation to respect local (smooth) up-vector:
			
			glm::quat adjust = glm::rotation(
				player.transform->rotation * glm::vec3(0.0f, 0.0f, 1.0f), //current up vector
				walkmesh->to_world_smooth_normal(player.at) //smoothed up vector at walk location
			);
			player.transform->rotation = glm::normalize(adjust * player.transform->rotation);
		}

		/*
		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 right = frame[0];
		//glm::vec3 up = frame[1];
		glm::vec3 forward = -frame[2];

		camera->transform->position += move.x * right + move.y * forward;
		*/
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	player.camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*player.camera);

	/* In case you are wondering if your walkmesh is lining up with your scene, try:
	{
		glDisable(GL_DEPTH_TEST);
		DrawLines lines(player.camera->make_projection() * glm::mat4(player.camera->transform->make_world_to_local()));
		for (auto const &tri : walkmesh->triangles) {
			lines.draw(walkmesh->vertices[tri.x], walkmesh->vertices[tri.y], glm::u8vec4(0x88, 0x00, 0xff, 0xff));
			lines.draw(walkmesh->vertices[tri.y], walkmesh->vertices[tri.z], glm::u8vec4(0x88, 0x00, 0xff, 0xff));
			lines.draw(walkmesh->vertices[tri.z], walkmesh->vertices[tri.x], glm::u8vec4(0x88, 0x00, 0xff, 0xff));
		}
	}
	*/

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("Mouse motion looks; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion looks; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
		lines.draw_text("Height: " + std::to_string(max_height*10),
			glm::vec3(-aspect + 0.1f * H + ofs, /*-1.0 + + 0.1f * H + ofs*/0.9, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
	GL_ERRORS();
}


void PlayMode::put_food_randomly(uint16_t ingre_index) {
	std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
	std::uniform_real_distribution<float> distx(-2.5f, 7.5f);
	std::uniform_real_distribution<float> disty(-2.5f, 2.5f);
	std::uniform_int_distribution<uint16_t> ing(0, 2);
	float rand_x = distx(rng);
	float rand_y = disty(rng);
	ingre_index = ing(rng);

	switch (ingre_index)
	{
	case Patty: {
		Mesh mesh = patty_meshes->lookup("Cylinder");

		auto newTrans = new Scene::Transform();
		scene.drawables.emplace_back(newTrans);
		Scene::Drawable& drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;
		drawable.pipeline.vao = patty_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;
		//drawable.transform->parent = player.transform;
		drawable.transform->position = glm::vec3(rand_x, rand_y, max_height);
		ingredients.push_back({ drawable.transform , Patty, walkmesh->nearest_walk_point(drawable.transform->position) });
		break;
	}
	case Cheese: {
		Mesh mesh = cheese_meshes->lookup("Cube");

		auto newTrans = new Scene::Transform();
		scene.drawables.emplace_back(newTrans);
		Scene::Drawable& drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;
		drawable.pipeline.vao = cheese_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;
		drawable.transform->position = glm::vec3(rand_x, rand_y, max_height);
		
		ingredients.push_back({ drawable.transform , Cheese, walkmesh->nearest_walk_point(drawable.transform->position) });
		break;
	}
	case Veggies: {
		Mesh mesh = veggies_meshes->lookup("Plane");

		auto newTrans = new Scene::Transform();
		scene.drawables.emplace_back(newTrans);
		Scene::Drawable& drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;
		drawable.pipeline.vao = cheese_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;
		drawable.transform->position = glm::vec3(rand_x, rand_y, max_height);
		//drawable.transform->parent = player.transform;.
		ingredients.push_back({ drawable.transform , Veggies, walkmesh->nearest_walk_point(drawable.transform->position) });
		break;
	}
	default:
		assert(false && "Something's wrong");
		break;
	}
	return;
}

void PlayMode::put_ingre_to_burger(uint8_t index) {
	ingredients[index].ingre->parent = player.transform;
	ingredients[index].ingre->position = glm::vec3(0.0f, 0.0f, max_height+height[ingredients[index].index]);
	max_height += height[ingredients[index].index];
	u_bun->position = glm::vec3(0.0f, 0.0f, max_height);
}

void PlayMode::add_walk_mesh(uint8_t n) {
	Mesh mesh = wm_meshes->lookup("Plane.001");
		
	for (; n > 0; n--) {
		auto newTrans = new Scene::Transform();
		scene.drawables.emplace_back(newTrans);
		Scene::Drawable& drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;
		drawable.pipeline.vao = wm_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

		drawable.transform->position = glm::vec3(n * 10.0f, 0.0f, -0.5f);

		if (n % 2) {
			drawable.transform->rotation *= glm::angleAxis(
				glm::radians(180.0f),
				glm::vec3(0.0f, 0.0f, 1.0f));
			// The origin of the mesh is not the "center" of the mesh, offsetting
			drawable.transform->position = glm::vec3(n * 10.0f + 6.0f, 0.0f, -0.5f);
		}
	}
}