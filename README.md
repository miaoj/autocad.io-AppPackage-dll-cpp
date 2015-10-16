# autocad.io-AppPackage-dll-cpp 

[![ver](https://img.shields.io/badge/AutoCAD.io-2.0.0-blue.svg)](https://developer.autodesk.com/api/autocadio/v2/)
[![License](http://img.shields.io/:license-mit-red.svg)](http://opensource.org/licenses/MIT)

##Description

This is a AutoCAD arx application written in C++. It mimics the same functionality written in C# at https://github.com/Developer-Autodesk/autocad.io-custom-activity-with-apppackage-CSharp. 

To successfully open the project, please edit the crxapp.vcxproj and set <ObjectARXPath> element to the correct ObjectArx path.

There is a post-build process that creates the auto-loadable zip package called package.zip at the output folder. Please look at the code at https://github.com/Developer-Autodesk/autocad.io-custom-activity-with-apppackage-CSharp about how to upload it as AppPackage. It does the same thing as the .NET crxapp project there.

## Questions

Please post your question at our [forum](http://forums.autodesk.com/t5/autocad-i-o/bd-p/105).

## License

These samples are licensed under the terms of the [MIT License](http://opensource.org/licenses/MIT). Please see the [LICENSE](LICENSE) file for full details.

##Written by 

Jonathan Miao

