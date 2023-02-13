#pragma once

#include "axisalignedbb.h"
#include "c_minecraft.h"

class c_entity {
public:
	c_entity( ) { }
	c_entity( jobject javaclass ) {
		java_class = javaclass;
	}

	vec3_t position( ) {
		return vec3_t( get_x( ), get_y( ), get_z( ) );
	}

	vec3_t size( ) {
		jfieldID width = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( java_class ), "J", "D" );
		jfieldID height = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( java_class ), "K", "D" );
		return vec3_t( minecraft->m_jenv->GetFloatField( java_class, height ), minecraft->m_jenv->GetFloatField( java_class, width ), 0.0f );
	}

	void set_position( vec3_t pos ) {
		jmethodID set_position = minecraft->m_jenv->GetMethodID( minecraft->m_jenv->GetObjectClass( java_class ), "b", "(DDD)V" );
		return minecraft->m_jenv->CallVoidMethod( java_class, set_position, pos.x, pos.y, pos.z );
	}

	vec3_t angles( ) {
		return vec3_t( get_pitch( ), get_yaw( ), 0.f );
	}

	int index( ) {
		jmethodID index = minecraft->m_jenv->GetMethodID( minecraft->m_jenv->GetObjectClass( java_class ), "F", "()I" );
		return minecraft->m_jenv->CallIntMethod( java_class, index );
	}

	std::string name( ) {
		jmethodID get_name = minecraft->m_jenv->GetMethodID( minecraft->m_jenv->GetObjectClass( java_class ), "e_", "()Ljava/lang/String;" );
		jstring name = ( jstring ) minecraft->m_jenv->CallObjectMethod( java_class, get_name );

		jboolean isCopy = false;
		return std::string( minecraft->m_jenv->GetStringUTFChars( name, &isCopy) );
	}

	bool is_valid( ) {
		return java_class;
	}

	bool is_item( ) {
		return name( ).find( "item.item" ) != std::string::npos;
	}

	bool is_alive( ) {
		jmethodID alive = minecraft->m_jenv->GetMethodID( minecraft->m_jenv->GetObjectClass( java_class ), "ai", "()Z" );
		return minecraft->m_jenv->CallBooleanMethod( java_class, alive );
	}

	void set_yaw( jfloat yaw ) {
		jfieldID set_yaw = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( java_class ), "y", "F" );
		minecraft->m_jenv->SetFloatField( java_class, set_yaw, yaw );
	}

	void set_pitch( jfloat pitch ) {
		jfieldID set_pitch = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( java_class ), "z", "F" );
		minecraft->m_jenv->SetFloatField( java_class, set_pitch, pitch );
	}

	void set_sprint( jboolean sprinting ) {
		jmethodID set_sprint = minecraft->m_jenv->GetMethodID( minecraft->m_jenv->GetObjectClass( java_class ), "d", "(Z)V" );
		minecraft->m_jenv->CallBooleanMethod( java_class, set_sprint, sprinting );
	}

	void set_step_height( jfloat height ) {
		auto jclass = minecraft->m_jenv->GetObjectClass(java_class);
		jfieldID step_height = minecraft->m_jenv->GetFieldID(jclass, "field_70138_W", "F" );

		minecraft->m_jenv->SetFloatField( java_class, step_height, height );

		minecraft->m_jenv->DeleteLocalRef(jclass);
	}

	jobject get_capabilities( ) {
		auto jclass = minecraft->m_jenv->GetObjectClass(java_class);
		jfieldID get_capabilities = minecraft->m_jenv->GetFieldID(jclass, "field_71075_bZ", "Lnet/minecraft/entity/player/PlayerCapabilities;" );

		minecraft->m_jenv->DeleteLocalRef(jclass);

		return minecraft->m_jenv->GetObjectField( java_class, get_capabilities );
	}

	void set_flight( jboolean state ) {
		auto jobject = get_capabilities();
		auto jclass = minecraft->m_jenv->GetObjectClass(jobject);

		jfieldID set_flight = minecraft->m_jenv->GetFieldID(jclass, "field_75101_c", "Z" );
		minecraft->m_jenv->SetBooleanField( get_capabilities( ), set_flight, state );

		minecraft->m_jenv->DeleteLocalRef(jclass);
		minecraft->m_jenv->DeleteLocalRef(jobject);
	}
	void set_flight_speed( jfloat state ) {
		auto jobject = get_capabilities();
		auto jclass = minecraft->m_jenv->GetObjectClass(jobject);

		jmethodID set_flight_speed = minecraft->m_jenv->GetMethodID(jclass, "func_75092_a", "(F)V");
		minecraft->m_jenv->CallFloatMethod(get_capabilities(), set_flight_speed, state);

		minecraft->m_jenv->DeleteLocalRef(jclass);
		minecraft->m_jenv->DeleteLocalRef(jobject);
	}

	void set_health( jfloat state ) {
		jmethodID set_health = minecraft->m_jenv->GetMethodID(minecraft->m_jenv->GetObjectClass(java_class), "func_70606_j", "(F)V");
		minecraft->m_jenv->CallFloatMethod(java_class, set_health, state);
	}

	void set_player_walk_speed( jfloat state ) {
		jmethodID set_player_walk_speed = minecraft->m_jenv->GetMethodID(minecraft->m_jenv->GetObjectClass(get_capabilities()), "func_82877_b", "(F)V");
		minecraft->m_jenv->CallFloatMethod(get_capabilities(), set_player_walk_speed, state);
	}

	void set_creative( jboolean state ) {
		jfieldID is_creative = minecraft->m_jenv->GetFieldID(minecraft->m_jenv->GetObjectClass(get_capabilities()), "field_75098_d", "Z");
		minecraft->m_jenv->SetBooleanField(get_capabilities(), is_creative, state);
	}

	std::shared_ptr<c_axisalignedbb> get_bounding_box()
	{
		auto player_class = minecraft->m_jenv->GetObjectClass(java_class);
		jfieldID boundingbox_fid;

		boundingbox_fid = minecraft->m_jenv->GetFieldID(player_class, "field_70121_D", "Lnet/minecraft/util/AxisAlignedBB;");

		minecraft->m_jenv->DeleteLocalRef(player_class);

		return std::make_shared<c_axisalignedbb>(minecraft->m_jenv->GetObjectField(java_class, boundingbox_fid));
	}

	float get_pitch() {
		jfieldID get_pitch = minecraft->m_jenv->GetFieldID(minecraft->m_jenv->GetObjectClass(java_class), "z", "F");
		return minecraft->m_jenv->GetFloatField(java_class, get_pitch);
	}

	float get_yaw() {
		jfieldID get_yaw = minecraft->m_jenv->GetFieldID(minecraft->m_jenv->GetObjectClass(java_class), "field_70177_z", "F");
		return minecraft->m_jenv->GetFloatField(java_class, get_yaw);
	}

	void set_x(float x) {
		jfieldID get_x = minecraft->m_jenv->GetFieldID(minecraft->m_jenv->GetObjectClass(java_class), "field_70165_t", "D");
		return minecraft->m_jenv->SetDoubleField(java_class, get_x, x);
	}

	void set_y(float y) {
		jfieldID get_y = minecraft->m_jenv->GetFieldID(minecraft->m_jenv->GetObjectClass(java_class), "field_70163_u", "D");
		return minecraft->m_jenv->SetDoubleField(java_class, get_y, y);
	}

	void set_z(float z) {
		jfieldID get_z = minecraft->m_jenv->GetFieldID(minecraft->m_jenv->GetObjectClass(java_class), "field_70161_v", "D");
		return minecraft->m_jenv->SetDoubleField(java_class, get_z, z);
	}

	void removeRef()
	{
		minecraft->m_jenv->DeleteLocalRef(java_class);
	}

private:
	jobject java_class;

	float get_x( ) {
		auto jclass = minecraft->m_jenv->GetObjectClass(java_class);

		jfieldID get_x = minecraft->m_jenv->GetFieldID( jclass, "field_70165_t", "D" );

		minecraft->m_jenv->DeleteLocalRef(jclass);

		return minecraft->m_jenv->GetDoubleField( java_class, get_x );
	}

	float get_y( ) {
		auto jclass = minecraft->m_jenv->GetObjectClass(java_class);

		jfieldID get_y = minecraft->m_jenv->GetFieldID(jclass, "field_70163_u", "D" );

		minecraft->m_jenv->DeleteLocalRef(jclass);

		return minecraft->m_jenv->GetDoubleField( java_class, get_y );
	}

	float get_z( ) {
		auto jclass = minecraft->m_jenv->GetObjectClass(java_class);

		jfieldID get_z = minecraft->m_jenv->GetFieldID( minecraft->m_jenv->GetObjectClass( java_class ), "field_70161_v", "D" );

		minecraft->m_jenv->DeleteLocalRef(jclass);

		return minecraft->m_jenv->GetDoubleField( java_class, get_z );
	}
};

extern c_entity* entity;