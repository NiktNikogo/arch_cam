set(embed_files
	"www/index.html.gz"
)

idf_component_register(
	SRC_DIRS .
	INCLUDE_DIRS .
	EMBED_FILES ${embed_files}
)
