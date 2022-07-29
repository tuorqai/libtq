
set(BUNDLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}")

set(BrotliCommon_VERSION "1.0.9" PARENT_SCOPE)
set(BrotliCommon_LIBRARY "${BUNDLE_DIR}/lib/libbrotlicommon-static.a" PARENT_SCOPE)
set(BrotliCommon_INCLUDE_DIR "${BUNDLE_DIR}/include" PARENT_SCOPE)

set(BrotliDec_VERSION "1.0.9" PARENT_SCOPE)
set(BrotliDec_LIBRARY "${BUNDLE_DIR}/lib/libbrotlidec-static.a" PARENT_SCOPE)
set(BrotliDec_INCLUDE_DIR "${BUNDLE_DIR}/include" PARENT_SCOPE)

set(PNG_VERSION "1.6.37" PARENT_SCOPE)
set(PNG_LIBRARY "${BUNDLE_DIR}/lib/libpng16.a" PARENT_SCOPE)
set(PNG_PNG_INCLUDE_DIR "${BUNDLE_DIR}/include/libpng16" PARENT_SCOPE)

set(FreeType_VERSION "2.12.1" PARENT_SCOPE)
set(FreeType_LIBRARY "${BUNDLE_DIR}/lib/libfreetype.a" PARENT_SCOPE)
set(FreeType_INCLUDE_DIR "${BUNDLE_DIR}/include/freetype2" PARENT_SCOPE)

set(HarfBuzz_VERSION "5.0.1" PARENT_SCOPE)
set(HarfBuzz_LIBRARY "${BUNDLE_DIR}/lib/libharfbuzz.a" PARENT_SCOPE)
set(HarfBuzz_INCLUDE_DIR "${BUNDLE_DIR}/include/harfbuzz" PARENT_SCOPE)

set(Ogg_VERSION "1.3.5" PARENT_SCOPE)
set(Ogg_LIBRARY "${BUNDLE_DIR}/lib/libogg.a" PARENT_SCOPE)
set(Ogg_INCLUDE_DIR "${BUNDLE_DIR}/include" PARENT_SCOPE)

set(Vorbis_VERSION "1.3.7" PARENT_SCOPE)
set(Vorbis_LIBRARY "${BUNDLE_DIR}/lib/libvorbis.a" PARENT_SCOPE)
set(Vorbis_INCLUDE_DIR "${BUNDLE_DIR}/include" PARENT_SCOPE)

set(VorbisFile_VERSION "1.3.7" PARENT_SCOPE)
set(VorbisFile_LIBRARY "${BUNDLE_DIR}/lib/libvorbisfile.a" PARENT_SCOPE)
set(VorbisFile_INCLUDE_DIR "${BUNDLE_DIR}/include" PARENT_SCOPE)
