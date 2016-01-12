# Directory Structure

## `armv7m_ti`

This contains a modified version of the armv7 package used by the majority of ARM projects using this RTOS. The major changes are:

    - A new version of Kochab that includes message queues, which have been torn out of the rigel variant and modified slightly.
    - More lenient parsing, and better error messages when doing bitband replacement so that the vector table script doesn't disagree with TI's libraries.

## `build.py`

A custom build script that makes it so prj happily munches through TI's frameworks and lwIP, with some added functionality for productivity:

    - Defines a bunch of flags that need to be set to compile TI's libraries and lwIP.
    - Sets the architecture properties properly
    - Dumps all additional include_paths used in .prx declarations to a .ycm file in the repo root (Why? => this means that ycm [most common autocompletion engine for vim] can find all your symbols properly)

## `projects`

This contains actively maintained projects on this platform; currently:

    - An IoT light dimmer controlled via telnet (Uses PWM; relays)
    - A remote regression server echo server controlled via telnet (Uses UART, Relays)

## `util`

Contains source files used by all projects. Currently only the lwIP configuration header - however this could be extended to more common code in the future (i.e telnet module is currently used in both projects)

## `ti_libs`

Heavily modified & stripped down version of the TI libraries for this chip. Includes lwIP ported from FreeRTOS to eChronos in the third_party directory. Also contains some useful tools when creating resources for lwIP systems.
