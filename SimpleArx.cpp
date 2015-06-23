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
#include "json\json.h"
#include <fstream>
#include <direct.h>

using namespace std;

//**************************************************************
void test()
//**************************************************************
{
	try
	{
        // Read the parameter file name and output folder name.
		ACHAR szFile[128] = _T("");
		ACHAR szFolder[128] = _T("");
		int res = acedGetString(1, _T("Specify parameter file: "), szFile);
		if(res != RTNORM)
			return;
		res = acedGetString(1, _T("Specify output sub-folder name: "), szFolder);
		if(res != RTNORM)
			return;

        // read and parse the parameter file (in Json format)
		Json::Reader reader;
		Json::Value root;
		ifstream inputFile(szFile);
		bool parseresult = reader.parse(inputFile, root);
        if(parseresult == false)
            return;
        Json::Value layerNames = root.get("ExtractLayerNames", layerNames);
        Json::Value blockNames = root.get("ExtractBlockNames", blockNames);

        // create the output folder.
        _wmkdir(szFolder);

        // extract layer and blocktable info.
        AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();
        if(layerNames.asBool())
        {
            wstring outputfile = szFolder;
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
        }

        if(blockNames.asBool())
        {
            wstring outputfile = szFolder;
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
