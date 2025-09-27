#!/usr/bin/env python

import sys
import os
import gi
import datetime

gi.require_version("Gtk", "4.0")
from gi.repository import Gtk, Gio


def save_callback(dialog, result):
    file = dialog.save_finish(result)
    if file is not None:
        data = sys.stdin.buffer.read()
        with open(file.get_path(), "wb") as image:
            image.write(data)
    Gtk.Application.release(app)


def on_activate(app):
    home = os.getenv("HOME")
    folder = Gio.File.new_for_path(home + "/Pictures")
    time = datetime.datetime.now().strftime("%e_%m_%Y-%H_%M_%S")

    f = Gtk.FileFilter.new()
    f.add_mime_type("image/*")
    chooser = Gtk.FileDialog.new()
    chooser.set_initial_folder(folder)
    chooser.set_initial_name(time + ".png")
    chooser.set_title("Save screenshot")
    chooser.set_default_filter(f)

    chooser.save(None, None, save_callback)
    Gtk.Application.hold(app)


app = Gtk.Application()
app.connect("activate", on_activate)

app.run(None)
