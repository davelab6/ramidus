#include <stdlib.h>
#include <clutter/clutter.h>
#include <clutter-gst/clutter-gst.h>
#include <gst/gst.h>
#include "shell-recorder.h"

static ShellRecorder *recorder;

static gboolean
stop_recording_timeout (gpointer data)
{
  printf("Done recording!\n\n");
  shell_recorder_close (recorder);
  return FALSE;
}

ClutterActor *stage = NULL;
ClutterActor* thescreen = NULL;
ClutterActor* thewebcam = NULL;
ClutterActor* screen_tex = NULL;
ClutterActor* webcam_tex = NULL;

GstElement *desktop_pipeline = NULL;
GstElement *webcam_pipeline = NULL;

#define capture_width 1280
#define capture_height 800
#define webcam_width 200
#define webcam_height 150

ClutterActor* new_screen_actor() {

  screen_tex = clutter_texture_new();
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), screen_tex);
  clutter_actor_set_position(screen_tex, 0, 0);
  clutter_actor_set_size(screen_tex, capture_width, capture_height);

  GstElement* videosink = clutter_gst_video_sink_new ((ClutterTexture *) screen_tex);
  //g_object_set(G_OBJECT(videosink), "use-shaders", FALSE, NULL);
  GstElement* imagesrc = gst_element_factory_make ("ximagesrc", NULL);
  GstElement* ffmpegcolor = gst_element_factory_make ("ffmpegcolorspace", NULL);

  g_object_set(G_OBJECT(imagesrc), "display-name", ":0.0", NULL);
  g_object_set(G_OBJECT(imagesrc), "startx", 0, NULL);
  g_object_set(G_OBJECT(imagesrc), "starty", 0, NULL);
  g_object_set(G_OBJECT(imagesrc), "endx", capture_width, NULL);
  g_object_set(G_OBJECT(imagesrc), "endy", capture_height, NULL);

  desktop_pipeline = gst_pipeline_new ("desktop");

  gst_bin_add_many (GST_BIN (desktop_pipeline), imagesrc, ffmpegcolor, videosink, NULL);
  gst_element_link_many (imagesrc, ffmpegcolor, videosink, NULL);

  gst_element_set_state(desktop_pipeline, GST_STATE_PLAYING);
  return screen_tex;
}


ClutterActor* new_webcam_actor() {

  webcam_tex = clutter_texture_new();
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), webcam_tex);
  clutter_actor_set_position(webcam_tex, capture_width - 1.3*webcam_width, capture_height - 1.3*webcam_height);
  clutter_actor_set_size(webcam_tex, webcam_width, webcam_height);

  GstElement* videosink = clutter_gst_video_sink_new ((ClutterTexture *) webcam_tex);
  //g_object_set(G_OBJECT(videosink), "use-shaders", FALSE, NULL);
  GstElement* v4l2src = gst_element_factory_make ("v4l2src", NULL);
  GstElement* ffmpegcolor = gst_element_factory_make ("ffmpegcolorspace", NULL);

  g_object_set(G_OBJECT(v4l2src), "device", "/dev/video0", NULL);

  webcam_pipeline = gst_pipeline_new ("webcam");

  gst_bin_add_many (GST_BIN (webcam_pipeline), v4l2src, ffmpegcolor, videosink, NULL);
  gst_element_link_many (v4l2src, ffmpegcolor, videosink, NULL);

  gst_element_set_state(webcam_pipeline, GST_STATE_PLAYING);
  return webcam_tex;
}

static GMainLoop *main_loop = NULL;
int main(int argc, char *argv[]) {

	if(g_thread_supported() == FALSE)
	{
		g_thread_init(NULL);
	}

	gst_init(NULL, NULL);

	main_loop = g_main_loop_new(NULL, FALSE);

  int retval = clutter_init (&argc, &argv);

  ClutterColor stage_color = { 0, 0, 50, 255 };

  stage = clutter_stage_get_default();
  clutter_actor_set_size (stage, capture_width, capture_height);
  clutter_stage_set_color (CLUTTER_STAGE(stage), &stage_color);

  thescreen = new_screen_actor(); 
  thewebcam = new_webcam_actor(); 

  clutter_actor_show_all (stage);

  recorder = shell_recorder_new (CLUTTER_STAGE (stage));
  shell_recorder_set_filename (recorder, "ramidus.webm");
  //start recording
  shell_recorder_record (recorder);
  //and stop after 15 secs
  g_timeout_add (15000, stop_recording_timeout, NULL);

	g_main_loop_run(main_loop);
  clutter_main ();

  return EXIT_SUCCESS;
}
