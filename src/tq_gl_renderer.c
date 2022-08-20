
//------------------------------------------------------------------------------

#if defined(TQ_WIN32) || defined(TQ_LINUX)

//------------------------------------------------------------------------------

#include <string.h>

#include <GL/glew.h>

#include "tq_core.h"
#include "tq_error.h"
#include "tq_graphics.h"
#include "tq_handle_list.h"
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
    case GL_NO_ERROR:           return "No error, everything is OK";
    case GL_INVALID_ENUM:       return "Invalid enumeration";
    case GL_INVALID_VALUE:      return "Invalid value";
    case GL_INVALID_OPERATION:  return "Invalid operation";
    case GL_OUT_OF_MEMORY:      return "Out of memory";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
                                return "Invalid framebuffer operation";
    }

    return "Unknown error";
}

static void check_gl_errors(char const *call, char const *file, unsigned int line)
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        libtq_log(LIBTQ_LOG_ERROR, "OpenGL error(s) occured in %s at line %d:\n", file, line);
        libtq_log(LIBTQ_LOG_ERROR, "-- %d: %s\n", line, call);
    }

    while (error != GL_NO_ERROR) {
        libtq_log(LIBTQ_LOG_ERROR, ":: [0x%04x] %s\n", error, get_gl_error_str(error));
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
 * Backbuffer vertex shader source code.
 */
static char const *vs_src_backbuf =
    "attribute vec2 a_position;\n"
    "attribute vec2 a_texCoord;\n"
    "varying vec2 v_texCoord;\n"
    "void main() {\n"
    "    v_texCoord = a_texCoord;\n"
    "    gl_Position = vec4(a_position, 0.0, 1.0);\n"
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
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "    vec4 texColor = texture2D(u_texture, v_texCoord);\n"
    "    float alpha = texColor.r;\n"
    "    gl_FragColor = vec4(1.0, 1.0, 1.0, alpha) * u_color;\n"
    "}\n";

//------------------------------------------------------------------------------

#define DEFAULT_VBO_SIZE            256

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
 * Vertex formats.
 */
enum
{
    VERTEX_FORMAT_SOLID,        // (x, y)
    VERTEX_FORMAT_COLORED,      // (x, y), (r, g, b, a)
    VERTEX_FORMAT_TEXTURED,     // (x, y), (s, t)
    NUM_VERTEX_FORMATS,
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
    PROGRAM_BACKBUF,
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
    bool smooth;
};

struct gl_surface
{
    GLuint framebuffer;
    GLuint depth;
    int texture_id;

    GLuint ms_framebuffer;
    GLuint ms_color_attachment;

    int samples;
};

struct gl_program
{
    GLuint handle;
    GLint uniforms[UNIFORM_COUNT];
    int dirty_uniform_bits;
};

struct gl_state
{
    int program_id;
    int bound_texture_id;
    int bound_surface_id;
    tq_blend_mode blend_mode;
};

DECLARE_FLEXIBLE_ARRAY(gl_texture)
DECLARE_FLEXIBLE_ARRAY(gl_surface)

struct libtq_gl_renderer_priv
{
    int             antialiasing_level;

    int             vertex_format;
    GLuint          vao[NUM_VERTEX_FORMATS];
    GLuint          vbo[NUM_VERTEX_FORMATS];
    GLsizei         vbo_offset[NUM_VERTEX_FORMATS];
    GLsizei         vbo_size[NUM_VERTEX_FORMATS];

    GLint           max_samples;
};

//------------------------------------------------------------------------------

static struct gl_colors colors;
static struct gl_matrices matrices;
static struct gl_texture_array textures;
static struct gl_program programs[PROGRAM_COUNT];
static struct gl_state state;
static struct gl_surface_array surfaces;
static struct libtq_gl_renderer_priv priv;

//------------------------------------------------------------------------------

static void decode_color24(GLfloat *dst, tq_color color)
{
    dst[0] = color.r / 255.0f;
    dst[1] = color.g / 255.0f;
    dst[2] = color.b / 255.0f;
}

static void decode_color32(GLfloat *dst, tq_color color)
{
    dst[0] = color.r / 255.0f;
    dst[1] = color.g / 255.0f;
    dst[2] = color.b / 255.0f;
    dst[3] = color.a / 255.0f;
}

/**
 * Get OpenGL render mode.
 */
static GLenum conv_mode(int mode)
{
    switch (mode) {
    case LIBTQ_POINTS:
        return GL_POINTS;
    case LIBTQ_LINE_STRIP:
        return GL_LINE_STRIP;
    case LIBTQ_LINE_LOOP:
        return GL_LINE_LOOP;
    case LIBTQ_TRIANGLES:
        return GL_TRIANGLES;
    case LIBTQ_TRIANGLE_FAN:
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
        return GL_RED;
    case 2:
        return GL_RG;
    case 3:
        return GL_RGB;
    case 4:
        return GL_RGBA;
    }

    return GL_INVALID_ENUM;
}

static bool compare_blend_mode(tq_blend_mode const *a, tq_blend_mode const *b)
{
    return (a->color_src_factor == b->color_src_factor)
        && (a->color_dst_factor == b->color_dst_factor)
        && (a->alpha_src_factor == b->alpha_src_factor)
        && (a->alpha_dst_factor == b->alpha_dst_factor)
        && (a->color_equation == b->color_equation)
        && (a->alpha_equation == b->alpha_equation);
}

static GLenum conv_blend_factor(tq_blend_factor factor)
{
    switch (factor) {
    case TQ_BLEND_ZERO:                 return GL_ZERO;
    case TQ_BLEND_ONE:                  return GL_ONE;
    case TQ_BLEND_SRC_COLOR:            return GL_SRC_COLOR;
    case TQ_BLEND_ONE_MINUS_SRC_COLOR:  return GL_ONE_MINUS_SRC_COLOR;
    case TQ_BLEND_DST_COLOR:            return GL_DST_COLOR;
    case TQ_BLEND_ONE_MINUS_DST_COLOR:  return GL_ONE_MINUS_DST_COLOR;
    case TQ_BLEND_SRC_ALPHA:            return GL_SRC_ALPHA;
    case TQ_BLEND_ONE_MINUS_SRC_ALPHA:  return GL_ONE_MINUS_SRC_ALPHA;
    case TQ_BLEND_DST_ALPHA:            return GL_DST_ALPHA;
    case TQ_BLEND_ONE_MINUS_DST_ALPHA:  return GL_ONE_MINUS_DST_ALPHA;
    }

    return GL_INVALID_ENUM;
}

static GLenum conv_blend_equation(tq_blend_equation equation)
{
    switch (equation) {
    case TQ_BLEND_ADD:                  return GL_FUNC_ADD;
    case TQ_BLEND_SUB:                  return GL_FUNC_SUBTRACT;
    case TQ_BLEND_REV_SUB:              return GL_FUNC_REVERSE_SUBTRACT;
    }

    return GL_INVALID_ENUM;
}

static void gl_texture_dtor(struct gl_texture *texture)
{
    CHECK_GL(glDeleteTextures(1, &texture->handle));
}

static void gl_surface_dtor(struct gl_surface *surface)
{
    CHECK_GL(glDeleteFramebuffers(1, &surface->framebuffer));
    CHECK_GL(glDeleteRenderbuffers(1, &surface->depth));
    gl_texture_array_remove(&textures, surface->texture_id);

    if (surface->samples > 1) {
        CHECK_GL(glDeleteFramebuffers(1, &surface->ms_framebuffer));
        CHECK_GL(glDeleteRenderbuffers(1, &surface->ms_color_attachment));
    }
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

        libtq_log(LIBTQ_LOG_WARNING, "** Failed to compile GLSL %s shader. **\n", type_str);
        libtq_log(LIBTQ_LOG_WARNING, "%s\n", buffer);

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

        libtq_log(LIBTQ_LOG_WARNING, "** Failed to link GLSL program. **\n");
        libtq_log(LIBTQ_LOG_WARNING, "%s\n", buffer);

        return 0;
    }

    return handle;
}

static void set_vertex_pointers(int vertex_format)
{
    switch (vertex_format) {
    case VERTEX_FORMAT_SOLID:
        CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE,
            2 * sizeof(GLfloat), (void *) 0));
        break;
    case VERTEX_FORMAT_COLORED:
        CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE,
            6 * sizeof(GLfloat), (void *) 0));
        CHECK_GL(glVertexAttribPointer(ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE,
            6 * sizeof(GLfloat), (void *) (2 * sizeof(float))));
        break;
    case VERTEX_FORMAT_TEXTURED:
        CHECK_GL(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(GLfloat), (void *) 0));
        CHECK_GL(glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(GLfloat), (void *) (2 * sizeof(float))));
        break;
    }
}

static void init_vertex_formats(void)
{
    CHECK_GL(glGenVertexArrays(NUM_VERTEX_FORMATS, priv.vao));
    CHECK_GL(glGenBuffers(NUM_VERTEX_FORMATS, priv.vbo));

    CHECK_GL(glBindVertexArray(priv.vao[VERTEX_FORMAT_SOLID]));
    CHECK_GL(glEnableVertexAttribArray(ATTRIB_POSITION));
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, priv.vbo[VERTEX_FORMAT_SOLID]));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, DEFAULT_VBO_SIZE, NULL, GL_DYNAMIC_DRAW));
    set_vertex_pointers(VERTEX_FORMAT_SOLID);

    CHECK_GL(glBindVertexArray(priv.vao[VERTEX_FORMAT_COLORED]));
    CHECK_GL(glEnableVertexAttribArray(ATTRIB_POSITION));
    CHECK_GL(glEnableVertexAttribArray(ATTRIB_COLOR));
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, priv.vbo[VERTEX_FORMAT_COLORED]));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, DEFAULT_VBO_SIZE, NULL, GL_DYNAMIC_DRAW));
    set_vertex_pointers(VERTEX_FORMAT_COLORED);

    CHECK_GL(glBindVertexArray(priv.vao[VERTEX_FORMAT_TEXTURED]));
    CHECK_GL(glEnableVertexAttribArray(ATTRIB_POSITION));
    CHECK_GL(glEnableVertexAttribArray(ATTRIB_TEXCOORD));
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, priv.vbo[VERTEX_FORMAT_TEXTURED]));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, DEFAULT_VBO_SIZE, NULL, GL_DYNAMIC_DRAW));
    set_vertex_pointers(VERTEX_FORMAT_TEXTURED);

    CHECK_GL(glBindVertexArray(0));

    priv.vertex_format = -1;

    for (int i = 0; i < NUM_VERTEX_FORMATS; i++) {
        priv.vbo_offset[i] = 0;
        priv.vbo_size[i] = DEFAULT_VBO_SIZE;
    }
}

static void init_vbo(void)
{
    CHECK_GL(glGenBuffers(NUM_VERTEX_FORMATS, priv.vbo));

    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, priv.vbo[VERTEX_FORMAT_SOLID]));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, DEFAULT_VBO_SIZE, NULL, GL_DYNAMIC_DRAW));
}

static void set_vertex_format(int vertex_format)
{
    if (priv.vertex_format == vertex_format) {
        return;
    }

    CHECK_GL(glBindVertexArray(priv.vao[vertex_format]));
    priv.vertex_format = vertex_format;
}

static GLsizei append_data_to_vbo(void const *data, size_t size)
{
    int vbo = priv.vertex_format;

    GLsizei offset = priv.vbo_offset[vbo];
    GLsizei required_vbo_size = offset + size;

    if (priv.vbo_size[vbo] < required_vbo_size) {
        GLsizei next_vbo_size = priv.vbo_size[vbo];

        while (next_vbo_size < required_vbo_size) {
            next_vbo_size *= 2;
        }

        GLuint prev_vbo = priv.vbo[vbo];
        GLsizei prev_vbo_size = priv.vbo_size[vbo];

        GLuint next_vbo;
        CHECK_GL(glGenBuffers(1, &next_vbo));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, next_vbo));
        CHECK_GL(glBufferData(GL_ARRAY_BUFFER, next_vbo_size, NULL, GL_DYNAMIC_DRAW));

        CHECK_GL(glBindBuffer(GL_COPY_READ_BUFFER, prev_vbo));
        CHECK_GL(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER,
            0, 0, prev_vbo_size));
        CHECK_GL(glBindBuffer(GL_COPY_READ_BUFFER, 0));

        CHECK_GL(glDeleteBuffers(1, &prev_vbo));

        set_vertex_pointers(priv.vertex_format);

        priv.vbo[vbo] = next_vbo;
        priv.vbo_size[vbo] = next_vbo_size;
    }

    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, priv.vbo[vbo]));
    CHECK_GL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
    
    priv.vbo_offset[vbo] += size;

    return offset;
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
 * - (void) initialize;
 */
static void initialize(void)
{
    if (glewInit()) {
        libtq_error("Failed to initialize GLEW.\n");
    }

    mat4_identity(matrices.proj);
    mat4_identity(matrices.mv);

    gl_texture_array_initialize(&textures, 16, gl_texture_dtor);
    gl_surface_array_initialize(&surfaces, 8, gl_surface_dtor);

    init_vertex_formats();

    state.program_id = -1;

    GLuint vs_standard = compile_shader(GL_VERTEX_SHADER, vs_src_standard);
    GLuint vs_backbuf = compile_shader(GL_VERTEX_SHADER, vs_src_backbuf);
    GLuint fs_solid = compile_shader(GL_FRAGMENT_SHADER, fs_src_solid);
    GLuint fs_colored = compile_shader(GL_FRAGMENT_SHADER, fs_src_colored);
    GLuint fs_textured = compile_shader(GL_FRAGMENT_SHADER, fs_src_textured);
    GLuint fs_font = compile_shader(GL_FRAGMENT_SHADER, fs_src_font);

    programs[PROGRAM_SOLID].handle = link_program(vs_standard, fs_solid);
    programs[PROGRAM_COLORED].handle = link_program(vs_standard, fs_colored);
    programs[PROGRAM_TEXTURED].handle = link_program(vs_standard, fs_textured);
    programs[PROGRAM_FONT].handle = link_program(vs_standard, fs_font);
    programs[PROGRAM_BACKBUF].handle = link_program(vs_backbuf, fs_textured);

    for (int i = 0; i < PROGRAM_COUNT; i++) {
        programs[i].uniforms[UNIFORM_PROJECTION] = glGetUniformLocation(programs[i].handle, "u_projection");
        programs[i].uniforms[UNIFORM_MODELVIEW] = glGetUniformLocation(programs[i].handle, "u_modelView");
        programs[i].uniforms[UNIFORM_COLOR] = glGetUniformLocation(programs[i].handle, "u_color");
        programs[i].dirty_uniform_bits = 2147483647; // totally not a magic number
    }

    glDeleteShader(vs_standard);
    glDeleteShader(vs_backbuf);
    glDeleteShader(fs_solid);
    glDeleteShader(fs_textured);
    glDeleteShader(fs_font);

    state.bound_texture_id = -1;
    state.bound_surface_id = -1;
    state.blend_mode = TQ_DEFINE_BLEND_MODE(TQ_BLEND_SRC_ALPHA, TQ_BLEND_ONE_MINUS_SRC_ALPHA);

    /**
     * Reset OpenGL state.
     */

    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECK_GL(glBlendEquation(GL_FUNC_ADD));

    CHECK_GL(glDisable(GL_CULL_FACE));
    CHECK_GL(glDisable(GL_DEPTH_TEST));

    CHECK_GL(glEnable(GL_MULTISAMPLE));

    CHECK_GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    CHECK_GL(glGetIntegerv(GL_MAX_SAMPLES, &priv.max_samples));

    priv.antialiasing_level = 0;

    /**
     * Initialization is done.
     */

    libtq_log(0, "OpenGL renderer initialized.\n");
    libtq_log(0, "GL_VENDOR: %s\n", glGetString(GL_VENDOR));
    libtq_log(0, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));
    libtq_log(0, "GL_VERSION: %s\n", glGetString(GL_VERSION));
}

/**
 * - (void) terminate;
 */
static void terminate(void)
{
    for (int i = 0; i < PROGRAM_COUNT; i++) {
        CHECK_GL(glDeleteProgram(programs[i].handle));
    }

    gl_surface_array_terminate(&surfaces);
    gl_texture_array_terminate(&textures);
}

/**
 * - (void) process;
 */
static void process(void)
{
    CHECK_GL(glFlush());
}

static void post_process(void)
{
    for (int i = 0; i < NUM_VERTEX_FORMATS; i++) {
        priv.vbo_offset[i] = 0;
    }
}

int request_antialiasing_level(int level)
{
    if (level == 0) {
        priv.antialiasing_level = 1;
    } else if (level <= priv.max_samples) {
        priv.antialiasing_level = level;
    }

    return priv.antialiasing_level;
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

    struct gl_texture texture;
    texture.format = conv_texture_format(channels);

    CHECK_GL(glGenTextures(1, &texture.handle));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture.handle));

    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, texture.format,
        width, height, 0, texture.format,
        GL_UNSIGNED_BYTE, NULL));

    texture.width = width;
    texture.height = height;
    texture.channels = channels;
    texture.smooth = false;

    int texture_id = gl_texture_array_add(&textures, &texture);

    state.bound_texture_id = texture_id;
    return texture_id;
}

/**
 * Delete a texture
 */
static void delete_texture(int texture_id)
{
    gl_texture_array_remove(&textures, texture_id);
}

static bool is_texture_smooth(int texture_id)
{
    if (!gl_texture_array_check(&textures, texture_id)) {
        return false;
    }

    return textures.data[texture_id].smooth;
}

static void set_texture_smooth(int texture_id, bool smooth)
{
    if (!gl_texture_array_check(&textures, texture_id)) {
        return;
    }

    if (textures.data[texture_id].smooth == smooth) {
        return;
    }

    CHECK_GL(glBindTexture(GL_TEXTURE_2D, textures.data[texture_id].handle));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST));

    textures.data[texture_id].smooth = smooth;
    state.bound_texture_id = texture_id;
}

static void get_texture_size(int texture_id, int *width, int *height)
{
    if (!gl_texture_array_check(&textures, texture_id)) {
        *width = -1;
        *height = -1;
        return;
    }

    *width = textures.data[texture_id].width;
    *height = textures.data[texture_id].height;
}

static void update_texture(int texture_id, int x_offset, int y_offset, int width, int height, unsigned char *pixels)
{
    if (!gl_texture_array_check(&textures, texture_id)) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, textures.data[texture_id].handle);

    if (x_offset == 0 && y_offset == 0 && width == -1 && height == -1) {
        glTexImage2D(GL_TEXTURE_2D, 0, textures.data[texture_id].format,
            textures.data[texture_id].width, textures.data[texture_id].height, 0,
            textures.data[texture_id].format, GL_UNSIGNED_BYTE, pixels);
    } else {
        glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, width, height,
            textures.data[texture_id].format, GL_UNSIGNED_BYTE, pixels);
    }

    state.bound_texture_id = texture_id;
}

static void bind_texture(int texture_id)
{
    if (state.bound_texture_id == texture_id) {
        return;
    }

    if (!gl_texture_array_check(&textures, texture_id)) {
        CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
    } else {
        CHECK_GL(glBindTexture(GL_TEXTURE_2D, textures.data[texture_id].handle));
    }

    state.bound_texture_id = texture_id;
}

/**
 * - (int) create_surface: (int) width: (int) height;
 */
static int create_surface(int width, int height)
{
    struct gl_surface surface = {0};

    surface.samples = priv.antialiasing_level;

    CHECK_GL(glGenRenderbuffers(1, &surface.depth));
    CHECK_GL(glBindRenderbuffer(GL_RENDERBUFFER, surface.depth));
    CHECK_GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height));

    surface.texture_id = create_texture(width, height, LIBTQ_RGBA);
    set_texture_smooth(surface.texture_id, true);
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    CHECK_GL(glGenFramebuffers(1, &surface.framebuffer));
    CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, surface.framebuffer));

    CHECK_GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, surface.depth));
    CHECK_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, textures.data[surface.texture_id].handle, 0));

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        libtq_error("Failed to create surface.\n");
    }

    if (surface.samples > 1) {
        CHECK_GL(glGenRenderbuffers(1, &surface.ms_color_attachment));
        CHECK_GL(glBindRenderbuffer(GL_RENDERBUFFER, surface.ms_color_attachment));
        CHECK_GL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, surface.samples,
            GL_RGBA8, width, height));

        CHECK_GL(glGenFramebuffers(1, &surface.ms_framebuffer));
        CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, surface.ms_framebuffer));

        CHECK_GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_RENDERBUFFER, surface.ms_color_attachment));

        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (status != GL_FRAMEBUFFER_COMPLETE) {
            libtq_error("Failed to create surface.\n");
        }
    }

    int surface_id = gl_surface_array_add(&surfaces, &surface);

    state.bound_surface_id = surface_id;
    return surface_id;
}

/**
 * - (void) delete_surface: (int) surface_id;
 */
static void delete_surface(int surface_id)
{
    gl_surface_array_remove(&surfaces, surface_id);
}

/**
 * - (int) get_surface_texture_id: (int) surface_id;
 */
static int get_surface_texture_id(int surface_id)
{
    if (!gl_surface_array_check(&surfaces, surface_id)) {
        return -1;
    }

    return surfaces.data[surface_id].texture_id;
}

/**
 * - (void) bind_surface: (int) surface_id;
 */
static void bind_surface(int surface_id)
{
    int prev_surface_id = state.bound_surface_id;

    if (prev_surface_id == surface_id) {
        return;
    }

    if (prev_surface_id != -1 && (surfaces.data[prev_surface_id].samples > 1)) {
        GLuint prev_framebuffer = surfaces.data[prev_surface_id].framebuffer;
        GLuint prev_ms_framebuffer = surfaces.data[prev_surface_id].ms_framebuffer;

        CHECK_GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prev_framebuffer));
        CHECK_GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, prev_ms_framebuffer));

        int prev_width = textures.data[surfaces.data[prev_surface_id].texture_id].width;
        int prev_height = textures.data[surfaces.data[prev_surface_id].texture_id].height;

        CHECK_GL(glBlitFramebuffer(
            0, 0, prev_width, prev_height,
            0, 0, prev_width, prev_height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST
        ));
    }

    GLuint framebuffer;
    int width;
    int height;

    if (!gl_surface_array_check(&surfaces, surface_id)) {
        framebuffer = 0;
        libtq_get_display_size(&width, &height);
    } else {
        if (surfaces.data[surface_id].samples > 1) {
            framebuffer = surfaces.data[surface_id].ms_framebuffer;
        } else {
            framebuffer = surfaces.data[surface_id].framebuffer;
        }
        width = textures.data[surfaces.data[surface_id].texture_id].width;
        height = textures.data[surfaces.data[surface_id].texture_id].height;
    }

    CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
    CHECK_GL(glViewport(0, 0, width, height));

    state.bound_surface_id = surface_id;
}

static void set_clear_color(tq_color clear_color)
{
    decode_color24(colors.clear, clear_color);
    CHECK_GL(glClearColor(colors.clear[0], colors.clear[1], colors.clear[2], 1.0f));
}

static void set_draw_color(tq_color draw_color)
{
    decode_color32(colors.draw, draw_color);

    for (int i = 0; i < PROGRAM_COUNT; i++) {
        programs[i].dirty_uniform_bits |= (1 << UNIFORM_COLOR);
    }

    if (state.program_id != -1) {
        apply_uniforms();
    }
}

static void set_blend_mode(tq_blend_mode mode)
{
    if (compare_blend_mode(&state.blend_mode, &mode)) {
        return;
    }

    CHECK_GL(glBlendFuncSeparate(
        conv_blend_factor(mode.color_src_factor),
        conv_blend_factor(mode.color_dst_factor),
        conv_blend_factor(mode.alpha_src_factor),
        conv_blend_factor(mode.alpha_dst_factor)
    ));

    CHECK_GL(glBlendEquationSeparate(
        conv_blend_equation(mode.color_equation),
        conv_blend_equation(mode.alpha_equation)
    ));

    state.blend_mode = mode;
}

static void clear(void)
{
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));
}

static void draw_solid(int mode, float const *data, int num_vertices)
{
    set_vertex_format(VERTEX_FORMAT_SOLID);
    set_program_id(PROGRAM_SOLID);

    GLsizei offset = append_data_to_vbo(data, 2 * sizeof(float) * num_vertices);
    GLint start = offset / sizeof(float) / 2;

    CHECK_GL(glDrawArrays(conv_mode(mode), start, num_vertices));
}

static void draw_colored(int mode, float const *data, int num_vertices)
{
    set_vertex_format(VERTEX_FORMAT_COLORED);
    set_program_id(PROGRAM_COLORED);

    GLsizei offset = append_data_to_vbo(data, 6 * sizeof(float) * num_vertices);
    GLint start = offset / sizeof(float) / 6;

    CHECK_GL(glDrawArrays(conv_mode(mode), start, num_vertices));
}

static void draw_textured(int mode, float const *data, int num_vertices)
{
    set_vertex_format(VERTEX_FORMAT_TEXTURED);
    set_program_id(PROGRAM_TEXTURED);

    GLsizei offset = append_data_to_vbo(data, 4 * sizeof(float) * num_vertices);
    GLint start = offset / sizeof(float) / 4;

    CHECK_GL(glDrawArrays(conv_mode(mode), start, num_vertices));
}

static void draw_font(float const *data, int num_vertices)
{
    set_vertex_format(VERTEX_FORMAT_TEXTURED);
    set_program_id(PROGRAM_FONT);

    GLsizei offset = append_data_to_vbo(data, 4 * sizeof(float) * num_vertices);
    GLint start = offset / sizeof(float) / 4;

    CHECK_GL(glDrawArrays(GL_TRIANGLES, start, num_vertices));
}

static void draw_canvas(float x0, float y0, float x1, float y1)
{
    CHECK_GL(glDisable(GL_BLEND));
    CHECK_GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));

    set_vertex_format(VERTEX_FORMAT_TEXTURED);
    set_program_id(PROGRAM_BACKBUF);

    float data[] = {
        x0, y0, 0.0f, 0.0f,
        x1, y0, 1.0f, 0.0f,
        x1, y1, 1.0f, 1.0f,
        x0, y1, 0.0f, 1.0f,
    };

    GLsizei offset = append_data_to_vbo(data, 16 * sizeof(float));
    GLint start = offset / sizeof(float) / 4;

    CHECK_GL(glDrawArrays(GL_TRIANGLE_FAN, start, 4));

    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glClearColor(colors.clear[0], colors.clear[1], colors.clear[2], 1.0f));
}

//------------------------------------------------------------------------------
// Module constructor

//--------------------------------------
// Construct abstract module for OpenGL renderer.
//--------------------------------------
void libtq_construct_gl_renderer(struct libtq_renderer_impl *renderer)
{
    *renderer = (struct libtq_renderer_impl) {
        .initialize = initialize,
        .terminate = terminate,
        .process = process,
        .post_process = post_process,

        .request_antialiasing_level = request_antialiasing_level,
        
        .update_projection = update_projection,
        .update_model_view = update_model_view,

        .create_texture = create_texture,
        .delete_texture = delete_texture,
        .is_texture_smooth = is_texture_smooth,
        .set_texture_smooth = set_texture_smooth,
        .get_texture_size = get_texture_size,
        .update_texture = update_texture,
        .bind_texture = bind_texture,

        .create_surface = create_surface,
        .delete_surface = delete_surface,
        .get_surface_texture_id = get_surface_texture_id,
        .bind_surface = bind_surface,

        .set_clear_color = set_clear_color,
        .set_draw_color = set_draw_color,
        .set_blend_mode = set_blend_mode,

        .clear = clear,
        .draw_solid = draw_solid,
        .draw_colored = draw_colored,
        .draw_textured = draw_textured,
        .draw_font = draw_font,
        .draw_canvas = draw_canvas,
    };
}

//------------------------------------------------------------------------------

#endif // defined(TQ_WIN32) || defined(TQ_LINUX)

//------------------------------------------------------------------------------
