#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include <gtk/gtk.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define BUFSIZE 4096

static bool stdinHasData() {
  struct pollfd fds;
  fds.fd = STDIN_FILENO;
  fds.events = POLLIN;

  return (poll(&fds, 1, 0) > 0);
}

static void savedCb(GObject *source_object, GAsyncResult *res, gpointer data) {
  GError *err;
  GFile *file;

  err = NULL;
  file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source_object), res, &err);

  g_object_unref(source_object);

  if (err == NULL && stdinHasData()) {
    GFileOutputStream *fileStream;
    unsigned char buffer[BUFSIZE];

    fileStream = g_file_replace(file, NULL, false,
                                G_FILE_CREATE_REPLACE_DESTINATION, NULL, NULL);

    while (fread(buffer, 1, sizeof(buffer), stdin) > 0) {
      if (!g_output_stream_write_all(G_OUTPUT_STREAM(fileStream), buffer,
                                     sizeof(buffer), NULL, NULL, NULL)) {
        printf("something went wrong when writing\n");
        break;
      };
    }

    g_output_stream_close(G_OUTPUT_STREAM(fileStream), NULL, NULL);
    g_object_unref(fileStream);
  } else {
    g_object_unref(err);
  }

  printf("%s\n", g_file_get_path(file));

  g_object_unref(file);
  g_application_release(G_APPLICATION(data));
}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkFileDialog *chooser;
  GFile *initfolder;
  time_t seconds;
  struct tm *t;
  char buf[200];

  time(&seconds);
  t = localtime(&seconds);
  strftime(buf, sizeof(buf), "%e_%m_%Y-%H_%M_%S.png", t);

  initfolder = g_file_new_for_path(strcat(getenv("HOME"), "/Pictures"));

  chooser = gtk_file_dialog_new();
  gtk_file_dialog_set_title(chooser, "Save file");
  gtk_file_dialog_set_initial_name(chooser, buf);
  gtk_file_dialog_set_initial_folder(GTK_FILE_DIALOG(chooser), initfolder);
  gtk_file_dialog_save(GTK_FILE_DIALOG(chooser), NULL, NULL, savedCb, app);

  g_object_unref(initfolder);
  g_application_hold(G_APPLICATION(app));
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("com.anewdi.gtksave", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
