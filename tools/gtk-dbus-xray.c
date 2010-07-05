#include <gtk/gtk.h>
#include <include/rbus-proxy-call.h>


static GtkTreeModel *create_model ()
{
  GtkListStore  *store;
  GtkTreeIter    iter;
  
  store = gtk_list_store_new (2 , G_TYPE_STRING, G_TYPE_UINT);

  /* Append a row and fill in some data */
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
                      0, "Heinz El-Mann",
                      1, 51,
                      -1);
  
  /* append another row and fill in some data */
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
                      0, "Jane Doe",
                      1, 23,
                      -1);
  
  /* ... and a third row */
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
                      0, "Joe Bungop",
                      1, 91,
                      -1);
  
  return GTK_TREE_MODEL (store);
}



static GtkWidget *create_tree()
{
  GtkCellRenderer     *renderer;
  GtkTreeModel        *model;
  GtkWidget           *view;

  view = gtk_tree_view_new ();
  renderer = gtk_cell_renderer_text_new ();

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Name",  
                                               renderer,
                                               "text", 0 ,
                                               NULL);

  /* --- Column #2 --- */

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Object",  
                                               renderer,
                                               "text", 1,
                                               NULL);

  model = create_model ();

  gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

  g_object_unref (model);

  return view;
}




int main( int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	//applet_widget_init(PACKAGE, VERSION, argc, argv, NULL, 0, NULL); 

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *view = create_tree();
//	GtkWidget* applet = applet_widget_new(PACKAGE); 

	gtk_container_add (GTK_CONTAINER (window), view);
	//applet_widget_add(APPLET_WIDGET(applet), controls);
	//gtk_widget_show(applet);
	gtk_widget_show_all(window);

	//applet_widget_gtk_main(); 
	gtk_main();


	return 0;
}

