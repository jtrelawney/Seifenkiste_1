normally when the libs are compiled the scripts in each lib directory is called
later when all libs are build as a project the subdirs are included
but at some point this would require changing all subdir scripts when adjsutements to teh compile settings are required (debug -> release)


simple example to test cmake options
define compiler settigs for debug and release
in main dir compile hello world and output settings
in subdir compile class hello world with either caller settings or local settings and check that compiler settings are correct and output settings

also define a global dir for the libs and the executables



the subdir scripts find out if a the option USE_CALLER_CONFIG is set and then uses the given parameters

also tried to define a variable and trasport the settings, this works top down when the caller exists and has the variable defined
it doesn't work when the script is called, then the varaible is not defined


this version sets the cmake flags directly, ti would be better to use targets, see v2





