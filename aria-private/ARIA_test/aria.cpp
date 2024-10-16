#include "aria.h"
#include "dllload.h"

AriaEncKeySetup ariaEncKeySetup;
AriaDecKeySetup ariaDecKeySetup;
AriaCrypt ariaCrypt;

static HMODULE m_module;

DLLLOAD_STATUS loadARIA() {
    m_module = LoadLibraryA("./ARIA.dll");
    auto m_moduleLoaded = (m_module != NULL);

    if (!m_moduleLoaded) {
        return DLLLOAD_STATUS::LOAD_FAIL;
    }

    DLL_PROC_SETUP(m_module);

	bool procLoaded = true;
	
    DLL_PROC_LOAD(ariaEncKeySetup, "AriaEncKeySetup", AriaEncKeySetup);
    DLL_PROC_LOAD(ariaDecKeySetup, "AriaDecKeySetup", AriaDecKeySetup);
    DLL_PROC_LOAD(ariaCrypt, "AriaCrypt", AriaCrypt);
    DLL_PROC_RESULT(procLoaded);

    if (procLoaded) {
		return DLLLOAD_STATUS::LOAD_SUCCESS;
	}
	else {
        freeARIA();

		return DLLLOAD_STATUS::PROC_LOAD_FAIL;
	}
}

void freeARIA() {
    if (m_module) {
        FreeLibrary(m_module);
    }
}