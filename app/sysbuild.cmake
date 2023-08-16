ExternalZephyrProject_Add(
    APPLICATION recovery
    SOURCE_DIR ${APP_DIR}/../recovery
    BUILD_ONLY TRUE
)
set(IMAGES ${IMAGES} "recovery")