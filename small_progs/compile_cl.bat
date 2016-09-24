::VS2010 Environment
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"
::include
::@set INCLUDE=include;%INCLUDE%
::lib
::@set LIB=lib;%LIB%
::compile and link
::cl simplest_ffmpeg_video_filter.cpp /MD /link SDL.lib SDLmain.lib avcodec.lib ^
::avformat.lib avutil.lib avdevice.lib avfilter.lib postproc.lib swresample.lib swscale.lib ^
::/SUBSYSTEM:WINDOWS /OPT:NOREF
cl psnr.c  
exit
