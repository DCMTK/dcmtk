#include "osconfig.h"

#include "dviface.h"
#include "dvpssp.h"
#include "diutils.h"


int main()
{
    DicomImageClass::setDebugLevel(DicomImageClass::DL_Warnings);
    DVInterface dvi("test.cfg", OFTrue);
/*
    dvi.startQueryRetrieveServer();
    dvi.terminateQueryRetrieveServer();
*/
/*
    dvi.startPrintServer();
    dvi.terminatePrintServer();
*/
/*
    dvi.selectStudy(0);
    dvi.loadImage(dvi.getStudyUID(), dvi.getSeriesUID(), dvi.getInstanceUID());
    dvi.selectInstance(1);
  cout << dvi.addImageReferenceToPState(dvi.getStudyUID(), dvi.getSeriesUID(), dvi.getInstanceUID());
  cout << dvi.saveCurrentPStateForReset() << endl;
  cout << dvi.resetPresentationState() << endl;
    dvi.savePState("pstate2.dcm");
*/
/*
    dvi.selectStudy(0);
    dvi.selectSeries(1);
    dvi.loadPState(dvi.getStudyUID(), dvi.getSeriesUID(), dvi.getInstanceUID());
    dvi.loadReferencedImage(1);
    dvi.savePState();
*/
/*
    cout << dvi.loadStoredPrint("database/sp_1.dcm") << endl;
    cout << dvi.getPrintHandler().getOriginator() << endl;
    cout << dvi.getPrintHandler().getDestination() << endl;
    cout << dvi.getPrintHandler().getPrinterName() << endl;
    cout << dvi.loadStoredPrint("database/sp_2.dcm") << endl;
    cout << dvi.getPrintHandler().getOriginator() << endl;
    cout << dvi.getPrintHandler().getDestination() << endl;
    cout << dvi.getPrintHandler().getPrinterName() << endl;
*/
/*
    cout << dvi.getPrintHandler().setOriginator("DCMPSTATE") << endl;
    cout << dvi.setCurrentPrinter("AGFA") << endl;
    cout << dvi.saveStoredPrint("sp_new.dcm", OFFalse) << endl;
*/
/*
    dvi.selectStudy(0);
    if (dvi.loadStoredPrint(dvi.getStudyUID(), dvi.getSeriesUID(), dvi.getInstanceUID()) == EC_Normal)
    {
        dvi.setMaxPrintPreviewWidthHeight(150, 200);
        if (dvi.loadPrintPreview(0) == EC_Normal)
        {
            unsigned long size = dvi.getPrintPreviewSize();
            char *buffer = new char[size];
            if (buffer != NULL)
            {
                unsigned long width, height;
                if (dvi.getPrintPreviewWidthHeight(width, height) == EC_Normal)
                    cout << "W/H: " << width << " / " << height << endl;
                if (dvi.getPrintPreviewBitmap(buffer, size) == EC_Normal)
                {
                    FILE *file = fopen("test.raw", "wb");
                    if (file)
                    {
                        fwrite(buffer, 1, size, file);
                        fclose(file);
                    }
                }
                delete[] buffer;
            }
        }
    }
*/
/*
    dvi.selectStudy(0);
    dvi.loadImage(dvi.getStudyUID(), dvi.getSeriesUID(), dvi.getInstanceUID());
    dvi.getCurrentPState().setGammaVOILUT(0.5);
*/
/*
    dvi.setLogFilter(DVPSM_error);
    dvi.writeLogMessage(DVPSM_warning, "GENERAL 1", "Hello World!\nNew line.");
    dvi.setLogFilter(DVPSM_warning);
    dvi.writeLogMessage(DVPSM_warning, "GENERAL 2", "Hello World!\nNew line.");
*/
/*
    for (int i = 0; i < 400; i++)
    {
        cout << i << "\t";
        cout << dvi.convertODtoLum(i, 50, 350, 8) << "\t";
        cout << dvi.convertODtoLum(i, 50, 350, 12) << "\t";
        cout << dvi.convertODtoLum(i, 50, 350, 16) << endl;
    }
*/
}
