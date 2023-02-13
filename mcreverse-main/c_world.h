#pragma once

#include "c_minecraft.h"
#include "c_javaset.h"
#include "c_entity.h"

class c_world {
public:
	c_world( ) { }
	c_world( jobject javaclass ) {
		java_class = javaclass;
	}

	int get_entities( ) {
		c_javaset* set = new c_javaset;
		jfieldID ent_list = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( java_class ), "c", "Ljava/util/Set;" );
		return set->size( minecraft->m_jenv->GetObjectField( java_class, ent_list ) );
	}

	std::vector<std::shared_ptr<c_entity>> get_players()
	{
		jfieldID player_entities = minecraft->m_jenv->GetFieldID(minecraft->m_jenv->GetObjectClass( java_class ), "field_73010_i", "Ljava/util/List;");

		jclass list_cls = minecraft->m_jenv->FindClass("java/util/List");
		jmethodID to_array_md = minecraft->m_jenv->GetMethodID(list_cls, "toArray", "()[Ljava/lang/Object;");

		minecraft->m_jenv->DeleteLocalRef(list_cls);

		std::vector<std::shared_ptr<c_entity>> res;

		jobject obj_player_entities = minecraft->m_jenv->GetObjectField(java_class, player_entities);

		if (!obj_player_entities)
			return res;

		jobjectArray array_player_list = reinterpret_cast<jobjectArray>(minecraft->m_jenv->CallObjectMethod(obj_player_entities, to_array_md));

		if (!array_player_list)
			return res;

		size_t len = minecraft->m_jenv->GetArrayLength(array_player_list);

		for (int i = 0; i < len; ++i)
		{
			jobject player = minecraft->m_jenv->GetObjectArrayElement(array_player_list, i);
			res.push_back(std::make_shared<c_entity>(player));
		}

		minecraft->m_jenv->DeleteLocalRef(obj_player_entities);
		minecraft->m_jenv->DeleteLocalRef(array_player_list);

		return res;
	}

	// this is done incorrectly
	// http://www.minecraftforge.net/forum/topic/5782-getting-a-world-object-and-player-list/
	int get_player_entities( ) {
		c_javaset* set = new c_javaset;
		jfieldID ent_list = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( java_class ), "j", "Ljava/util/Set;" );
		return set->size( minecraft->m_jenv->GetObjectField( java_class, ent_list ) );
	}

	c_entity* get_entity( int i ) {
		c_javaset* set = new c_javaset;
		jfieldID ent_list = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( java_class ), "c", "Ljava/util/Set;" );
		return new c_entity( set->get( minecraft->m_jenv->GetObjectField( java_class, ent_list ), i ) );
	}

	void set_time( jobject java_class, jlong time ) {
		jmethodID set_time = minecraft->m_jenv->GetMethodID( minecraft->m_jenv->GetObjectClass( java_class ), "b", "(J)V" );
		minecraft->m_jenv->CallLongMethod( java_class, set_time, time );
	}

	bool is_valid( ) {
		return java_class;
	}

	void removeRef()
	{
		minecraft->m_jenv->DeleteLocalRef(java_class);
	}
private:
	jobject java_class;
};

extern c_world* world;