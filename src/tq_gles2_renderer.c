
//------------------------------------------------------------------------------

#if defined(TQ_ANDROID) || defined(TQ_USE_GLES2)

//------------------------------------------------------------------------------

#include <string.h>

#include <GLES2/gl2.h>

#include "tq_core.h"
#include "tq_graphics.h"
#include "tq_handle_list.h"
#include "tq_log.h"
#include "tq_math.h"
#include "tq_mem.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// OpenGL ES debug stuff

#if defined(NDEBUG)

#define CHECK_GL(call_) call_

#else

static char const *get_gles2_error_str(GLenum error)
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

static void check_gles2_errors(char const *call, char const *file, unsigned int line)
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        libtq_log(LIBTQ_ERROR, "OpenGL error(s) occured in %s at line %d:\n", file, line);
        libtq_log(LIBTQ_ERROR, "-- %d: %s\n", line, call);
    }

    while (error != GL_NO_ERROR) {
        libtq_log(LIBTQ_ERROR, ":: [0x%04x] %s\n", error, get_gles2_error_str(error));
        error = glGetError();
    }
}

#define CHECK_GLES2(call_) \
    do { \
        call_; \
        check_gles2_errors(#call_, __FILE__, __LINE__); \
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
    "precision mediump float;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "    gl_FragColor = u_color;\n"
    "}\n";

/**
 * Colored mesh fragment shader source code.
 */
static char const *fs_src_colored =
    "precision mediump float;\n"
    "varying vec4 v_color;\n"
    "void main() {\n"
    "    gl_FragColor = v_color;\n"
    "}\n";

/**
 * Textured mesh fragment shader source code.
 */
static char const *fs_src_textured =
    "precision mediump float;\n"
    "varying vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "    gl_FragColor = texture2D(u_texture, v_texCoord);\n"
    "}\n";

/**
 * Font mesh fragment shader source code.
 */
static char const *fs_src_font =
    "precision mediump float;\n"
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

struct gles2_texture
{
    GLuint handle;
    GLsizei width;
    GLsizei height;
    GLenum format;
    int channels;
    bool smooth;
};

DECLARE_FLEXIBLE_ARRAY(gles2_texture)

struct gles2_surface
{
    GLuint framebuffer;
    GLuint depth;
    int texture_id;
};

DECLARE_FLEXIBLE_ARRAY(gles2_surface)

struct gles2_program
{
    GLuint handle;
    GLint uniforms[UNIFORM_COUNT];
    int dirty_uniform_bits;
};

struct libtq_gles2_renderer_priv
{
    int vertex_format;
    int program_id;
    int texture_id;
    int surface_id;

    tq_blend_mode blend_mode;

    GLfloat clear_color[4];
    GLfloat draw_color[4];

    float projection[16];
    float model_view[16];

    struct gles2_texture_array textures;
    struct gles2_surface_array surfaces;

    struct gles2_program programs[PROGRAM_COUNT];
};

//------------------------------------------------------------------------------

static struct libtq_gles2_renderer_priv priv;

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
 * Get OpenGL ES render mode.
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
 * Get OpenGL ES texture format.
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
 * Blend mode comparator.
 */
static bool compare_blend_mode(tq_blend_mode const *a, tq_blend_mode const *b)
{
    return (a->color_src_factor == b->color_src_factor)
        && (a->color_dst_factor == b->color_dst_factor)
        && (a->alpha_src_factor == b->alpha_src_factor)
        && (a->alpha_dst_factor == b->alpha_dst_factor)
        && (a->color_equation == b->color_equation)
        && (a->alpha_equation == b->alpha_equation);
}

/**
 * Blend factor constant converter.
 */
static GLenum conv_blend_factor(tq_blend_factor factor)
{
    switch (factor) {
    case TQ_BLEND_ZERO:
        return GL_ZERO;
    case TQ_BLEND_ONE:
        return GL_ONE;
    case TQ_BLEND_SRC_COLOR:
        return GL_SRC_COLOR;
    case TQ_BLEND_ONE_MINUS_SRC_COLOR:
        return GL_ONE_MINUS_SRC_COLOR;
    case TQ_BLEND_DST_COLOR:
        return GL_DST_COLOR;
    case TQ_BLEND_ONE_MINUS_DST_COLOR:
        return GL_ONE_MINUS_DST_COLOR;
    case TQ_BLEND_SRC_ALPHA:
        return GL_SRC_ALPHA;
    case TQ_BLEND_ONE_MINUS_SRC_ALPHA:
        return GL_ONE_MINUS_SRC_ALPHA;
    case TQ_BLEND_DST_ALPHA:
        return GL_DST_ALPHA;
    case TQ_BLEND_ONE_MINUS_DST_ALPHA:
        return GL_ONE_MINUS_DST_ALPHA;
    }

    return GL_INVALID_ENUM;
}

/**
 * Blend equation constant converter.
 */
static GLenum conv_blend_equation(tq_blend_equation equation)
{
    switch (equation) {
    case TQ_BLEND_ADD:
        return GL_FUNC_ADD;
    case TQ_BLEND_SUB:
        return GL_FUNC_SUBTRACT;
    case TQ_BLEND_REV_SUB:
        return GL_FUNC_REVERSE_SUBTRACT;
    }

    return GL_INVALID_ENUM;
}

/**
 * Texture array item destructor.
 */
static void gles2_texture_dtor(struct gles2_texture *texture)
{
    CHECK_GLES2(glDeleteTextures(1, &texture->handle));
}

/**
 * Surface array item destructor.
 */
static void gles2_surface_dtor(struct gles2_surface *surface)
{
    CHECK_GLES2(glDeleteFramebuffers(1, &surface->framebuffer));
    CHECK_GLES2(glDeleteRenderbuffers(1, &surface->depth));
    gles2_texture_array_remove(&priv.textures, surface->texture_id);
}

/**
 * Compile GLSL shader.
 */
static GLuint compile_shader(GLenum type, char const *source)
{
    GLuint handle = glCreateShader(type);
    CHECK_GLES2(glShaderSource(handle, 1, &source, NULL));
    CHECK_GLES2(glCompileShader(handle));

    GLint success;
    CHECK_GLES2(glGetShaderiv(handle, GL_COMPILE_STATUS, &success));

    if (!success) {
        GLchar buffer[1024];
        CHECK_GLES2(glGetShaderInfoLog(handle, sizeof(buffer), NULL, buffer));

        char const *type_str;

        if (type == GL_VERTEX_SHADER) {
            type_str = "vertex";
        } else if (type == GL_FRAGMENT_SHADER) {
            type_str = "fragment";
        } else {
            type_str = "unknown";
        }

        libtq_error("Failed to compile GLSL %s shader:\n%s.\n", type_str, buffer);

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

    CHECK_GLES2(glAttachShader(handle, vs));
    CHECK_GLES2(glAttachShader(handle, fs));

    CHECK_GLES2(glBindAttribLocation(handle, ATTRIB_POSITION, "a_position"));
    CHECK_GLES2(glBindAttribLocation(handle, ATTRIB_COLOR, "a_color"));
    CHECK_GLES2(glBindAttribLocation(handle, ATTRIB_TEXCOORD, "a_texCoord"));

    CHECK_GLES2(glLinkProgram(handle));

    GLint success;
    CHECK_GLES2(glGetProgramiv(handle, GL_LINK_STATUS, &success));

    if (!success) {
        GLchar buffer[1024];
        CHECK_GLES2(glGetProgramInfoLog(handle, sizeof(buffer), NULL, buffer));

        libtq_log(LIBTQ_WARNING, "** Failed to link GLSL program. **\n");
        libtq_log(LIBTQ_WARNING, "%s\n", buffer);

        return 0;
    }

    return handle;
}

/**
 * Set vertex pointers according to the vertex format.
 */
static void set_vertex_format(int vertex_format)
{
    if (priv.vertex_format == vertex_format) {
        return;
    }

    priv.vertex_format = vertex_format;

    switch (vertex_format) {
    case VERTEX_FORMAT_SOLID:
        CHECK_GLES2(glEnableVertexAttribArray(ATTRIB_POSITION));
        CHECK_GLES2(glDisableVertexAttribArray(ATTRIB_COLOR));
        CHECK_GLES2(glDisableVertexAttribArray(ATTRIB_TEXCOORD));
        break;
    case VERTEX_FORMAT_COLORED:
        CHECK_GLES2(glEnableVertexAttribArray(ATTRIB_POSITION));
        CHECK_GLES2(glEnableVertexAttribArray(ATTRIB_COLOR));
        CHECK_GLES2(glDisableVertexAttribArray(ATTRIB_TEXCOORD));
        break;
    case VERTEX_FORMAT_TEXTURED:
        CHECK_GLES2(glEnableVertexAttribArray(ATTRIB_POSITION));
        CHECK_GLES2(glDisableVertexAttribArray(ATTRIB_COLOR));
        CHECK_GLES2(glEnableVertexAttribArray(ATTRIB_TEXCOORD));
        break;
    }
}

/**
 * Set vertex pointers according to the current vertex format.
 */
static void set_vertex_pointers(float const *data)
{
    switch (priv.vertex_format) {
    case VERTEX_FORMAT_SOLID:
        CHECK_GLES2(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE,
            2 * sizeof(GLfloat), data));
        break;
    case VERTEX_FORMAT_COLORED:
        CHECK_GLES2(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE,
            6 * sizeof(GLfloat), data));
        CHECK_GLES2(glVertexAttribPointer(ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE,
            6 * sizeof(GLfloat), data + 2));
        break;
    case VERTEX_FORMAT_TEXTURED:
        CHECK_GLES2(glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(GLfloat), data));
        CHECK_GLES2(glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(GLfloat), data + 2));
        break;
    }
}

/**
 * Updates all uniforms for the current shader if needed.
 */
static void apply_uniforms(void)
{
    struct gles2_program *program = &priv.programs[priv.program_id];

    long bits = program->dirty_uniform_bits;
    GLint *location = program->uniforms;

    if (bits & (1 << UNIFORM_PROJECTION)) {
        CHECK_GLES2(glUniformMatrix4fv(location[UNIFORM_PROJECTION], 1, GL_TRUE, priv.projection));
    }

    if (bits & (1 << UNIFORM_MODELVIEW)) {
        CHECK_GLES2(glUniformMatrix4fv(location[UNIFORM_MODELVIEW], 1, GL_TRUE, priv.model_view));
    }

    if (bits & (1 << UNIFORM_COLOR)) {
        CHECK_GLES2(glUniform4fv(location[UNIFORM_COLOR], 1, priv.draw_color));
    }

    program->dirty_uniform_bits = 0;
}

/**
 * Switch to different shader.
 */
static void set_program_id(int program_id)
{
    if (priv.program_id == program_id) {
        return;
    }

    priv.program_id = program_id;

    if (program_id == -1) {
        CHECK_GLES2(glUseProgram(0));
        return;
    }

    CHECK_GLES2(glUseProgram(priv.programs[program_id].handle));

    if (priv.programs[program_id].dirty_uniform_bits) {
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
    priv.programs[program_id].dirty_uniform_bits |= (1 << uniform_id);

    if (priv.program_id == program_id) {
        apply_uniforms();
    }
}

//------------------------------------------------------------------------------

static void initialize(void)
{
    mat4_identity(priv.projection);
    mat4_identity(priv.model_view);

    gles2_texture_array_initialize(&priv.textures, 16, gles2_texture_dtor);
    gles2_surface_array_initialize(&priv.surfaces, 8, gles2_surface_dtor);

    priv.vertex_format = -1;
    priv.program_id = -1;

    GLuint vs_standard = compile_shader(GL_VERTEX_SHADER, vs_src_standard);
    GLuint vs_backbuf = compile_shader(GL_VERTEX_SHADER, vs_src_backbuf);
    GLuint fs_solid = compile_shader(GL_FRAGMENT_SHADER, fs_src_solid);
    GLuint fs_colored = compile_shader(GL_FRAGMENT_SHADER, fs_src_colored);
    GLuint fs_textured = compile_shader(GL_FRAGMENT_SHADER, fs_src_textured);
    GLuint fs_font = compile_shader(GL_FRAGMENT_SHADER, fs_src_font);

    priv.programs[PROGRAM_SOLID].handle = link_program(vs_standard, fs_solid);
    priv.programs[PROGRAM_COLORED].handle = link_program(vs_standard, fs_colored);
    priv.programs[PROGRAM_TEXTURED].handle = link_program(vs_standard, fs_textured);
    priv.programs[PROGRAM_FONT].handle = link_program(vs_standard, fs_font);
    priv.programs[PROGRAM_BACKBUF].handle = link_program(vs_backbuf, fs_textured);

    for (int i = 0; i < PROGRAM_COUNT; i++) {
        struct gles2_program *p = &priv.programs[i];

        p->uniforms[UNIFORM_PROJECTION] = glGetUniformLocation(p->handle, "u_projection");
        p->uniforms[UNIFORM_MODELVIEW] = glGetUniformLocation(p->handle, "u_modelView");
        p->uniforms[UNIFORM_COLOR] = glGetUniformLocation(p->handle, "u_color");
        p->dirty_uniform_bits = 2147483647; // totally not a magic number
    }

    glDeleteShader(vs_standard);
    glDeleteShader(vs_backbuf);
    glDeleteShader(fs_solid);
    glDeleteShader(fs_textured);
    glDeleteShader(fs_font);

    priv.texture_id = -1;
    priv.surface_id = -1;
    priv.blend_mode = TQ_DEFINE_BLEND_MODE(TQ_BLEND_SRC_ALPHA, TQ_BLEND_ONE_MINUS_SRC_ALPHA);

    /**
     * Reset OpenGL state.
     */

    CHECK_GLES2(glEnable(GL_BLEND));
    CHECK_GLES2(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECK_GLES2(glBlendEquation(GL_FUNC_ADD));

    CHECK_GLES2(glDisable(GL_CULL_FACE));
    CHECK_GLES2(glDisable(GL_DEPTH_TEST));

    CHECK_GLES2(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    /**
     * Initialization is done.
     */

    libtq_log(0, "OpenGL ES renderer initialized.\n");
    libtq_log(0, "GL_VENDOR: %s\n", glGetString(GL_VENDOR));
    libtq_log(0, "GL_RENDERER: %s\n", glGetString(GL_RENDERER));
    libtq_log(0, "GL_VERSION: %s\n", glGetString(GL_VERSION));
}

static void terminate(void)
{
    for (int i = 0; i < PROGRAM_COUNT; i++) {
        CHECK_GLES2(glDeleteProgram(priv.programs[i].handle));
    }

    gles2_surface_array_terminate(&priv.surfaces);
    gles2_texture_array_terminate(&priv.textures);
}

static void process(void)
{
    CHECK_GLES2(glFlush());
}

static void post_process(void)
{
}

static int request_antialiasing_level(int level)
{
    return 0;
}

static void update_projection(float const *mat4)
{
    mat4_copy(priv.projection, mat4);

    for (int i = 0; i < PROGRAM_COUNT; i++) {
        set_dirty_uniform(i, UNIFORM_PROJECTION);
    }
}

static void update_model_view(float const *mat3)
{
    // (Note: the "view" matrix corresponds to the projection matrix,
    //  and the "transform" one roughly corresponds to the model-view matrix
    //  of classic OpenGL).
    // (Note 2: OpenGL's own matrices are not used here, since I handle
    //  this in the "tq::graphics" module independently of renderer).

    mat4_expand(priv.model_view, mat3);

    for (int i = 0; i < PROGRAM_COUNT; i++) {
        set_dirty_uniform(i, UNIFORM_MODELVIEW);
    }
}

static int create_texture(int width, int height, int channels)
{
    if ((width < 0) || (height < 0)) {
        return -1;
    }

    if ((channels < 1) || (channels > 4)) {
        return -1;
    }

    struct gles2_texture texture;
    texture.format = conv_texture_format(channels);

    if (texture.format == GL_INVALID_ENUM) {
        return -1;
    }

    CHECK_GLES2(glGenTextures(1, &texture.handle));
    CHECK_GLES2(glBindTexture(GL_TEXTURE_2D, texture.handle));

    CHECK_GLES2(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CHECK_GLES2(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    CHECK_GLES2(glTexImage2D(GL_TEXTURE_2D, 0, texture.format,
        width, height, 0, texture.format,
        GL_UNSIGNED_BYTE, NULL));

    texture.width = width;
    texture.height = height;
    texture.channels = channels;
    texture.smooth = false;

    int texture_id = gles2_texture_array_add(&priv.textures, &texture);
    priv.texture_id = texture_id;

    return texture_id;
}

static void delete_texture(int texture_id)
{
    gles2_texture_array_remove(&priv.textures, texture_id);
}

static bool is_texture_smooth(int texture_id)
{
    if (!gles2_texture_array_check(&priv.textures, texture_id)) {
        return false;
    }

    return priv.textures.data[texture_id].smooth;
}

static void set_texture_smooth(int texture_id, bool smooth)
{
    if (!gles2_texture_array_check(&priv.textures, texture_id)) {
        return;
    }

    if (priv.textures.data[texture_id].smooth == smooth) {
        return;
    }

    CHECK_GLES2(glBindTexture(GL_TEXTURE_2D, priv.textures.data[texture_id].handle));
    CHECK_GLES2(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST));
    CHECK_GLES2(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST));

    priv.textures.data[texture_id].smooth = smooth;
    priv.texture_id = texture_id;
}

static void get_texture_size(int texture_id, int *width, int *height)
{
    if (!gles2_texture_array_check(&priv.textures, texture_id)) {
        *width = -1;
        *height = -1;
        return;
    }

    *width = priv.textures.data[texture_id].width;
    *height = priv.textures.data[texture_id].height;
}

static void update_texture(int texture_id, int x_offset, int y_offset, int width, int height, unsigned char *pixels)
{
    if (!gles2_texture_array_check(&priv.textures, texture_id)) {
        return;
    }

    struct gles2_texture *texture = &priv.textures.data[texture_id];

    CHECK_GLES2(glBindTexture(GL_TEXTURE_2D, texture->handle));

    if (x_offset == 0 && y_offset == 0 && width == -1 && height == -1) {
        CHECK_GLES2(glTexImage2D(GL_TEXTURE_2D, 0, texture->format,
            texture->width, texture->height, 0,
            texture->format, GL_UNSIGNED_BYTE, pixels));
    } else {
        CHECK_GLES2(glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, width, height,
            texture->format, GL_UNSIGNED_BYTE, pixels));
    }

    priv.texture_id = texture_id;
}

static void bind_texture(int texture_id)
{
    if (priv.texture_id == texture_id) {
        return;
    }

    if (!gles2_texture_array_check(&priv.textures, texture_id)) {
        CHECK_GLES2(glBindTexture(GL_TEXTURE_2D, 0));
    } else {
        CHECK_GLES2(glBindTexture(GL_TEXTURE_2D, priv.textures.data[texture_id].handle));
    }

    priv.texture_id = texture_id;
}

static int create_surface(int width, int height)
{
    struct gles2_surface surface = {0};

    CHECK_GLES2(glGenRenderbuffers(1, &surface.depth));
    CHECK_GLES2(glBindRenderbuffer(GL_RENDERBUFFER, surface.depth));
    CHECK_GLES2(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height));

    surface.texture_id = create_texture(width, height, LIBTQ_RGBA);
    set_texture_smooth(surface.texture_id, true);
    CHECK_GLES2(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CHECK_GLES2(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    CHECK_GLES2(glGenFramebuffers(1, &surface.framebuffer));
    CHECK_GLES2(glBindFramebuffer(GL_FRAMEBUFFER, surface.framebuffer));

    CHECK_GLES2(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, surface.depth));
    CHECK_GLES2(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, priv.textures.data[surface.texture_id].handle, 0));

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        libtq_error("Failed to create surface.\n");
    }

    int surface_id = gles2_surface_array_add(&priv.surfaces, &surface);

    priv.surface_id = surface_id;
    return surface_id;
}

static void delete_surface(int surface_id)
{
    gles2_surface_array_remove(&priv.surfaces, surface_id);
}

static int get_surface_texture_id(int surface_id)
{
    if (!gles2_surface_array_check(&priv.surfaces, surface_id)) {
        return -1;
    }

    return priv.surfaces.data[surface_id].texture_id;
}

static void bind_surface(int surface_id)
{
    int prev_surface_id = priv.surface_id;

    if (prev_surface_id == surface_id) {
        return;
    }

    struct gles2_surface *surface = &priv.surfaces.data[surface_id];

    GLuint framebuffer;
    int width;
    int height;

    if (!gles2_surface_array_check(&priv.surfaces, surface_id)) {
        framebuffer = 0;
        libtq_get_display_size(&width, &height);
    } else {
        framebuffer = surface->framebuffer;
        width = priv.textures.data[surface->texture_id].width;
        height = priv.textures.data[surface->texture_id].height;
    }

    CHECK_GLES2(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
    CHECK_GLES2(glViewport(0, 0, width, height));

    priv.surface_id = surface_id;
}

static void set_clear_color(tq_color color)
{
    decode_color24(priv.clear_color, color);

    CHECK_GLES2(glClearColor(
        priv.clear_color[0],
        priv.clear_color[1],
        priv.clear_color[2],
        1.0f
    ));
}

static void set_draw_color(tq_color color)
{
    decode_color32(priv.draw_color, color);

    for (int i = 0; i < PROGRAM_COUNT; i++) {
        priv.programs[i].dirty_uniform_bits |= (1 << UNIFORM_COLOR);
    }

    if (priv.program_id != -1) {
        apply_uniforms();
    }
}

static void set_blend_mode(tq_blend_mode mode)
{
    if (compare_blend_mode(&priv.blend_mode, &mode)) {
        return;
    }

    CHECK_GLES2(glBlendFuncSeparate(
        conv_blend_factor(mode.color_src_factor),
        conv_blend_factor(mode.color_dst_factor),
        conv_blend_factor(mode.alpha_src_factor),
        conv_blend_factor(mode.alpha_dst_factor)
    ));

    CHECK_GLES2(glBlendEquationSeparate(
        conv_blend_equation(mode.color_equation),
        conv_blend_equation(mode.alpha_equation)
    ));

    priv.blend_mode = mode;
}

static void clear(void)
{
    CHECK_GLES2(glClear(GL_COLOR_BUFFER_BIT));
}

static void draw_solid(int mode, float const *data, int num_vertices)
{
    set_vertex_format(VERTEX_FORMAT_SOLID);
    set_vertex_pointers(data);
    set_program_id(PROGRAM_SOLID);

    CHECK_GLES2(glDrawArrays(conv_mode(mode), 0, num_vertices));
}

static void draw_colored(int mode, float const *data, int num_vertices)
{
    set_vertex_format(VERTEX_FORMAT_COLORED);
    set_vertex_pointers(data);
    set_program_id(PROGRAM_COLORED);

    CHECK_GLES2(glDrawArrays(conv_mode(mode), 0, num_vertices));
}

static void draw_textured(int mode, float const *data, int num_vertices)
{
    set_vertex_format(VERTEX_FORMAT_TEXTURED);
    set_vertex_pointers(data);
    set_program_id(PROGRAM_TEXTURED);

    CHECK_GLES2(glDrawArrays(conv_mode(mode), 0, num_vertices));
}

static void draw_font(float const *data, int num_vertices)
{
    set_vertex_format(VERTEX_FORMAT_TEXTURED);
    set_vertex_pointers(data);
    set_program_id(PROGRAM_FONT);

    CHECK_GLES2(glDrawArrays(GL_TRIANGLES, 0, num_vertices));
}

static void draw_canvas(float x0, float y0, float x1, float y1)
{
    CHECK_GLES2(glDisable(GL_BLEND));

    CHECK_GLES2(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    CHECK_GLES2(glClear(GL_COLOR_BUFFER_BIT));

    set_vertex_format(VERTEX_FORMAT_TEXTURED);
    set_program_id(PROGRAM_BACKBUF);

    float data[] = {
        x0, y0, 0.0f, 0.0f,
        x1, y0, 1.0f, 0.0f,
        x1, y1, 1.0f, 1.0f,
        x0, y1, 0.0f, 1.0f,
    };

    set_vertex_pointers(data);

    CHECK_GLES2(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    CHECK_GLES2(glEnable(GL_BLEND));
    CHECK_GLES2(glClearColor(
        priv.clear_color[0],
        priv.clear_color[1],
        priv.clear_color[2],
        1.0f
    ));
}

//------------------------------------------------------------------------------
// Module constructor

void libtq_construct_gles2_renderer(struct libtq_renderer_impl *renderer)
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

#endif // defined(TQ_ANDROID) || defined(TQ_USE_GLES2)

//------------------------------------------------------------------------------
