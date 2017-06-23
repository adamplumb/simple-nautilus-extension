#include <glib.h>
/* Nautilus extension headers */
#include <libnautilus-extension/nautilus-extension-types.h>
#include <libnautilus-extension/nautilus-file-info.h>
#include <libnautilus-extension/nautilus-menu-provider.h>

#include <string.h>
#include <time.h>

static GType provider_types[1];
static GType foo_extension_type;
static GObjectClass *parent_class;

typedef struct {
	GObject parent_slot;
} FooExtension;

typedef struct {
	GObjectClass parent_slot;
} FooExtensionClass;

/* nautilus extension interface */
void nautilus_module_initialize (GTypeModule  *module);
void nautilus_module_shutdown (void);
void nautilus_module_list_types (const GType **types, int *num_types);
GType foo_extension_get_type (void);

static void foo_extension_register_type (GTypeModule *module);

/* menu filler */
static GList * foo_extension_get_file_items (NautilusMenuProvider *provider,
                GtkWidget *window,
                GList *files);
#if 0
static GList * foo_extension_get_background_items (NautilusMenuProvider *provider,
                GtkWidget *window,
                NautilusFileInfo *current_folder);
static GList * foo_extension_get_toolbar_items (NautilusMenuProvider *provider,
                GtkWidget *window,
                NautilusFileInfo *current_folder);
#endif

/* command callback */
static gboolean bloop (NautilusMenuProvider *provider, gpointer user_data);
static gboolean show_item = FALSE;

void nautilus_module_initialize (GTypeModule  *module)
{
        foo_extension_register_type (module);

        provider_types[0] = foo_extension_get_type ();
}

void nautilus_module_shutdown (void)
{
        /* Any module-specific shutdown */
}

void nautilus_module_list_types (const GType **types,
                                 int *num_types)
{
        *types = provider_types;
        *num_types = G_N_ELEMENTS (provider_types);
}

GType foo_extension_get_type (void)
{
        return foo_extension_type;
}

static void foo_extension_instance_init (FooExtension *object)
{
}

static void foo_extension_class_init(FooExtensionClass *class)
{
	parent_class = g_type_class_peek_parent (class);
}

static void foo_extension_menu_provider_iface_init(
		NautilusMenuProviderIface *iface)
{
	iface->get_file_items = foo_extension_get_file_items;
}

static void foo_extension_register_type (GTypeModule *module)
{
        static const GTypeInfo info = {
                sizeof (FooExtensionClass),
                (GBaseInitFunc) NULL,
                (GBaseFinalizeFunc) NULL,
                (GClassInitFunc) foo_extension_class_init,
                NULL,
                NULL,
                sizeof (FooExtension),
                0,
                (GInstanceInitFunc) foo_extension_instance_init,
        };

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) foo_extension_menu_provider_iface_init,
		NULL,
		NULL
	};

        foo_extension_type = g_type_module_register_type (module,
                             G_TYPE_OBJECT,
                             "FooExtension",
                             &info, 0);

	g_type_module_add_interface (module,
				     foo_extension_type,
				     NAUTILUS_TYPE_MENU_PROVIDER,
				     &menu_provider_iface_info);
}


static GList * foo_extension_get_file_items (NautilusMenuProvider *provider,
                GtkWidget *window,
                GList *files)
{
        GList *ret = NULL; 

        g_print("get_file_items()\n");

        if (show_item) {
            g_print("Returning full list\n");
            NautilusMenuItem *item = nautilus_menu_item_new ("FooExtension::sometimes",
                                   "Sometimes here",
                                   "Do important foo-related stuff to the selected files",
                                   NULL /* icon name */);
            ret = g_list_append (NULL, item);
        } else {
            if (g_list_length(files) > 0) {
                g_print("Adding timeout\n");
                g_timeout_add_seconds(1, (GSourceFunc) bloop, provider);                
            }
        }

        NautilusMenuItem *item = nautilus_menu_item_new ("FooExtension::synced",
                                   "Always here",
                                   "Nothing",
                                   NULL /* icon name */);
        ret = g_list_append(ret, item);        
        return ret;
}

static gboolean bloop (NautilusMenuProvider *provider, gpointer user_data) {
    g_print("Toggling the Sometimes here item\n");

    if (!show_item) {
        show_item = TRUE;
    }

    nautilus_menu_provider_emit_items_updated_signal(provider);

    return FALSE;
}