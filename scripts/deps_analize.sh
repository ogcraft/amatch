 find . -name "*.cpp" | xargs gcc -I ./src -I ./include -I ./android/jni -MM
