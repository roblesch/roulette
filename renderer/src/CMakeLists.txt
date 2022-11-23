add_library(core
	"scene.cpp"
	"scene.h"
)
target_link_libraries(core
	glm
	pugixml
)
