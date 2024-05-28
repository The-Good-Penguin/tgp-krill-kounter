KrillKounter Daemon
-------------------

## cJsonParser

**openJson**

Returns: *bool*

*std::string jsonPath*

Open a JSON file located at *jsonPath*. The JSON file must be formatted using the schema defined in `examples/test-sd-reference.json`. Returns `true` on success, `false` on failure.

**closeJson**

Returns: *bool*

Close the JSON file previously opened with `openJson`. Returns `true` on success, `false` on failure.

**getTotalBytesWritten**

Returns: *bool*

*std::string serialNumber*

*int\* pValue*

Retreive the `totalBytesWritten` value in the entry with the key *serialNumber* from the JSON file previously opened with `openJson`. The value of `totalBytesWritten` is written to *pValue*. Returns `true` on success, `false` on failure.

**getStats**

Returns: *bool*

*std::string serialNumber*

*struct sBlockStats\* pStats*

Retreive the `previousStats` value in the entry with the key *serialNumber* from the JSON file previously opened with `openJson`. The value of `previousStats` is written to *pStats* as a `sBlockStats` struct defined in `src/library/include/structs.hh`. Returns `true` on success, `false` on failure.

**getPath**

Returns: *bool*

*std::string serialNumber*

*std::string\* pValue*

Retreive the `previousPath` value in the entry with the key *serialNumber* from the JSON file previously opened with `openJson`. The value of `previousPath` is written to *pValue*. Returns `true` on success, `false` on failure.

**getFirstSightingDate**

Returns: *bool*

*std::string serialNumber*

*std::string\* pValue*

Retreive the `firstSightingDate` value in the entry with the key *serialNumber* from the JSON file previously opened with `openJson`. The value of `firstSightingDate` is written to *pValue*. Returns `true` on success, `false` on failure.

**getSerialNumbers**

Return: *bool*

*std::vector\<std::string\>\* pValue*

Retreive all serial numbers from the JSON file previously opened with `openJson`. The output is written to *pValue* as a vector of strings. Returns `true` on success, `false` on failure.

## cJsonWriter

**writeJson**

Return: *bool*

*std::string jsonPathInput*

*std::string jsonPathOutput*

*std::string serialNumber*

*std::string firstSightingDate*

*std::string previousPath*

*struct sBlockStats\* pStats*

*uintmax_t totalBytesWritten*

Write data to a JSON file using the schema defined in `examples/test-sd-reference.json`. First a data set is loaded from a JSON file *jsonPathInput*. If the JSON data has an entry with a matching serial number to the one provided with *serialNumber*, then that entry is updated with the values from *previousPath*, *pStats*, and *totalBytesWritten*. If no entry with a matching serial number is found, then a new entry is created with *serialNumber*, containing the values from *previousPath*, *pStats*, and *totalBytesWritten*. If there is no file present at *jsonPathInput*, then the JSON data set will only contain the newest entry, defined by the values of *serialNumber*, *previousPath*, *pStats*, and *totalBytesWritten*. The JSON data set is then written to a JSON file with the path *jsonPathOutput*, also using the schema defined in `examples/test-sd-reference.json`. If no file is present at *jsonPathOutput*, then a new file will be created and written to.
Returns `true` on success, `false` on failure.
