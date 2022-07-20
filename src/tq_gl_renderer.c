
//------------------------------------------------------------------------------

#if defined(TQ_USE_OPENGL)

//------------------------------------------------------------------------------

#include <string.h>

#if !defined(TQ_USE_OPENGL_ES)
#   include <GL/glew.h>
#else
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#endif

#include "tq_core.h"
#include "tq_error.h"
#include "tq_graphics.h"
#include "tq_image_loader.h"
#include "tq_log.h"
#include "tq_math.h"
#include "tq_mem.h"

//------------------------------------------------------------------------------
// OpenGL debug stuff

#if defined(NDEBUG)

#define CHECK_GL(call_) call_

#else

static char const *get_gl_error_str(GLenum error)
{
    switch (error) {
    case GL_NO_ERROR:           return "No error, everything is OK.";
    case GL_INVALID_ENUM:       return "Invalid enumeration.";
    case GL_INVALID_VALUE:      return "Invalid value.";
    case GL_INVALID_OPERATION:  return "Invalid operation.";
    case GL_OUT_OF_MEMORY:      return "Out of memory.";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
                                return "Invalid framebuffer operation.";
    }

    return "Unknown error.";
}

static void check_gl_errors(char const *call, char const *file, unsigned int line)
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        log_error("OpenGL error(s) occured in %s at line %d:\n", file, line);
        log_error("-- %d: %s\n", line, call);
    }

    while (error != GL_NO_ERROR) {
        log_error(":: [0x%x] %s\n", error, get_gl_error_str(error));
        error = glGetError();
    }
}

#define CHECK_GL(call_) \
    do { \
        call_; \
        check_gl_errors(#call_, __FILE__, __LINE__); \
    } while (0);

#endif // defined(NDEBUG)

//------------------------------------------------------------------------------

/**
 * Standard vertex shader source code.
 */
static char const *vs_src_standard =
    "attribute vec2 a_position;\n"
    "attribute vec4 a_color;\n"
    "attribute vec2 a_texCoord;\n"
    "varying vec4 v_color;\n"
    "varying vec2 v_texCoord;\n"
    "uniform mat4 u_projection;\n"
    "uniform mat4 u_modelView;\n"
    "void main() {\n"
    "    v_texCoord = a_texCoord;\n"
    "    v_color = a_color;\n"
    "    vec4 position = vec4(a_position, 0.0, 1.0);\n"
    "    gl_Position = u_projection * u_modelView * position;\n"
    "}\n";

/**
 * Solid mesh fragment shader source code.
 */
static char const *fs_src_solid =
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "    gl_FragColor = u_color;\n"
    "}\n";

/**
 * Colored mesh fragment shader source code.
 */
static char const *fs_src_colored =
    "varying vec4 v_color;\n"
    "void main() {\n"
    "    gl_FragColor = v_color;\n"
    "}\n";

/**
 * Textured mesh fragment shader source code.
 */
static char const *fs_src_textured =
    "varying vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "    gl_FragColor = texture2D(u_texture, v_texCoord);\n"
    "}\n";

/**
 * Font mesh fragment shader source code.
 */
static char const *fs_src_font =
    "varying vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "    vec4 texColor = texture2D(u_texture, v_texCoord);\n"
    "    gl_FragColor = vec4(texColor.rgb, texColor.g);\n"
    "}\n";

//------------------------------------------------------------------------------

#define INITIAL_TEXTURE_COUNT       16

/**
 * Vertex attributes.
 */
enum
{
    ATTRIB_POSITION,
    ATTRIB_COLOR,
    ATTRIB_TEXCOORD,
};

/**
 * Vertex attribute bits.
 */
enum
{
    ATTRIB_BIT_POSITION = (1 << ATTRIB_POSITION),
    ATTRIB_BIT_COLOR = (1 << ATTRIB_COLOR),
    ATTRIB_BIT_TEXCOORD = (1 << ATTRIB_TEXCOORD)
};

/**
 * Shader programs.
 */
enum
{
    PROGRAM_SOLID,
    PROGRAM_COLORED,
    PROGRAM_TEXTURED,
    PROGRAM_FONT,
    PROGRAM_COUNT,
};

/**
 * Shader uniforms.
 */
enum
{
    UNIFORM_PROJECTION,
    UNIFORM_MODELVIEW,
    UNIFORM_COLOR,
    UNIFORM_COUNT,
};

//------------------------------------------------------------------------------

struct gl_colors
{
    GLfloat clear[4];
    GLfloat draw[4];
};

struct gl_matrices
{
    float proj[16];
    float mv[16];
};

struct gl_texture
{
    GLuint handle;
    GLsizei width;
    GLsizei height;
    GLenum format;
    int channels;
};

struct gl_program
{
    GLuint handle;
    GLint uniforms[UNIFORM_COUNT];
    int dirty_uniform_bits;
};

struct gl_state
{
    int vertex_format;
    int program_id;
};

//------------------------------------------------------------------------------

static struct gl_colors colors;
static struct gl_matrices matrices;
static struct gl_texture *textures;
static int texture_count;
static struct gl_program programs[PROGRAM_COUNT];
static struct gl_state state;

//------------------------------------------------------------------------------

static void delete_texture(int texture_id);

//------------------------------------------------------------------------------

static void decode_color24(GLfloat *dst, tq_color_t color)
{
    dst[0] = ((color >> 24) & 255) / 255.0f;
    dst[1] = ((color >> 16) & 255) / 255.0f;
    dst[2] = ((color >>  8) & 255) / 255.0f;
    dst[3] = 1.0f;
}

static void decode_color32(GLfloat *dst, tq_color_t color)
{
    dst[0] = ((color >> 24) & 255) / 255.0f;
    dst[1] = ((color >> 16) & 255) / 255.0f;
    dst[2] = ((color >>  8) & 255) / 255.0f;
    dst[3] = ((color <<  0) & 255) / 255.0f;
}

static int get_texture_id(void)
{
    for (int i = 0; i < texture_count; i++) {
        if (textures[i].handle == 0) {
            return i;
        }
    }

    int next_count = TQ_MAX(texture_count * 2, INITIAL_TEXTURE_COUNT);
    size_t next_size = sizeof(struct gl_texture) * next_count;

    struct gl_texture *next_array = mem_realloc(textures, next_size);

    if (!next_array) {
        out_of_memory();
    }

    int texture_id = texture_count;

    texture_count = next_count;
    textures = next_array;

    for (int i = texture_id; i < texture_count; i++) {
        textures[i].handle = 0;
    }

    return texture_id;
}

//------------------------------------------------------------------------------
// Utility functions

/**
 * Get OpenGL render mode.
 */
static GLenum conv_mode(int mode)
{
    switch (mode) {
    case PRIMITIVE_POINTS:
        return GL_POINTS;
    case PRIMITIVE_LINE_STRIP:
        return GL_LINE_STRIP;
    case PRIMITIVE_LINE_LOOP:
        return GL_LINE_LOOP;
    case PRIMITIVE_TRIANGLES:
        return GL_TRIANGLES;
    case PRIMITIVE_TRIANGLE_FAN:
        return GL_TRIANGLE_FAN;
    }

    return GL_INVALID_ENUM;
}

/**
 * Get OpenGL texture format.
 */
static GLenum conv_texture_format(int channels)
{
    switch (channels) {
    case 1:
        return GL_LUMINANCE;
    case 2:
        return GL_LUMINANCE_ALPHA;
    case 3:
        return GL_RGB;
    case 4:
        return GL_RGBA;
    }

    return GL_INVALID_ENUM;
}

/**
 * Compile GLSL shader.
 */
static GLuint compile_shader(GLenum type, char const *source)
{
    GLuint handle = glCreateShader(type);
    CHECK_GL(glShaderSource(handle, 1, &source, NULL));
    CHECK_GL(glCompileShader(handle));

    GLint success;
    CHECK_GL(glGetShaderiv(handle, GL_COMPILE_STATUS, &success));

    if (!success) {
        GLchar buffer[1024];
        CHECK_GL(glGetShaderInfoLog(handle, sizeof(buffer), NULL, buffer));

        char const *type_str;

        if (type == GL_VERTEX_SHADER) {
            type_str = "vertex";
        } else if (type == GL_FRAGMENT_SHADER) {
            type_str = "fragment";
        } else {
            type_str = "unknown";
        }

        log_error("** Failed to compile GLSL %s shader. **\n", type_str);
        log_error("%s\n", buffer);

        return 0;
    }

    return handle;
}

/**
 * Assemble GLSL shader program.
 */
static GLuint link_program(GLuint vs, GLuint fs)
{
    GLuint handle = glCreateProgram();

    CHECK_GL(glAttachShader(handle, vs));
    CHECK_GL(glAttachShader(handle, fs));

    CHECK_GL(glBindAttribLocation(handle, ATTRIB_POSITION, "a_position"));
    CHECK_GL(glBindAttribLocation(handle, ATTRIB_COLOR, "a_color"));
    CHECK_GL(glBindAttribLocation(handle, ATTRIB_TEXCOORD, "a_texCoord"));

    CHECK_GL(glLinkProgram(handle));

    GLint success;
    CHECK_GL(glGetProgramiv(handle, GL_LINK_STATUS, &success));

    if (!success) {
        GLchar buffer[1024];
        CHECK_GL(glGetProgramInfoLog(handle, sizeof(buffer), NULL, buffer));

        log_error("** Failed to link GLSL program. **\n");
        log_error("%s\n", buffer);

        return 0;
    }

    return handle;
}

/**
 * Update current vertex format.
 * Doesn't do anything if the format is the same.
 */
static void set_vertex_format(int vertex_format)
{
    if (state.vertex_format == vertex_format) {
        return;
    }

    if (vertex_format & ATTRIB_BIT_POSITION) {
        CHECK_GL(glEnableVertexAttribArray(ATTRIB_POSITION));
    } else {
        CHECK_GL(glDisableVertexAttribArray(ATTRIB_POSITION));
    }

    if (vertex_format & ATTRIB_BIT_COLOR) {
        CHECK_GL(glEnableVertexAttribArray(ATTRIB_COLOR));
    } else {
        CHECK_GL(glDisableVertexAttribArray(ATTRIB_COLOR));
    }

    if (vertex_format & ATTRIB_BIT_TEXCOORD) {
        CHECK_GL(glEnableVertexAttribArray(ATTRIB_TEXCOORD));
    } else {
        CHECK_GL(glDisableVertexAttribArray(ATTRIB_TEXCOORD));
    }

    state.vertex_format = vertex_format;
}

/**
 * Updates all uniforms for the current shader if needed.
 */
static void apply_uniforms(void)
{
    long bits = programs[state.program_id].dirty_uniform_bits;
    GLint *location = programs[state.program_id].uniforms;

    if (bits & (1 << UNIFORM_PROJECTION)) {
        CHECK_GL(glUniformMatrix4fv(location[UNIFORM_PROJECTION], 1, GL_TRUE, matrices.proj));
    }

    if (bits & (1 << UNIFORM_MODELVIEW)) {
        CHECK_GL(glUniformMatrix4fv(location[UNIFORM_MODELVIEW], 1, GL_TRUE, matrices.mv));
    }

    if (bits & (1 << UNIFORM_COLOR)) {
        CHECK_GL(glUniform4fv(location[UNIFORM_COLOR], 1, colors.draw));
    }

    programs[state.program_id].dirty_uniform_bits = 0;
}

/**
 * Switch to different shader.
 */
static void set_program_id(int program_id)
{
    if (state.program_id == program_id) {
        return;
    }

    state.program_id = program_id;

    if (program_id == -1) {
        CHECK_GL(glUseProgram(0));
        return;
    }

    CHECK_GL(glUseProgram(programs[program_id].handle));

    if (programs[program_id].dirty_uniform_bits) {
        apply_uniforms();
    }
}

/**
 * Mark uniform for change.
 * This is needed so we don't have to update uniforms
 * for each shader. Only when shader is activated, the pending
 * uniform changes will take effect.
 */
static void set_dirty_uniform(int program_id, int uniform_id)
{
    programs[program_id].dirty_uniform_bits |= (1 << uniform_id);

    if (state.program_id == program_id) {
        apply_uniforms();
    }
}

//------------------------------------------------------------------------------

/**
 * Initialize OpenGL renderer.
 */
static void initialize(void)
{
    #ifndef TQ_USE_OPENGL_ES
        // Load OpenGL extensions.
        if (glewInit() != GLEW_OK) {
            log_error("Failed to initialize GLEW.\n");
        }
    #endif

    mat4_identity(matrices.proj);
    mat4_identity(matrices.mv);

    textures = NULL;
    texture_count = 0;

    state.vertex_format = 0;
    state.program_id = -1;

    GLuint vs_standard = compile_shader(GL_VERTEX_SHADER, vs_src_standard);
    GLuint fs_solid = compile_shader(GL_FRAGMENT_SHADER, fs_src_solid);
    GLuint fs_colored = compile_shader(GL_FRAGMENT_SHADER, fs_src_colored);
    GLuint fs_textured = compile_shader(GL_FRAGMENT_SHADER, fs_src_textured);
    GLuint fs_font = compile_shader(GL_FRAGMENT_SHADER, fs_src_font);

    programs[PROGRAM_SOLID].handle = link_program(vs_standard, fs_solid);
    programs[PROGRAM_COLORED].handle = link_program(vs_standard, fs_colored);
    programs[PROGRAM_TEXTURED].handle = link_program(vs_standard, fs_textured);
    programs[PROGRAM_FONT].handle = link_program(vs_standard, fs_font);

    for (int i = 0; i < PROGRAM_COUNT; i++) {
        programs[i].uniforms[UNIFORM_PROJECTION] = glGetUniformLocation(programs[i].handle, "u_projection");
        programs[i].uniforms[UNIFORM_MODELVIEW] = glGetUniformLocation(programs[i].handle, "u_modelView");
        programs[i].uniforms[UNIFORM_COLOR] = glGetUniformLocation(programs[i].handle, "u_color");
        programs[i].dirty_uniform_bits = 2147483647; // totally not a magic number
    }

    glDeleteShader(vs_standard);
    glDeleteShader(fs_solid);
    glDeleteShader(fs_textured);
    glDeleteShader(fs_font);

    // Reset OpenGL state.
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

#if !defined(TQ_USE_OPENGL_ES)
    glEnable(GL_MULTISAMPLE);
#endif

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Initialization is done.
    log_info("OpenGL renderer initialized.\n");
    log_info("GL_VENDOR: %s\n", glGetString(GL_VENDOR));
    log_info("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
    log_info("GL_VERSION: %s\n", glGetString(GL_VERSION));
}

/**
 * Terminate OpenGL renderer.
 */
static void terminate(void)
{
    // Delete all shader programs.
    for (int i = 0; i < PROGRAM_COUNT; i++) {
        CHECK_GL(glDeleteProgram(programs[i].handle));
    }

    // Free all textures that are still in use.
    for (int i = 0; i < texture_count; i++) {
        delete_texture(i);
    }

    mem_free(textures);
}

/**
 * Called every frame.
 */
static void process(void)
{
    // Is this even needed?
    CHECK_GL(glFlush());
}

/**
 * Update viewport.
 */
static void update_viewport(int x, int y, int w, int h)
{
    CHECK_GL(glViewport(x, y, w, h));
}

/**
 * Update projection matrix.
 */
static void update_projection(float const *mat4)
{
    mat4_copy(matrices.proj, mat4);

    for (int i = 0; i < PROGRAM_COUNT; i++) {
        set_dirty_uniform(i, UNIFORM_PROJECTION);
    }
}

/**
 * Update model-view matrix.
 */
static void update_model_view(float const *mat3)
{
    // (Note: the "view" matrix corresponds to the projection matrix,
    //  and the "transform" one roughly corresponds to the model-view matrix
    //  of classic OpenGL).
    // (Note 2: OpenGL's own matrices are not used here, since I handle
    //  this in the "tq::graphics" module independently of renderer).

    mat4_expand(matrices.mv, mat3);

    for (int i = 0; i < PROGRAM_COUNT; i++) {
        set_dirty_uniform(i, UNIFORM_MODELVIEW);
    }
}

/**
 * Generate empty texture with given parameters.
 */
static int create_texture(int width, int height, int channels)
{
    if ((width < 0) || (height < 0)) {
        return -1;
    }

    if ((channels < 1) || (channels > 4)) {
        return -1;
    }

    GLuint handle = 0;
    GLenum format = conv_texture_format(channels);

    CHECK_GL(glGenTextures(1, &handle));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, handle));

    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, format,
        width, height, 0, format,
        GL_UNSIGNED_BYTE, NULL));

    int texture_id = get_texture_id();

    textures[texture_id].handle = handle;
    textures[texture_id].width = width;
    textures[texture_id].height = height;
    textures[texture_id].format = format;
    textures[texture_id].channels = channels;

    return texture_id;
}

/**
 * Delete a texture
 */
static void delete_texture(int texture_id)
{
    if (texture_id < 0 || texture_id > texture_count || textures[texture_id].handle == 0) {
        return;
    }

    CHECK_GL(glDeleteTextures(1, &textures[texture_id].handle));
    textures[texture_id].handle = 0;
}

static void get_texture_size(int texture_id, int *width, int *height)
{
    if (texture_id < 0 || texture_id > texture_count || textures[texture_id].handle == 0) {
        *width = -1;
        *height = -1;
    } else {
        *width = textures[texture_id].width;
        *height = textures[texture_id].height;
    }
}

static void update_texture(int texture_id, int x_offset, int y_offset, int width, int height, unsigned char *pixels)
{
    glBindTexture(GL_TEXTURE_2D, textures[texture_id].handle);

    if (x_offset == 0 && y_offset == 0 && width == -1 && height == -1) {
        glTexImage2D(GL_TEXTURE_2D, 0, textures[texture_id].format,
            textures[texture_id].width, textures[texture_id].height, 0,
            textures[texture_id].format, GL_UNSIGNED_BYTE, pixels);
    } else {
        glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, width, height,
            textures[texture_id].format, GL_UNSIGNED_BYTE, pixels);
    }
}

static void resize_texture(int texture_id, int new_width, int new_height)
{
    int old_width = textures[texture_id].width;
    int old_height = textures[texture_id].height;

    unsigned char *pixels = mem_malloc(textures[texture_id].channels *
        old_width * old_height);

    if (!pixels) {
        out_of_memory();
    }

    glBindTexture(GL_TEXTURE_2D, textures[texture_id].handle);
    glGetTexImage(GL_TEXTURE_2D, 0, textures[texture_id].format, GL_UNSIGNED_BYTE, pixels);

    glTexImage2D(GL_TEXTURE_2D, 0,
        textures[texture_id].format, new_width, new_height, 0,
        textures[texture_id].format, GL_UNSIGNED_BYTE, NULL);

    if (new_width >= old_width && new_height >= old_height) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, old_width, old_height,
            textures[texture_id].format, GL_UNSIGNED_BYTE, pixels);
    }

    textures[texture_id].width = new_width;
    textures[texture_id].height = new_height;

    mem_free(pixels);
}

static void bind_texture(int texture_id)
{
    if (texture_id == -1) {
        CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
        return;
    }

    glBindTexture(GL_TEXTURE_2D, textures[texture_id].handle);
}

static void set_clear_color(tq_color_t clear_color)
{
    decode_color24(colors.clear, clear_color);
    CHECK_GL(glClearColor(colors.clear[0], colors.clear[1], colors.clear[2], 1.0f));
}

static void set_draw_color(tq_color_t draw_color)
{
    decode_color32(colors.draw, draw_color);

    for (int i = 0; i < PROGRAM_COUNT; i++) {
        programs[i].dirty_uniform_bits |= (1 << UNIFORM_COLOR);
    }

    if (state.program_id != -1) {
        apply_uniforms();
    }
}

static void clear(void)
{
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));
}

static void draw_solid(int mode, float const *data, int num_vertices)
{
    set_vertex_format(ATTRIB_BIT_POSITION);
    set_program_id(PROGRAM_SOLID);

    CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, data));
    CHECK_GL(glDrawArrays(conv_mode(mode), 0, num_vertices));
}

static void draw_colored(int mode, float const *data, int num_vertices)
{
    set_vertex_format(ATTRIB_BIT_POSITION | ATTRIB_BIT_COLOR);
    set_program_id(PROGRAM_COLORED);

    CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, data + 0));
    CHECK_GL(glVertexAttribPointer(ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, data + 2));
    CHECK_GL(glDrawArrays(conv_mode(mode), 0, num_vertices));
}

static void draw_textured(int mode, float const *data, int num_vertices)
{
    set_vertex_format(ATTRIB_BIT_POSITION | ATTRIB_BIT_TEXCOORD);
    set_program_id(PROGRAM_TEXTURED);

    CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, data + 0));
    CHECK_GL(glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, data + 2));
    CHECK_GL(glDrawArrays(conv_mode(mode), 0, num_vertices));
}

static void draw_font(float const *data, unsigned int const *indices, int num_indices)
{
    set_vertex_format(ATTRIB_BIT_POSITION | ATTRIB_BIT_TEXCOORD);
    set_program_id(PROGRAM_FONT);

    CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, data + 0));
    CHECK_GL(glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, data + 2));
    CHECK_GL(glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices));
}

//------------------------------------------------------------------------------
// Module constructor

//--------------------------------------
// Construct abstract module for OpenGL renderer.
//--------------------------------------
void construct_gl_renderer(struct renderer_impl *renderer)
{
    *renderer = (struct renderer_impl) {
        .initialize = initialize,
        .terminate = terminate,
        .process = process,
        
        .update_viewport = update_viewport,
        .update_projection = update_projection,
        .update_model_view = update_model_view,

        .create_texture = create_texture,
        .delete_texture = delete_texture,
        .get_texture_size = get_texture_size,
        .update_texture = update_texture,
        .resize_texture = resize_texture,
        .bind_texture = bind_texture,

        .set_clear_color = set_clear_color,
        .set_draw_color = set_draw_color,

        .clear = clear,
        .draw_solid = draw_solid,
        .draw_colored = draw_colored,
        .draw_textured = draw_textured,
        .draw_font = draw_font,
    };
}

//------------------------------------------------------------------------------

#endif // defined(TQ_USE_OPENGL)

//------------------------------------------------------------------------------
