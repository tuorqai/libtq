
//------------------------------------------------------------------------------

#if defined(TQ_ANDROID)

//------------------------------------------------------------------------------

#include "tq_core.h"

//------------------------------------------------------------------------------

static void initialize(void)
{

}

static void terminate(void)
{

}

static void present(void)
{

}

static bool process_events(void)
{
    return false;
}

static void set_size(uint32_t width, uint32_t height)
{

}

static void set_title(char const *title)
{

}

static void set_key_autorepeat_enabled(bool enabled)
{

}

static void set_mouse_cursor_hidden(bool hidden)
{

}

static void show_message_box(char const *title, char const *message)
{

}

static void *get_gl_proc_addr(char const *name)
{
    return NULL;
}

static bool check_gl_ext(char const *name)
{
    return false;
}

//------------------------------------------------------------------------------

void libtq_construct_android_display(struct libtq_display_impl *display)
{
    display->initialize                 = initialize;
    display->terminate                  = terminate;
    display->present                    = present;
    display->process_events             = process_events;
    display->set_size                   = set_size;
    display->set_title                  = set_title;
    display->set_key_autorepeat_enabled = set_key_autorepeat_enabled;
    display->set_mouse_cursor_hidden    = set_mouse_cursor_hidden;
    display->show_message_box           = show_message_box;
    display->get_gl_proc_addr           = get_gl_proc_addr;
    display->check_gl_ext               = check_gl_ext;
}

//------------------------------------------------------------------------------

#endif // defined(TQ_ANDROID)

//------------------------------------------------------------------------------
