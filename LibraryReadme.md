KrillKounter Library
-------------------

## cStatReader

**findDevices**

Returns: *std::vector\<std::string\>*

Find every block device available under */dev/*, return results as a Vector of Strings. This function takes no arguments.

**getSpaceInfo**

Returns: *bool*

*std::string deviceName*

*uintmax_t\* pValue*

Retreive the total size of a connected block device, in bytes. The name of the device is provided via *deviceName*, in the form "XYZ", where the target device is located at `/dev/XYZ`. Results are provided as a `uintmax_t` value, via pointer *pValue*. Returns `true` on success, `false` on failure.

**getStats**

Returns: *bool*

*std::string deviceName*

*struct sBlockStats\* pStats*

Retrieve stats for a connected block device. The name of the device is provided via *deviceName*, in the form "XYZ", where the target device is located at `/dev/XYZ`. Results are returned as a pointer to a `sBlockStats` struct, via *pStats*. Returns `true` on success, `false` on failure.

**getSpecs**

Returns: *bool*

*std::string deviceName*

*struct sDeviceSpecs\* pSpecs*

Retrieve specs for a connected block device. The name of the device is provided via *deviceName*, in the form "XYZ", where the target device is located at `/dev/XYZ`. The primary method attempts to access the CID of the target device, although this is not possible in all scenarios. If the primary method fails, a secondary method using the `lsblk` utility is attempted, although this method only provides the device serial number and none of the other device specs. Results are returned as a pointer to a `sDeviceSpecs` struct, via *pSpecs*. Each entry in `sDeviceSpecs` is a smaller struct of type `sBlockStatStub`, which contains both a "value" and "enabled" variables. For each spec successfully retreived by *getSpecs()*, the "enabled" variable will be set to `true`, and the "value" variable will be set the the retreived value for that spec.
*getSpecs* returns `true` on success, and `false` on failure.

## cStatComputer

**getAverageWriteSize**

Returns: *uint*

*struct sBlockStats\* pDeviceStats*

*uint sectorSize*

Calculate average size of writes to a given block device. The values of *writeSectors* and *writeIo* within *pDeviceStats* must be valid, and *sectorSize* must match the sector size used by the block device, in bytes. *getAverageWriteSize()* returns the calculated value as a `uint`.

**totalBytesWritten**

Returns: *float*

*uint sectorSize*
    
*uintmax_t currentWriteSectors*

*uintmax_t previousWriteSectors*

*float previousTotal*

Calculates to total number of bytes written to a block device. Requires the sector size of the target device (*sectorSize*), the current value of the device's write sector stat (*currentWriteSectors*), the previous value of the device's write sector stat (*previousWriteSectors*), and the previous value for the total bytes written (*previousTotal*). Returns the total bytes written value as a float.
