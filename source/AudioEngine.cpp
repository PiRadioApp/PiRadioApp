#include "AudioEngine.hpp"
#include <stdexcept>
#include <memory>

AudioEngine::AudioEngine(const char* url){
    int Argc = 1;
    char** Argv = new char* ();
    Argv[0] = "foo";
    gst_init (&Argc, &Argv);

    loop = g_main_loop_new(NULL, FALSE);

    char* playbinText = "playbin uri=https://stream.open.fm/127?type=.mp3";
    pipeline = gst_parse_launch("playbin uri=https://stream.open.fm/127?type=.mp3", NULL);

    if (!pipeline){
        g_printerr("Pipeline could not be created. Exiting...\n");
        throw "One element of AudioEngine could not be created!";
    }

    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    bus_watch_id = gst_bus_add_watch(bus, bus_call, loop);
}

gboolean AudioEngine::bus_call(GstBus *bus, GstMessage *msg, gpointer data){
    GMainLoop *loop = (GMainLoop *) data;

    switch (GST_MESSAGE_TYPE (msg)) {

      case GST_MESSAGE_EOS:
        g_print ("End of stream\n");
        g_main_loop_quit (loop);
        break;

      case GST_MESSAGE_ERROR: {
        gchar  *debug;
        GError *error;

        gst_message_parse_error (msg, &error, &debug);
        g_free (debug);

        g_printerr ("Error: %s\n", error->message);
        g_error_free (error);

        g_main_loop_quit (loop);
        break;
      }
      default:
        break;
    }

    return TRUE;
}

bool AudioEngine::setState(AudioState state){
    switch(state)
    {
    case AudioState::PLAY:
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
        g_print ("Running...\n");
        g_main_loop_run(loop);
        break;
    case AudioState::PAUSE:
        gst_element_set_state (pipeline, GST_STATE_PAUSED);
        g_print ("Paused,\n");
        break;
    case AudioState::STOP:
        g_print ("Returned, stopping playback\n");
        gst_element_set_state (pipeline, GST_STATE_NULL);
        g_main_loop_quit (loop);
        break;
    }
}





















