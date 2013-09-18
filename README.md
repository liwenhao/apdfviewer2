apdfviewer2
===========

apdfviewer2 is a pdf viewer for android based on poppler, it is a rewrite version of the old apdfviewer at googlecode

You can find more infomation about the old apdfviewer at https://code.google.com/p/apdfviewer/

Why rewrite?
------------

- The old apdfviewer use android private libraries, must be compiled with android system.
- Android build in freetype library does not support type1 font.

What's new?
-----------

- build with NDK

How to build
------------

- Put source of freetype into jni/external/freetype2
- Put source of poppler into jni/external/poppler (version 0.18.4)
- Then build with ADT and NDK
