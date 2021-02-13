ABOUT THIS DISTRIBUTION
=======================

My complements go to Alessio Massaro for sharing his work with the rest of the world.
This distribution is built upon his. The reason I have done this is Alessio didn't include
gcc library file (*.a) with his distribution, so extra work was needed to use fftw3 with 
MinGW. Everyone is welcome to build their own distributions using this distribution as a
basis.

INSTALLATION NOTES
==================

For Microsoft Visual C++:
1. Copy fftw3.lib into Lib directory inside Microsoft Visual C++ directory
2. Copy fftw3.h into Include directory inside Microsoft Visual C++ directory
3. Copy fftw3.dll into Windows\System32 directory
4. Be sure to distribute fftw3.dll with compiled programs (e.g. inside program directory)

For MinGW:
1. Copy libfftw3.a into Lib directory inside MinGW directory
2. Copy fftw3.h into Include directory inside MinGW directory
3. Copy fftw3.dll into Windows\System32 directory
4. Be sure to distribute fftw3.dll with compiled programs (e.g. inside program directory)
