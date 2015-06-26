# autocad.io-AppPackage-dll-cpp 
This is a AutoCAD arx application written in C++. It mimics the same functionality written in C# at https://github.com/Developer-Autodesk/workflow-custom-activity-with-apppackage-autocad.io. 

To successfully open the project, please edit the crxapp.vcxproj and set <ObjectARXPath> element to the correct ObjectArx path.

There is a post-build process that creates the auto-loadable zip package called package.zip at the output folder. Please look at the code at https://github.com/Developer-Autodesk/workflow-custom-activity-with-apppackage-autocad.io about how to upload it as AppPackage. It does the same thing as the .NET crxapp project there.

This project has be tested. If any issue, please contact jonathan.miao@autodesk.com. 

