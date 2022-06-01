# DCMTK

This DICOM ToolKit (DCMTK) package consists of source code, documentation and installation instructions for a set of software libraries and applications implementing part of the DICOM/MEDICOM Standard.

DCMTK contains the following sub-packages, each in its own sub-directory:

- **config**   - configuration utilities for DCMTK
- **dcmdata**  - a data encoding/decoding library and utility apps
- **dcmect**   - a library for working with Enhanced CT objects
- **dcmfg**    - a library for working with functional groups
- **dcmimage** - adds support for color images to dcmimgle
- **dcmimgle** - an image processing library and utility apps
- **dcmiod**   - a library for working with information objects and modules
- **dcmjpeg**  - a compression/decompression library and utility apps
- **dcmjpls**  - a compression/decompression library and utility apps
- **dcmnet**   - a networking library and utility apps
- **dcmpmap**  - a library for working with parametric map objects
- **dcmpstat** - a presentation state library and utility apps
- **dcmqrdb**  - an image database server
- **dcmrt**    - a radiation therapy library and utility apps
- **dcmseg**   - a library for working with segmentation objects
- **dcmsign**  - a digital signature library and utility apps
- **dcmsr**    - a structured reporting library and utility apps
- **dcmtls**   - security extensions for the network library
- **dcmtract** - a library for working with tractography results
- **dcmwlm**   - a modality worklist database server
- **oficonv**  - a character set conversion library
- **oflog**    - a logging library based on log4cplus
- **ofstd**    - a library of general purpose classes

Each sub-directory (except _config_) contains further sub-directories for application source code (_apps_), library source code (_libsrc_), library include files (_include_), configuration data (_etc_), documentation (_docs_), sample and support data (_data_) as well as test programs (_tests_).

To build and install the DCMTK package see the [INSTALL](INSTALL) file.  For copyright information see the [COPYRIGHT](COPYRIGHT) file.  For information about the history of this software see the [HISTORY](HISTORY) file.  For answers to frequently asked questions please consult the [FAQ](https://forum.dcmtk.org/faq/).

In addition to the API documentation, which is also available [online](https://support.dcmtk.org/docs/), there is a [Wiki](https://support.dcmtk.org/wiki/) system where further information (e.g. HOWTOs) can be found.

If you find bugs or other problems with this software, we would appreciate hearing about them.  Please send electronic mail to: bugs/at/dcmtk/dot/org

Please try to describe the problem in detail and if possible give a suggested fix.  For general questions on how to compile, install or use the toolkit we recommend the [public discussion forum](https://forum.dcmtk.org/).
