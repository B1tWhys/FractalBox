#pragma once

/*
This is where the code for the command line GUI lives. The App initializes an instance
of this object (as well as an Engine object), and then calls run() on this object, which
starts the loop which presents all the command line stuff to the user. 

When a change is made here, a new Settings struct is created and then passed to the App
singleton via a callback. The App object then does the appropriate stuff to update the
Engine (usually it updates Engine's Settings struct, then calls the funciton to rebuild
the render pipeline).
*/

// TODO: Implement the CLI