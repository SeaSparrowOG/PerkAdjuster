#include "papyrus.h"

namespace Papyrus {
	std::vector<int> GetVersion(STATIC_ARGS) {
		std::vector<int> response = std::vector<int>();
		response.push_back(Version::MAJOR);
		response.push_back(Version::MINOR);
		response.push_back(Version::PATCH);

		return response;
	}

	bool Bind(VM& a_vm)
	{
		BIND(GetVersion);
		return true;
	}

	bool RegisterFunctions(VM* a_vm) {
		Bind(*a_vm);
		return true;
	}
}
