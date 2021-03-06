===============
# The Params File
===============

The params file consists of several group definitions.  Objects of class
HyPerConn, HyPerLayer, or their derived classes typically read parameters
from the params file by querying the group whose name matches the object's
name.  The params file belonging to a HyPerCol is available to other
functions (such as main) using the HyPerCol::parameters() function.

The basic format of the params file is

// Begin sample params file
Keyword1 "group 1 name" = {
    parameter1 = value1;
    parameter2 = value2;
    // etc.
};

Keyword2 "group 2 name" = {
    parameter1 = value1; // Use C++ style double slashes for comments.
    parameter2 = value2;
    // etc.
};

// et cetera
// End sample params file

Each group consists of a keyword, a group name, and a list of parameter
definitions.

The keyword is a nonempty string whose characters must be letters (either
case), numerals, or the low line (underscore).  The first character must be
a letter (either case).  Any such keyword is permissible, with the exception
of "infinity", "true", and "false", which will be parsed as numbers; and
"FileDefinition", although the FileDefinition functionality is deprecated
and the restriction may be removed. Generally the keyword is used to
indicate the class of the object defined by the group.

The group name is a string whose characters must be letters (either case),
numerals, the low line (underscore), or the space.  It must be enclosed in
quotation marks.  Two groups should not have the same group name, although
this is not enforced.  If two groups have the same name, querying that name
for a parameter searches the first group with that name; parameters defined
in the second group will not be retrieved.

The parameter definitions have the form "parametername = value;"  Allowed
parameter names are the same as allowed keywords.  The value can be either a
number or a string.

Numbers can have any of the format for finite numbers understandable by the
scanf %f conversion.  In addition, the values "infinity", "+infinity",
"-infinity", "true" and "false" are acceptable.  The quotes are not
included, or else the value is interpreted as a string.  infinity is replaced
by the value of FLT_MAX in float.h; true is replaced by 1; and false by zero.
The match is case sensitive, so that "True", "TRUE", etc., give a syntax
error.

Strings must be included in quotes.  They can be in one of two formats:
(1)  the same format as the group name:  the allowed characters are letters,
     numerals, low lines and spaces.
(2)  The allowed characters are letters, numerals, and the characters
     '_', '~', '+', '/', '.', '-' (but not space).  This possibility is
     used to set filenames.

Querying the params file is done using the PVParams methods
    int          present(const char * groupName, const char * paramName)
    float        value(const char * groupName, const char * paramName)
    float        value(const char * groupName, const char *paramName, float initialValue)
    int          stringPresent(const char * groupName, const char * paramStringName)
    const char * stringValue(const char * groupName, const char * paramStringName)

==================
# Parameter Sweeps
==================

Params files can define more than one run using ParameterSweep blocks.  They have
the format

ParameterSweep "group name":sweptParameter = {
    value1;
    value2;
    value3;
    //etc.
};

There can be more than one ParameterSweep statement, but all must have the same
number of values.  ParameterSweeps can be defined for string values as well.
The format is

ParameterSweep "group name":parameterString = {
    "string_a";
    "string_b";
    "string_c",
    //etc.
};


If a params file with at least one ParameterSweep is passed to buildandrun,
several runs are performed in a loop.  The number of runs is the same as the 
number of values in the ParameterSweep.  On the first run, the group named
"group name" has the parameter sweptParameter set to value1.  On the second
run, sweptParameter is set to value2, and so on.

If there is more than one ParameterSweep, the first run uses the first value
of each ParameterSweep, then the second run uses the second value of each
ParameterSweep, and so on.  The sweeps are not nested.  This is why each
sweep must have the same number of values.

If there isn't a ParameterSweep for the HyPerCol's outputPath, one is
generated internally, using values "output0/", "output1/", "output2/", etc. 
If the number of sweeps is more than ten, the directories will be
"output00/", "output01/", and so on.  This is so that the directories appear
in a standard ls listing in the proper order.
