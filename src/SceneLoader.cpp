#include "SceneLoader.h"

OPint SceneLoader(OPstream* str, OPscript** script) {
	*script = (OPscript*)OPalloc(sizeof(OPscript));
	OPlog("allocated script");
	const OPchar* preScript = "module.exports = ";
	(*script)->data = (OPchar*)OPalloc(str->Length + strlen(preScript));
	OPlog("allocated data");

	OPmemcpy((*script)->data, preScript, strlen(preScript));
	OPmemcpy((*script)->data + strlen(preScript), str->Data, str->Length);

	OPlog("copied memory");
	OPlog("mem %s", str->Source);
	(*script)->filename = OPstringCopy(str->Source);
	OPlog("copied filename");

#ifdef _DEBUG
	(*script)->changed = 0;
#endif

	return 1;
}

void SceneAddLoader() {
	OPassetLoader loaderOPS = {
		".opscene",
		"Scenes/",
		sizeof(OPscript),
		(OPint(*)(OPstream*, void**))SceneLoader,
		(OPint(*)(void*))OPscriptUnload,
		(OPint(*)(OPstream*, void**))OPscriptReload
	};
	OPCMAN.AddLoader(&loaderOPS);
}