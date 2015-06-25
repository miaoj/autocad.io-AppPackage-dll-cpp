// (C) Copyright 1997-1999 by Autodesk, Inc.
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted,
// provided that the above copyright notice appears in all copies and
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS.
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC.
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//
//    The close-to-minimal ARX program demonstate how to make a custom
//    text-looking entity to be displayed parallel to the current view screen.
//    The user would expand this program by allowing some user inputs such as
//	  getting a point to locate the custom entity to a user desired position.
//
//    The application defines an AutoCAD command my_text.
//

#include "StdAfx.h"
#include <dbobjptr2.h>
#include <fstream>
#include <direct.h>
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/encodedstream.h"

using namespace std;
using namespace rapidjson;

//**************************************************************
void test()
//**************************************************************
{
	try
	{
        acutPrintf(_T("SimpleArx starts.\n"));

        // Read the parameter file name and output folder name.
        ACHAR szString[256] = _T("");
        int res = acedGetString(1, _T("Specify parameter file and sub-folder name: "), szString);
		if(res != RTNORM)
			return;

        // extract parameter file name and sub-folder name
        AcString fullStr = szString;
        int spaceSperatorIndex = fullStr.find(_T(" "));
        AcString paramFileName = fullStr.substr(0, spaceSperatorIndex);
        AcString outputFolderName = fullStr.substr(spaceSperatorIndex + 1, fullStr.length() - spaceSperatorIndex);
        acutPrintf(_T("The parameter file name is: %s\n"), paramFileName);
        acutPrintf(_T("The output sub-folder name is: %s\n"), outputFolderName);

        // Parse json file using rapidjson library -- check client code at https://github.com/miaoj/workflow-custom-activity-with-apppackage-autocad.io
        // The WorkItem includes an inline json string as:
        // Resource = @"data:application/json, "+ JsonConvert.SerializeObject(new CrxApp.Parameters { ExtractBlockNames = true, ExtractLayerNames = true })
        // which will be saved into "params.json" file in UTF-16 format. 
        // We parse the json file here using rapidjson library.
        TCHAR cDir[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, cDir);
        acutPrintf(_T("current search folder is: %s\n"), cDir);

        FILE *fp;
        bool extractLayerNames = false;
        bool ExtractBlockNames = false;
        try
        {
            _wfopen_s(&fp, _T("params.json"), _T("r"));
            GenericReader<AutoUTF<unsigned>, UTF16<> > reader;     
            char readBuffer[65536];
            FileReadStream is(fp, readBuffer, sizeof(readBuffer));
            AutoUTFInputStream<unsigned, FileReadStream> eis(is);
            Document doc;
            doc.ParseStream<AutoUTFInputStream<unsigned, FileReadStream>>(eis);
            extractLayerNames = doc[("ExtractLayerNames")].GetBool();
            ExtractBlockNames = doc[("ExtractBlockNames")].GetBool();
            fclose(fp);
        }
        catch(exception e)
        {
            fclose(fp);
        }

        // create the output folder.
        _wmkdir(outputFolderName);

        // extract layer and blocktable info.
        AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();
        if(extractLayerNames)
        {
            wstring outputfile = outputFolderName;
            outputfile.append(_T("\\layers.txt"));
            wofstream output (outputfile);
            AcDbSmartObjectPointer<AcDbLayerTable> layerTable( db->layerTableId(), AcDb::kForRead );
            if(layerTable.openStatus() != ErrorStatus::eOk)
                return;
            AcDbLayerTableIterator* pLayerIter = NULL;
            if(layerTable->newIterator(pLayerIter) != ErrorStatus::eOk || NULL == pLayerIter)
                return;
            for(pLayerIter->start(); !pLayerIter->done(); pLayerIter->step())
            {
                AcDbLayerTableRecord* pLayer = NULL;
                if(pLayerIter->getRecord(pLayer, AcDb::kForRead) != ErrorStatus::eOk || pLayer == NULL)
                    continue;
                AcString layerName;
                pLayer->getName(layerName);
                output.write(layerName.kACharPtr(), layerName.length());
                output.write(_T("\n"),1);
                pLayer->close();
            }
            delete pLayerIter;
            layerTable.close();
            output.close();

            acutPrintf(_T("The layer file is at: %s\n"), outputfile.c_str());
        }

        if(ExtractBlockNames)
        {
            wstring outputfile = outputFolderName;
            outputfile.append(_T("\\blocks.txt"));
            wofstream output (outputfile);
            AcDbSmartObjectPointer<AcDbBlockTable> blockTable( db->blockTableId(), AcDb::kForRead );
            if(blockTable.openStatus() != ErrorStatus::eOk)
                return;
            AcDbBlockTableIterator* pBlockIter = NULL;
            if(blockTable->newIterator(pBlockIter) != ErrorStatus::eOk || NULL == pBlockIter)
                return;
            for(pBlockIter->start(); !pBlockIter->done(); pBlockIter->step())
            {
                AcDbBlockTableRecord* pBlock = NULL;
                if(pBlockIter->getRecord(pBlock, AcDb::kForRead) != ErrorStatus::eOk || pBlock == NULL)
                    continue;
                AcString blockName;
                pBlock->getName(blockName);
                output.write(blockName.kACharPtr(), blockName.length());
                output.write(_T("\n"),1);
                pBlock->close();
            }
            delete pBlockIter;
            blockTable.close();
            output.close();

            acutPrintf(_T("The block file is at: %s\n"), outputfile.c_str());
        }
	}
	catch(exception e)
	{

	}
}

//**************************************************************
extern "C"
AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void *pkt)
//**************************************************************
{
	switch(msg)
	{
		case AcRx::kInitAppMsg:
			acrxDynamicLinker->unlockApplication(pkt);
			acrxDynamicLinker->registerAppMDIAware(pkt);
			acrxBuildClassHierarchy();
			acedRegCmds->addCommand(_T("SIMPLETEST"), _T("test"), _T("test"), ACRX_CMD_MODAL, test);
			break;
		case AcRx::kUnloadAppMsg:
			acedRegCmds->removeGroup(_T("SIMPLETEST"));
			break;
		default:
			break;
	}
	return AcRx::kRetOK;
}
