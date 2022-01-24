#include <numbers>
#include <utility>
#include <cstdint>
#include <limits>
#include <SA2ModLoader.h>
#include <UsercallFunctionHandler.h>
#include <IniFile.hpp>
// I probably should not be including a source file, 
// but it has a function I want (matrix4x4_Lookat())
namespace flipscreen {
	#include <flipscreen.cpp>
}

// my functions for when I replace generated code with "working" code

using addr = std::uint32_t;
using dword = DWORD;
using word = WORD;
using byte = BYTE;

template<typename function_type>
constexpr auto sub(const addr address) {
	return reinterpret_cast<function_type*>(address);
}

template<typename return_type, typename... args>
constexpr auto sub_thiscall(const addr address) {
	return reinterpret_cast<return_type(__thiscall*)(args...)>(address);
}

template<typename data_type>
constexpr auto& ref(const addr address) {
	return *reinterpret_cast<data_type*>(address);
}

constexpr auto dword_ref = ref<dword>;
constexpr auto word_ref = ref<word>;
constexpr auto byte_ref = ref<byte>;
constexpr auto flt_ref = ref<float>;

struct stru {
	const addr address{};

	template<typename data_type>
	constexpr auto& field(const addr offset) const {
		return ref<data_type>(address + offset);
	}
};

// replacement functions

NJS_VECTOR* __fastcall sub_4EC770_replacement(int a1) {
	// big yikes section

	const int v1{ 9432 * a1 };
	const double v2{ flt_ref(0x1DCFF3C) };
	constexpr auto stru_1DCFF40 = stru{ 0x1DCFF40 };
	auto& stru_1DCFF40_field_194 = stru_1DCFF40.field<NJS_VECTOR>(0x194);
	addr stru_1DCFF40_gap_1AC{ 0x1DCFF40 + 0x1AC };
	NJS_VECTOR* result{ &stru_1DCFF40_field_194 + v1 };
	memcpy(
		reinterpret_cast<void*>(stru_1DCFF40_gap_1AC + v1 + 40),
		reinterpret_cast<void*>(&stru_1DCFF40_field_194 + v1),
		0x40u
	);

	// idk?

	result[5].x = static_cast<float>(v2);

	// position

	result->x = flt_ref(0x1DCFF0C);
	result->y = flt_ref(0x1DCFF10);
	result->z = flt_ref(0x1DCFF14);

	// rotation
	// vector pointing from the camera's position to player

	const auto& delta = result[2] = ref<NJS_VECTOR>(0x1DCFF24);

	// controls, sort of?
	// uses x and y, as signed words for some reason (not floats...)
	// no clue what x does
	// y seems to change what way forward goes
	// angle is [word min, word max]
	// z seems to control camera roll, why?

	constexpr auto word_max =
		static_cast<float>((std::numeric_limits<word>::max)());
	constexpr auto pi = std::numbers::pi_v<float>;
	constexpr auto rad_to_word = [](float rad) -> word {
		return static_cast<word>(word_max * rad / (2.f * pi));
	};

	word x{ word_ref(0x1DCFF18) };
	word y{ word_ref(0x1DCFF1C) };
	word z{ word_ref(0x1DCFF20) };
	// this math was trial and errored, so I can't explain when 
	// positive/negative values are used, nor the order of parameters
	if (CurrentLevel == LevelIDs_CrazyGadget) {
		switch (GravityDirection) {
		case GravityDirectionType::DownwardX:
			y = rad_to_word(atan2f(-delta.y, -delta.z));
			break;
		case GravityDirectionType::UpwardX:
			y = rad_to_word(atan2f(delta.y, -delta.z));
			break;
		case GravityDirectionType::DownwardY:
			// if there is some math I can put here, I do not know it
			// instead, I will hack the controls :(
			break;
		case GravityDirectionType::DownwardZ:
			y = rad_to_word(atan2f(-delta.x, -delta.y));
			break;
		case GravityDirectionType::UpwardZ:
			y = rad_to_word(atan2f(-delta.x, delta.y));
			break;
		}
	}
	result[1].x = *reinterpret_cast<float*>(&x);
	result[1].y = *reinterpret_cast<float*>(&y);
	result[1].z = *reinterpret_cast<float*>(&z);

	// this seems to be the position of what the camera is looking at, 
	// but I believe result[2] is used over it most the time. weird things do 
	// seem to happen with this frozen though.

	result[3].x = flt_ref(0x1DCFF30);
	result[3].y = flt_ref(0x1DCFF34);
	result[3].z = flt_ref(0x1DCFF38);
	
	// below does not seem to do much, seems set with slight zoom out after
	// rotating camera, but that effect still applies without this...

	result[4].x = result->x;
	result[4].y = result->y;
	result[4].z = result->z;
	
	result[4].x -= flt_ref(stru_1DCFF40_gap_1AC + v1 + 40);
	result[4].y -= flt_ref(stru_1DCFF40_gap_1AC + v1 + 44);
	result[4].z -= flt_ref(stru_1DCFF40_gap_1AC + v1 + 48);

	return result;
}

flipscreen::Matrix4x4* matrix4x4_Lookat_replacement(
	flipscreen::Vector3* origin,
	flipscreen::Vector3* target,
	flipscreen::Vector3* up,
	flipscreen::Matrix4x4* output
) {
	if (CurrentLevel == LevelIDs_CrazyGadget) {
		// perhaps we should combine the up parameter with this hardcoded up, 
		// instead of replacing it?
		flipscreen::Vector3 up_replacement{ 0.f, 1.f, 0.f };
		switch (GravityDirection) {
		case GravityDirectionType::DownwardX:
			up_replacement = { -1.f, 0.f, 0.f };
			break;
		case GravityDirectionType::UpwardX:
			up_replacement = { 1.f, 0.f, 0.f };
			break;
		case GravityDirectionType::DownwardY:
			up_replacement = { 0.f, -1.f, 0.f };
			break;
		case GravityDirectionType::DownwardZ:
			up_replacement = { 0.f, 0.f, -1.f };
			break;
		case GravityDirectionType::UpwardZ:
			up_replacement = { 0.f, 0.f, 1.f };
			break;
		}
		return flipscreen::matrix4x4_Lookat(
			origin,
			target,
			&up_replacement,
			output
		);
	}
	return flipscreen::matrix4x4_Lookat(origin, target, up, output);
}

// stolen from flipscreen
void __declspec(naked) matrix4x4_Lookat_hook_replacement() {
	__asm {
		push        edx
		push        ecx
		push        ebx
		push        dword ptr[esp + 14h]
		push        dword ptr[esp + 14h]
		push        eax
		call        matrix4x4_Lookat_replacement
		add         esp, 0Ch
		pop         ebx
		pop         ecx
		pop         edx
		ret
	}
}

extern "C" {
	__declspec(dllexport) void Init(
		const char* path,
		const HelperFunctions& helperFunctions
	) {
		// replace some random camera function
		WriteJump(
			reinterpret_cast<void*>(0x4EC770),
			sub_4EC770_replacement
		);

		// my flipscreen "compatibility" fix (ie, stealing their entire mod)
		const IniFile* settings =
			new IniFile(std::string(path) + "\\config.ini");

		std::string sFlipmode = settings->getString("Settings", "Flipmode");
		if (sFlipmode._Equal("Horizontal")) {
			flipscreen::active_flipmode =
				flipscreen::flipmode::flipmode_Horizontal;
		}
		else if (sFlipmode._Equal("Vertical")) {
			flipscreen::active_flipmode =
				flipscreen::flipmode::flipmode_Vertical;
		}

		flipscreen::rotationRadians =
			settings->getFloat("Settings", "Rotate Screen") /
			flipscreen::Rad2Deg;
		flipscreen::rotationSpeed =
			settings->getFloat("Settings", "Rotation Animation Speed") /
			flipscreen::Rad2Deg;

		delete settings;

		// my flipscreen::hookFlipScreen() replacement
		WriteJump(
			reinterpret_cast<void*>(0x427AA0),
			matrix4x4_Lookat_hook_replacement
		);
	}

	__declspec(dllexport) void __cdecl OnInput() {
		// hacking controls, because I could not figure out how to make the 
		// player move "normally" whilst upside down :(
		if (CurrentLevel == LevelIDs_CrazyGadget) {
			if (GravityDirection == GravityDirectionType::DownwardY) {
				ControllerPointers[0]->x1 = -ControllerPointers[0]->x1;
				ControllerPointers[0]->x2 = -ControllerPointers[0]->x2;
				std::swap(ControllerPointers[0]->l, ControllerPointers[0]->r);
			}
		}

	}

	__declspec(dllexport) ModInfo SA2ModInfo { ModLoaderVer };
}
