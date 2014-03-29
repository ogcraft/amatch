/*
amatch_interface.i:
SWIG interface file to amatch functions
*/

%module amatch_interface
%include "arrays_java.i";
%{
#include "amatch_interface.h"
%}

// Enable the JNI class to load the required native library.
%pragma(java) jniclasscode=%{
  static {
    try {
        java.lang.System.loadLibrary("amatch");
    } catch (UnsatisfiedLinkError e) {
        java.lang.System.err.println("native code library failed to load.\n" + e);
        java.lang.System.exit(1);
    }
  }
%}

%include "amatch_interface.h"
