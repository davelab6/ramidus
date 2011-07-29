GST_FLAGS = `pkg-config --cflags --libs gstreamer-0.10`
GST_BASE_FLAGS = `pkg-config --cflags --libs gstreamer-base-0.10`
CLUTTER_FLAGS = `pkg-config --cflags --libs clutter-1.0`
CLUTTER_GST_FLAGS = `pkg-config --cflags --libs clutter-gst-1.0`
CC = gcc

all: ramidus

ramidus: ramidus.c shell-recorder-src.c shell-recorder.c
	$(CC) ramidus.c shell-recorder-src.c shell-recorder.c $(GST_FLAGS) $(GST_BASE_FLAGS) $(CLUTTER_FLAGS) $(CLUTTER_GST_FLAGS) -o ramidus

clean:
	rm ramidus

