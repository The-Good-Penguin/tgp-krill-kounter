#include <glib.h>

#include <json-glib/json-glib.h>

#include "utils/log-event.hh"

int main(int argc, char* argv[])
{
    LogEventInit(basename(argv[0]), 6);

    LOG_EVENT(LOG_INFO, "Starting app.");

    LogEventDeinit();

    return EXIT_SUCCESS;
}