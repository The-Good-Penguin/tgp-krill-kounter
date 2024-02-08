# Release v0.02

Bug fix release, resolving:
 *  CJsonParser: fix error handling in JSON parsing
 *  incorrect cleanup of json error bug
 *  main: only update stats and write file when needed
 *  Wrong sequence order when assesing errors on json_reader_read_member. bug
 *  cJsonParser: fix SIGSEGV in getValueAsInt
 *  main: fix log messages
 *  main: Fix stats updating
 *  main: Change stat types to gint64
 *  main: Add stats directory check
 *  cStatReader: fixed getSerialNumberFallback logs
 *  getSerialNumberFallback log messages are incomplete bug
 *  totalBytesWritten is a double invalid
 *  KrillKounter: add service restart
 *  CMakeLists: remove unused sigc++
 *  Fix GLib assertion errors
 *  CMakeLists: add missing default directory
 *  Update systemd instructions documentation
 *  Odd journal output bug
 *  totalBytesWritten gets reset to 0! bug
 *  GLib assertion failures bug
 *  Stats file is written when not necessary bug
 *  Default directory path for stats isn't present bug
 *  Systemd service should restart enhancement
 *  Systemd service install enhancement
 *  cJsonParser: Mutliple members missing causes SIGSEGV

# Release v0.01

Initial beta release for integration testing
