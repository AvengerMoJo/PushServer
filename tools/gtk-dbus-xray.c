#include <gtk/gtk.h>
#include <include/rbus-get-service-names.h>

enum { 
  INDEX, 
  SERVICE_NAME,
  NUM_SERVICES
};

static GtkTreeModel *create_model ()
{
	int i;
	GtkListStore  *store;
	GtkTreeIter    iter;
	service_list_type *system_list = get_system_service_names();
	service_list_type *session_list = get_session_service_names();

	store = gtk_list_store_new (NUM_SERVICES, 
					G_TYPE_UINT,
					G_TYPE_STRING );

	for( i=0; i < system_list->service_count ; i++ ) {
		gtk_list_store_append (store, &iter);
		gtk_list_store_set(store, &iter,
				INDEX, i+1,
				SERVICE_NAME, system_list->service_name[i],
				-1 );
	}
	return GTK_TREE_MODEL (store);
}


int main( int argc, char *argv[])
{
  gtk_init(&argc, &argv);

  //applet_widget_init(PACKAGE, VERSION, argc, argv, NULL, 0, NULL);
  GError **error;
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  //GtkWidget *view = create_tree();
  GtkWidget *vbox = gtk_vbox_new(FALSE, 8);
  //GtkWidget *iconbox = gtk_hbox_new(FALSE, 8);
  GtkWidget *iconbox = gtk_toolbar_new ();

  GdkPixbuf *session_pixbuf;
  GtkToolItem *session_icon;
  GdkPixbuf *system_pixbuf;
  GtkToolItem *system_icon;

  GtkWidget *service_name_list = gtk_scrolled_window_new(NULL, NULL);
  GtkTreeModel *model;
  GtkWidget *treeview;

  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  gtk_window_set_title(GTK_WINDOW(window), "DBus-Xray" );

  g_signal_connect (window, "destroy", G_CALLBACK (gtk_widget_destroyed), &window);
  gtk_container_set_border_width (GTK_CONTAINER (window), 8);

  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new( "System and Session Service Name List") , FALSE, FALSE, 0);

  session_icon = gtk_tool_button_new( NULL, "Session List" ); 
  gtk_tool_button_set_icon_name( GTK_TOOL_BUTTON( session_icon ), "config-users" ); 

  system_icon = gtk_tool_button_new( NULL, "System List" ); 
  gtk_tool_button_set_icon_name( GTK_TOOL_BUTTON( session_icon ), "ksysguard" ); 

  gtk_toolbar_insert (GTK_TOOLBAR (iconbox), system_icon , -1);
  gtk_toolbar_insert (GTK_TOOLBAR (iconbox), session_icon , -1);

  gtk_box_pack_start(GTK_BOX(vbox), iconbox , FALSE, FALSE, 0);

  //gtk_box_pack_start(GTK_BOX(iconbox), gtk_icon_new("system"), FALSE, FALSE, 0);
  //gtk_box_pack_start(GTK_BOX(iconbox), gtk_icon_new("session"), FALSE, FALSE, 0);
  // need to update to non-fix path search
  //session_pixbuf = gdk_pixbuf_new_from_file ("/usr/share/icons/Tango/scalable/apps/system-users.svg", error);
  //system_pixbuf = gdk_pixbuf_new_from_file ("/usr/share/icons/Tango/scalable/apps/ksysguard.svg", error);

  //gtk_box_pack_start(GTK_BOX(vbox), service_name_list, FALSE, FALSE, 0);

  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (service_name_list), GTK_SHADOW_ETCHED_IN);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (service_name_list), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  gtk_box_pack_start (GTK_BOX (vbox), service_name_list , TRUE, TRUE, 0);

  /* create tree model */
  model = create_model ();

  /* create tree view */
  treeview = gtk_tree_view_new_with_model (model);

  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);
  //gtk_tree_view_set_search_column (GTK_TREE_VIEW (treeview), COLUMN_DESCRIPTION);

//  g_object_unref (model);
  gtk_container_add (GTK_CONTAINER (service_name_list), treeview);

  /* column for fixed toggles */
  renderer = gtk_cell_renderer_text_new ();
  //g_signal_connect (renderer, "toggled", G_CALLBACK (fixed_toggled), model);

  column = gtk_tree_view_column_new_with_attributes ("Index",
						     renderer,
						     "text", INDEX,
						     NULL);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
				   GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 100);
  gtk_tree_view_append_column (treeview, column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Service Name",
						     renderer,
						     "text",
						     SERVICE_NAME,
						     NULL);
  gtk_tree_view_column_set_sort_column_id (column, SERVICE_NAME );
  gtk_tree_view_append_column (treeview, column);





  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}

