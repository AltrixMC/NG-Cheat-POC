#pragma once

#include "c_minecraft.h"
#include "c_entity.h"
#include "c_world.h"
#include "c_rendermanager.h"
using namespace std;
class c_gameinstance {
public:
	jclass findClass(const char* name)
	{
		jstring jname = minecraft->m_jenv->NewStringUTF(name);
		jclass cls = reinterpret_cast<jclass>(minecraft->m_jenv->CallObjectMethod(minecraft->classLoader, minecraft->findClass, jname));
		minecraft->m_jenv->DeleteLocalRef(jname);
		return cls;
	}

	void getLaunchWrapper()
	{
		jclass launchWrapperClass = minecraft->m_jenv->FindClass("net/minecraft/launchwrapper/LaunchClassLoader");

		jclass launchClass = minecraft->m_jenv->FindClass("net/minecraft/launchwrapper/Launch");

		jfieldID classLoader = minecraft->m_jenv->GetStaticFieldID(launchClass, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;");

		minecraft->findClass = minecraft->m_jenv->GetMethodID(launchWrapperClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
		minecraft->classLoader = minecraft->m_jenv->NewGlobalRef(minecraft->m_jenv->GetStaticObjectField(launchClass, classLoader));

		minecraft->m_jenv->DeleteLocalRef(launchWrapperClass);
		minecraft->m_jenv->DeleteLocalRef(launchClass);

		if (minecraft->classLoader && minecraft->findClass)
			std::cout << "success" << std::endl;
	}

	jobject get_minecraft() {
		jclass minecraft_class = findClass("net.minecraft.client.Minecraft");
		jmethodID find_minecraft = minecraft->m_jenv->GetStaticMethodID(minecraft_class, "func_71410_x", "()Lnet/minecraft/client/Minecraft;");

		auto response = minecraft->m_jenv->CallStaticObjectMethod(minecraft_class, find_minecraft);

		minecraft->m_jenv->DeleteLocalRef(minecraft_class);
		return response;
	}

	jobject get_nationsGUI() {
		jclass nations_gui_class = findClass("net.ilexiconn.nationsgui.forge.NationsGUI");
		std::cout << nations_gui_class << std::endl;
		jfieldID find_minecraft = minecraft->m_jenv->GetStaticFieldID(nations_gui_class, "INSTANCE", "Lnet/ilexiconn/nationsgui/forge/NationsGUI;");
		std::cout << find_minecraft << std::endl;

		auto response = minecraft->m_jenv->GetObjectField(nations_gui_class, find_minecraft);
		minecraft->m_jenv->DeleteLocalRef(nations_gui_class);
		return response;
	}

	void set_blockReach(float reach) {
		jclass proxy_class = findClass("net.ilexiconn.nationsgui.forge.client.ClientProxy");
		jfieldID get_reach = minecraft->m_jenv->GetStaticFieldID(proxy_class, "blockReach", "F");
		
		minecraft->m_jenv->SetStaticFloatField(proxy_class, get_reach, reach);

		minecraft->m_jenv->DeleteLocalRef(proxy_class);
	}


	void click_mouse( ) {
		jmethodID click_mouse = minecraft->m_jenv->GetMethodID( minecraft->m_jenv->GetObjectClass( get_minecraft( ) ), "c", "()V" );
		return minecraft->m_jenv->CallVoidMethod( get_minecraft( ), click_mouse );
	}

	/*jobject get_screen( ) {
		jfieldID get_screen = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( get_minecraft( ) ), "m", "Lavi;" );
		return minecraft->m_jenv->GetObjectField( get_minecraft( ), get_screen );
	}*/

	jobject get_player( ) {
		auto jobject = get_minecraft();
		auto jclass = minecraft->m_jenv->GetObjectClass(jobject);
		jfieldID get_player = minecraft->m_jenv->GetFieldID(jclass, "field_71439_g", "Lnet/minecraft/client/entity/EntityClientPlayerMP;" );

		auto response = minecraft->m_jenv->GetObjectField(jobject, get_player);
		minecraft->m_jenv->DeleteLocalRef(jobject);
		minecraft->m_jenv->DeleteLocalRef(jclass);

		return response;
	}

	/*c_rendermanager* get_render_manager( ) {
		jfieldID render_manager = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( get_minecraft( ) ), "aa", "Lbiu;" );
		return new c_rendermanager( minecraft->m_jenv->GetObjectField( get_minecraft( ), render_manager ) );
	}*/

	c_entity* get_local( ) {
		return new c_entity( minecraft->game->get_player( ) );
	}

	c_world* get_world( ) {
		auto jobject = get_minecraft();
		auto jclass = minecraft->m_jenv->GetObjectClass(jobject);

		jfieldID get_world = minecraft->m_jenv->GetFieldID( jclass, "field_71441_e", "Lnet/minecraft/client/multiplayer/WorldClient;" );

		auto response = minecraft->m_jenv->GetObjectField(jobject, get_world);
		minecraft->m_jenv->DeleteLocalRef(jobject);
		minecraft->m_jenv->DeleteLocalRef(jclass);

		return new c_world(response);
	}

	c_world* get_net_handler( ) {
		jfieldID get_world = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( get_minecraft( ) ), "func_150729_e", "Lnet/minecraft/client/multiplayer/NetClientHandler;" );
		return new c_world( minecraft->m_jenv->GetObjectField( get_minecraft( ), get_world ) );
	}
};