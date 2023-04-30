This project should be built using Visual Studio on a Windows PC (which uses x86).
The TB.sln file should ideally import the project properly. The reason for our
reliance on Visual Studio is due to the usage of naked functions in trigger_bot.cpp
which is not allowed by gcc. I have noticed that VMs don't end up running the code as
expected so it is best to run the code directly on a windows machine.

You must change the dll_path variable in dll_injector.cpp after building the project to point
to the newly built dll. I apologize for the inconvenience. In my experience, the dll gets loaded
when using the absolute path but not when using a relative path. It is likely that the function
responsible for loading the dll acts a little differently than what I might be expecting of it.
This is something that I would like to fix in the future.

Note: Naked functions are functions that don't set up a stack frame. They leave it to the
user to safely enter and exit the function. We need to use this function because our code involves 
using a jump statement to transfer control flow when we are done with our nefarious activities.