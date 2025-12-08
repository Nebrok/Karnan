#pragma once

#include "ReflectionRegistry.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <sstream>

namespace Serialise
{
	void SerialiseGameObject(Reflectable* obj, std::ostream& out)
	{
		ClassInfo* classInfo = ReflectionRegistry::Find(obj->GetClassName());
		out << classInfo->Name << '\n';

        char* base = reinterpret_cast<char*>(obj);

        for (FieldInfo& field : classInfo->Fields) {
            out << field.Name << " ";
            switch (field.Type) {
            case FieldType::Int:
                out << *reinterpret_cast<int*>(base + field.Offset);
                break;
            case FieldType::Float:
                out << *reinterpret_cast<float*>(base + field.Offset);
                break;
            case FieldType::Bool:
                out << *reinterpret_cast<bool*>(base + field.Offset);
                break;
            case FieldType::String:
                out << *reinterpret_cast<std::string*>(base + field.Offset);
                break;
            case FieldType::Vec3:
                out << *reinterpret_cast<float*>(base + field.Offset);
                out << *reinterpret_cast<float*>(base + field.Offset + 1);
                out << *reinterpret_cast<float*>(base + field.Offset + 2);
                break;
            case FieldType::Vec2:
                out << *reinterpret_cast<float*>(base + field.Offset);
                out << *reinterpret_cast<float*>(base + field.Offset + 1);
                break;
            }
            out << "\n";
        }



	}
}
