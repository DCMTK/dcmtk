The libcharls library containing the JPEG-LS encoder and decoder in this
DCMTK module is based on the CharLS library (see http://charls.codeplex.com/).

The CharLS library is available under the license that can be found in
dcmjpls/docs/License.txt.
------------------------------------------------------------------------------

The current CharLS version used is revision 55020 dated 2010-10-05.

The following modifications have been applied to the library:
- removed "test" directory
- removed project files (CharLS.sln, CharLS.vcproj)
- renamed implementation file extensions from .cpp to .cc
- made file names fit into 8.3 characters
- converted file to use UNIX line feeds instead of Windows CR/LF
- removed trailing whitespace, purified tab usage
