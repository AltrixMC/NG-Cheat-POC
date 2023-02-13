#include "c_minecraft.h"
#include "c_gameinstance.h"
#include "c_entity.h"
#include "c_world.h"
#include "c_gui.h"

#include "c_legit.h"
#include "c_movement.h"
#include "utils.h"

bool AreSame(float a, float b)
{
	return fabs(a - b) < std::numeric_limits<float>::epsilon();
}

vector<shared_ptr<c_entity>> get_new_list(c_entity* player, c_world* worldObj) {
	double dist = 14;
	vector<shared_ptr<c_entity>> new_list;
	for (auto& entity : worldObj->get_players())
	{
		auto entity_position = entity->position();
		auto position = player->position();
		if (AreSame(position.x, entity_position.x) && AreSame(position.y, entity_position.y) && AreSame(position.z, entity_position.z)) continue;

		/// Basic stuff
		if (sdk::util::distance(position.x, position.y, position.z, entity_position.x, entity_position.y, entity_position.z) <= dist)
		{
			new_list.push_back(entity);
		} else
		{
			entity->removeRef();
		}
	}

	return new_list;
};
/* ~~ our hooking function. ~~ */
void c_main::hook(void) {
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	// wait for java virtual machine
	while (!(minecraft->m_jvm_dll = GetModuleHandleA(xor ("jvm.dll")))) {
		std::cout << "jvm" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	while (!(minecraft->m_opengl_dll = GetModuleHandleA(xor ("opengl32.dll")))) {
		std::cout << "opengl32" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	while (!(minecraft->m_kernel32_dll = GetModuleHandleA(xor ("Kernel32.dll")))) {
		std::cout << "Kernel32" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	// lambda function to recognise window titles.
	auto get_window_title = [&](void) {
		// buffer
		char title[256];

		// get current window handle
		m_hwnd = GetForegroundWindow();
		GetWindowTextA(m_hwnd, title, sizeof(title));

		std::string m_title = title;
		return m_title;
	};

	// find game through previously explained lambda function
	while (!(get_window_title().find(xor ("NationsGlory")) != std::string::npos)) {
		std::cout << "title" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	// get the jvm and jni enviroment
	jsize count;
	if (JNI_GetCreatedJavaVMs(&minecraft->m_jvm, 1, &count) != JNI_OK || count == 0) {
		std::cout << "pas ok" << std::endl;
		return;
	}

	std::cout << "get env" << std::endl;

	jint res = minecraft->m_jvm->GetEnv((void**)&minecraft->m_jenv, JNI_VERSION_1_8);
	if (res == JNI_EDETACHED) {
		std::cout << "attach" << std::endl;
		res = minecraft->m_jvm->AttachCurrentThread((void**)&minecraft->m_jenv, nullptr);
	}

	std::cout << "after attach" << std::endl;


	if (res != JNI_OK) {
		return;
	}

	std::cout << "get base addresses for hooking" << std::endl;

	// get base addresses for hooking
	minecraft->m_wgl_swap_buffers = GetProcAddress(minecraft->m_opengl_dll, xor ("wglSwapBuffers"));
	minecraft->m_open_process = GetProcAddress(minecraft->m_kernel32_dll, xor ("OpenProcess"));
	minecraft->m_create_remote_thread = GetProcAddress(minecraft->m_kernel32_dll, xor ("CreateRemoteThread"));
	minecraft->m_enum_process_modules = GetProcAddress(minecraft->m_kernel32_dll, xor ("EnumProcessModules"));

	// start hooking
	m_wndproc = (WNDPROC)SetWindowLongPtrA(m_hwnd, GWLP_WNDPROC, (LONG_PTR)hooks::wndproc);

	MH_Initialize();
	MH_CreateHook(minecraft->m_wgl_swap_buffers, hooks::wgl_swap_buffers, reinterpret_cast<void**>(&original_wgl_swap_buffers));
	MH_EnableHook(minecraft->m_wgl_swap_buffers);

	MH_CreateHook(minecraft->m_open_process, hooks::open_process, reinterpret_cast<void**>(&original_open_process));
	MH_EnableHook(minecraft->m_open_process);

	MH_CreateHook(minecraft->m_create_remote_thread, hooks::create_remote_thread, reinterpret_cast<void**>(&original_create_remote_thread));
	MH_EnableHook(minecraft->m_create_remote_thread);

	MH_CreateHook(minecraft->m_enum_process_modules, hooks::enum_process_modules, reinterpret_cast<void**>(&original_enum_process_modules));
	MH_EnableHook(minecraft->m_enum_process_modules);

	while (!ctx.m_unload) {
		if (GetAsyncKeyState(VK_DELETE)) {
			ctx.m_unload = true;
		}

		if (minecraft->classLoader == nullptr)
		{
			minecraft->game->getLaunchWrapper();
		}

		auto playerObject = minecraft->game->get_player();
		if (!playerObject)
		{
			continue;
		}

		auto player = minecraft->game->get_local();
		auto worldObj = minecraft->game->get_world();

		player->set_step_height(minecraft->step_height);
		player->set_flight(minecraft->fly);
		player->set_flight_speed(minecraft->fly_speed);

		minecraft->game->set_blockReach(minecraft->block_reach);

		if (minecraft->destroy)
		{
			auto list = get_new_list(player, worldObj);
			if (!list.empty())
			{
				int index = rand() % list.size(); // pick a random index
				auto entity = list[index]; // a random value taken from that list

				auto entity_position = entity->position();
				auto position = player->position();
				if (AreSame(position.x, entity_position.x) && AreSame(position.y, entity_position.y) && AreSame(position.z, entity_position.z)) continue;

				entity->get_bounding_box()->set_native_boundingbox(player->get_bounding_box()->get_native_boundingbox());

				entity->removeRef();
			}
			list.clear();
		}
		else if (minecraft->aimbot)
		{
			const auto get_closest_player = [&]() {
				double dist = 14;
				std::shared_ptr<c_entity> target = nullptr;

				for (auto& entity : worldObj->get_players())
				{
					auto entity_position = entity->position();
					auto position = player->position();
					if (AreSame(position.x, entity_position.x) && AreSame(position.y, entity_position.y) && AreSame(position.z, entity_position.z)) continue;

					/// Basic stuff
					if (sdk::util::distance(position.x, position.y, position.z, entity_position.x, entity_position.y, entity_position.z) <= dist)
					{
						if (target != nullptr) target->removeRef();

						dist = sdk::util::distance(position.x, position.y, position.z, entity_position.x, entity_position.y, entity_position.z);
						target = entity;
					}
					else
					{
						entity->removeRef();
					}
				}

				return target;
			};

			auto closest = get_closest_player();
			if (closest != nullptr)
			{
				auto bb = player->get_bounding_box()->get_native_boundingbox();

				bb.maxX += 20;
				bb.maxY += 20;
				bb.maxZ += 20;
				bb.minX -= 20;
				bb.minY -= 20;
				bb.minZ -= 20;
				closest->get_bounding_box()->set_native_boundingbox(bb);
				closest->removeRef();
			}
		}

		player->removeRef();
		delete player;
		worldObj->removeRef();
		delete worldObj;
		minecraft->m_jenv->DeleteLocalRef(playerObject);


		Sleep(30);
	}
}

/* ~~ our unhooking function ~~ */
void c_main::unhook( void ) {
	// replace with original wndproc
	SetWindowLongPtrA( m_hwnd, GWLP_WNDPROC, ( LONG_PTR ) m_wndproc );

	MH_RemoveHook( minecraft->m_wgl_swap_buffers );
	MH_RemoveHook( minecraft->m_open_process );
	MH_RemoveHook( minecraft->m_create_remote_thread );
	MH_RemoveHook( minecraft->m_enum_process_modules );
	MH_Uninitialize( );

	minecraft->m_jvm->DetachCurrentThread( );
}

/* ~~ global sanity check ~~ */
bool c_context::sane( void ) {
	return ( ctx.m_player->is_valid( ) && ctx.m_world->is_valid( ) );
}

/* ~~ define our external variables ~~ */
c_minecraft* minecraft = new c_minecraft( );
c_context    ctx;