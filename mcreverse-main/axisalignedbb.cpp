#include "axisalignedbb.h"

#include "c_minecraft.h"

c_axisalignedbb::c_axisalignedbb(jobject obj)
{
	axis_aligned_obj = obj;
}

c_axisalignedbb::~c_axisalignedbb()
{
	minecraft->m_jenv->DeleteLocalRef(axis_aligned_obj);
}

s_axisalignedbb c_axisalignedbb::get_native_boundingbox()
{
	auto this_clazz = minecraft->m_jenv->GetObjectClass(axis_aligned_obj);

	auto minX_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72340_a", "D"),
		minY_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72338_b", "D"),
		minZ_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72339_c", "D");

	auto maxX_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72336_d", "D"),
		maxY_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72337_e", "D"),
		maxZ_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72334_f", "D");

	minecraft->m_jenv->DeleteLocalRef(this_clazz);

	s_axisalignedbb bb{};
	bb.minX = minecraft->m_jenv->GetDoubleField(axis_aligned_obj, minX_fid);
	bb.minY = minecraft->m_jenv->GetDoubleField(axis_aligned_obj, minY_fid);
	bb.minZ = minecraft->m_jenv->GetDoubleField(axis_aligned_obj, minZ_fid);

	bb.maxX = minecraft->m_jenv->GetDoubleField(axis_aligned_obj, maxX_fid);
	bb.maxY = minecraft->m_jenv->GetDoubleField(axis_aligned_obj, maxY_fid);
	bb.maxZ = minecraft->m_jenv->GetDoubleField(axis_aligned_obj, maxZ_fid);

	return bb;
}

void c_axisalignedbb::set_native_boundingbox(s_axisalignedbb target)
{
	set(target.minX, target.minY, target.minZ, target.maxX, target.maxY, target.maxZ);
}

void c_axisalignedbb::set(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
{
	auto this_clazz = minecraft->m_jenv->GetObjectClass(axis_aligned_obj);

	auto minX_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72340_a", "D"), 
		minY_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72338_b", "D"), 
		minZ_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72339_c", "D");

	auto maxX_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72336_d", "D"),
		maxY_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72337_e", "D"),
		maxZ_fid = minecraft->m_jenv->GetFieldID(this_clazz, "field_72334_f", "D");

	minecraft->m_jenv->DeleteLocalRef(this_clazz);

	minecraft->m_jenv->SetDoubleField(axis_aligned_obj, minX_fid, minX);
	minecraft->m_jenv->SetDoubleField(axis_aligned_obj, minY_fid, minY);
	minecraft->m_jenv->SetDoubleField(axis_aligned_obj, minZ_fid, minZ);

	minecraft->m_jenv->SetDoubleField(axis_aligned_obj, maxX_fid, maxX);
	minecraft->m_jenv->SetDoubleField(axis_aligned_obj, maxY_fid, maxY);
	minecraft->m_jenv->SetDoubleField(axis_aligned_obj, maxZ_fid, maxZ);

}
