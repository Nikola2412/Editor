#pragma once

#include <vector>
#include "Log.h"
#include "Texture.h"

#define TEXTURE_LOAD_FAILURE_WARNING(value) \
	if constexpr (std::is_same_v<T, Ref<Texture2D>> || std::is_same_v<T, Scope<Texture2D>>) { \
		if (!value->IsLoaded()) { \
			Log::GetCoreLogger()->Warn("Texture was not added to list"); \
			return; \
		} \
	}

namespace Editor
{
	template <typename T>
	class vec
	{
	public:
		vec() = default;
		vec(size_t size) : data(size) {}
		vec(std::initializer_list<T> init) : data(init) {}
		T& operator[](size_t index) { return data[index]; }
		const T& operator[](size_t index) const { return data[index]; }
		size_t size() const { return data.size(); }

		void reserve(size_t new_capacity) { data.reserve(new_capacity); }
		void resize(size_t new_size) { data.resize(new_size); }

		void push_back(const T& value) {
			TEXTURE_LOAD_FAILURE_WARNING(value)
			data.emplace_back(value); 
		}
		void clear() { data.clear(); }
		void emplace_back(T&& value) { 
			TEXTURE_LOAD_FAILURE_WARNING(value)
			data.emplace_back(std::move(value));
		}
	private:
		std::vector<T> data;
	};

}