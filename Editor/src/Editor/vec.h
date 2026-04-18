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
		T& operator[](size_t index) {
			if (index >= data.size()) {
				Log::GetCoreLogger()->Error("Attempting to access element from an empty vector or index out of bounds");
				Log::GetCoreLogger()->Info("Adding temporary texture to vector");
				ASSERT(false, "Attempting to access element from an empty vector or index out of bounds");
				/*if constexpr (std::is_same_v<T, Ref<Texture2D>> || std::is_same_v<T, Scope<Texture2D>>) {
					this->emplace_back(Texture2D::Load("temp.png"));
					temp_index = data.size() - 1;
				}
				else {
					
				}
				return data[temp_index];*/
			}
			return data[index];

		}

		bool empty() const { return data.empty(); }
		void erase(size_t index) {
			if (index >= data.size()) {
				Log::GetCoreLogger()->Error("Attempting to erase element from an empty vector or index out of bounds");
				return;
			}
			
			data.erase(data.begin() + index);
		}

		size_t size() const { return data.size(); }
		operator size_t() const { return data.size(); }

		void reserve(size_t new_capacity) { data.reserve(new_capacity); }
		void resize(size_t new_size) { data.resize(new_size); }

		void push_back(const T& value) {
			TEXTURE_LOAD_FAILURE_WARNING(value);
			if (temp_index != -1) {
				data.clear();
				temp_index = -1;
			}
			data.emplace_back(value); 
		}
		void clear() { data.clear(); }
		void emplace_back(T&& value) { 
			TEXTURE_LOAD_FAILURE_WARNING(value);
			if (temp_index != -1) {
				data.clear();				
				temp_index = -1;
			}
			data.emplace_back(std::move(value));
		}
	private:
		//Ref<Texture2D> fallback = Texture2D::Load("temp.png");
		std::vector<T> data;
		int temp_index = -1;
	};

}